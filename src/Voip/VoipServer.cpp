#include "Voip.hpp"
#include "VoipTokenManager.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/String.hpp"
#include "../Blam/BlamNetwork.hpp"

#include <teamspeak/serverlib.h>
#include <teamspeak/public_errors.h>
#include <cstring>

#include <Windows.h>

using namespace Voip;

namespace
{
	const char *DefaultServerIp = "0.0.0.0";     // Bind to everything
	const unsigned int DefaultServerPort = 9987; // Default TeamSpeak port
	const char *DefaultServerName = "ElDewrito In-Game VoIP Server";

	bool Initialized = false;
	bool Started = false;
	uint64 ServerId;

	bool Initialize();
	DWORD WINAPI StartThread(LPVOID lpParameter);

	void LogError(unsigned int error);

	void OnClientPasswordEncrypt(uint64 serverId, const char *plaintext, char *encryptedText, int encryptedTextByteSize);
	unsigned int OnCustomServerPasswordCheck(uint64 serverId, const struct ClientMiniExport *client, const char *password);
	void OnClientConnected(uint64 serverId, anyID clientId, uint64 channelId, unsigned int *removeClientError);
	void OnClientDisconnected(uint64 serverId, anyID clientId, uint64 channelId);
	void OnUserLoggingMessageEvent(const char *logMessage, int logLevel, const char *logChannel, uint64 logId, const char *logTime, const char *completeLogString);
	unsigned int PermClientKickFromChannel(uint64 serverId, const struct ClientMiniExport *client, int toKickCount, struct ClientMiniExport *toKickClients, const char *reasonText);
	unsigned int PermClientUpdate(uint64 serverId, anyID clientId, struct VariablesExport *variables);

	anyID PeerClients[Blam::Network::MaxPeers];
	Blam::Network::PeerBitSet ValidPeerClients;

	void RegisterClient(anyID client, int peerIndex);
	void UnregisterClient(anyID client);
	bool GetClientPeer(anyID client, int *result);
	bool GetPeerClient(int peerIndex, anyID *result);
}

namespace Voip
{
	namespace Server
	{
		void Shutdown()
		{
			if (!Initialized)
				return;
			ts3server_destroyServerLib();
			ValidPeerClients.reset();
			Initialized = false;
		}

		void Start()
		{
			if (Started)
				return;
			Started = true;
			CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
		}

		void Stop()
		{
			if (!Started)
				return;
			ts3server_stopVirtualServer(ServerId);
			ValidPeerClients.reset();
			Started = false;
		}

		bool IsPeerConnected(int peerIndex)
		{
			// Check if the peer has a peer index -> TS3 client ID mapping
			return GetPeerClient(peerIndex, nullptr);
		}
	}
}

namespace
{
	bool Initialize()
	{
		if (Initialized)
			return true;

		ServerLibFunctions funcs;
		memset(&funcs, 0, sizeof(funcs));

		funcs.onClientPasswordEncrypt = OnClientPasswordEncrypt;
		funcs.onCustomServerPasswordCheck = OnCustomServerPasswordCheck;
		funcs.onClientConnected = OnClientConnected;
		funcs.onClientDisconnected = OnClientDisconnected;
		funcs.onUserLoggingMessageEvent = OnUserLoggingMessageEvent;
		funcs.permClientKickFromChannel = PermClientKickFromChannel;
		funcs.permClientKickFromServer = PermClientKickFromChannel;
		funcs.permClientUpdate = PermClientUpdate;

		auto error = ts3server_initServerLib(&funcs, LogType_USERLOGGING, nullptr);
		if (error != ERROR_ok)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Voice, Utils::LogLevel::Error, "Failed to initialize the TS3 SDK - VoIP hosting will fail!");
			LogError(error);
			return false;
		}

#ifndef _DEBUG
		// Don't log debug messages on production servers
		ts3server_setLogVerbosity(LogLevel_WARNING);
#endif

		ts3server_disableClientCommand(CLIENT_COMMAND_requestClientMove);            // No switching channels
		ts3server_disableClientCommand(CLIENT_COMMAND_flushChannelCreation);         // No creating channels
		ts3server_disableClientCommand(CLIENT_COMMAND_flushChannelUpdates);          // No editing channels
		ts3server_disableClientCommand(CLIENT_COMMAND_requestChannelMove);           // No moving channels
		ts3server_disableClientCommand(CLIENT_COMMAND_requestChannelDelete);         // No deleting channels
		ts3server_disableClientCommand(CLIENT_COMMAND_requestChannelDescription);    // No channel information
		ts3server_disableClientCommand(CLIENT_COMMAND_requestChannelXXSubscribeXXX); // No channel subscriptions
		ts3server_disableClientCommand(CLIENT_COMMAND_requestSendXXXTextMsg);        // No messaging (we have our own messaging system)
		ts3server_disableClientCommand(CLIENT_COMMAND_filetransfers);                // No file transfers

		ValidPeerClients.reset();
		Initialized = true;
		return true;
	}

	DWORD WINAPI StartThread(LPVOID lpParameter)
	{
		if (!Initialized && !Initialize())
		{
			Started = false;
			return FALSE;
		}

		// NOTE: I'm not passing a keypair here, but we don't need one because we aren't using a license anyway...
		auto error = ts3server_createVirtualServer(DefaultServerPort, DefaultServerIp, DefaultServerName, "", Blam::Network::MaxPeers, &ServerId);
		if (error != ERROR_ok)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Voice, Utils::LogLevel::Error, "Failed to start the TeamSpeak 3 server!");
			LogError(error);
			Started = false;
			return FALSE;
		}
		return TRUE;
	}

	void LogError(unsigned int error)
	{
		char* errorMessage;
		if (ts3server_getGlobalErrorMessage(error, &errorMessage) != ERROR_ok)
			return;
		Utils::Logger::Instance().Log(Utils::LogTypes::Voice, Utils::LogLevel::Error, "ts3server error message: \"%s\"", errorMessage);
		ts3server_freeMemory(errorMessage);
	}

	void RegisterClient(anyID client, int peerIndex)
	{
		if (peerIndex < 0 || peerIndex >= Blam::Network::MaxPeers)
			return;
		PeerClients[peerIndex] = client;
		ValidPeerClients[peerIndex] = true;
	}

	void UnregisterClient(anyID client)
	{
		int peerIndex;
		if (GetClientPeer(client, &peerIndex))
			ValidPeerClients[peerIndex] = false;
	}

	bool GetClientPeer(anyID client, int *result)
	{
		// Just search the peer -> client ID array for the client ID
		for (size_t i = 0; i < ValidPeerClients.size(); i++)
		{
			if (ValidPeerClients[i] && PeerClients[i] == client)
			{
				if (result)
					*result = i;
				return true;
			}
		}
		return false;
	}

	bool GetPeerClient(int peerIndex, anyID *result)
	{
		if (peerIndex < 0 || peerIndex >= Blam::Network::MaxPeers)
			return false;
		if (!ValidPeerClients[peerIndex])
			return false;
		if (result)
			*result = PeerClients[peerIndex];
		return true;
	}

	void OnClientPasswordEncrypt(uint64 serverId, const char *plaintext, char *encryptedText, int encryptedTextByteSize)
	{
		if (encryptedTextByteSize < sizeof(AuthToken) * 2 + 1)
			return; // wut
		strcpy_s(encryptedText, encryptedTextByteSize, plaintext); // Just copy the auth token across without encryption
	}

	unsigned int OnCustomServerPasswordCheck(uint64 serverId, const struct ClientMiniExport *client, const char *password)
	{
		// The password is the client's authentication token stored as a hex
		// string. Parse it and validate it with the token manager.

		std::string passwordStr = password;
		if (passwordStr.length() != sizeof(AuthToken) * 2)
			return ERROR_server_invalid_password;
		AuthToken token;
		Utils::String::HexStringToBytes(passwordStr, token, sizeof(AuthToken));
		auto peerIndex = TokenManager::CheckToken(token);
		if (peerIndex < 0)
			return ERROR_server_invalid_password;

		RegisterClient(client->ID, peerIndex);
		return ERROR_ok;
	}

	void OnClientConnected(uint64 serverId, anyID clientId, uint64 channelId, unsigned int *removeClientError)
	{
		// Make sure that the client has been authenticated, just to be safe...
		if (!GetClientPeer(clientId, nullptr))
			*removeClientError = ERROR_client_not_logged_in;
	}

	void OnClientDisconnected(uint64 serverId, anyID clientId, uint64 channelId)
	{
		UnregisterClient(clientId);
	}

	void OnUserLoggingMessageEvent(const char *logMessage, int tsLogLevel, const char *logChannel, uint64 logId, const char *logTime, const char *completeLogString)
	{
		if (strcmp(logChannel, "Accounting") == 0)
			return; // Ignore license warnings
		Utils::LogLevel logLevel;
		auto logType = Utils::LogTypes::Voice;
		switch (tsLogLevel)
		{
		case LogLevel_INFO:
			logLevel = Utils::LogLevel::Info;
			break;
		case LogLevel_WARNING:
			logLevel = Utils::LogLevel::Warning;
			break;
		case LogLevel_CRITICAL:
		case LogLevel_ERROR:
			logLevel = Utils::LogLevel::Error;
			break;
		case LogLevel_DEVEL:
		case LogLevel_DEBUG:
			logLevel = Utils::LogLevel::Info;
			logType = static_cast<Utils::LogTypes>(logType | Utils::LogTypes::Debug);
			break;
		default:
			return;
		}
		std::string name = "ts3server";
		if (logChannel && logChannel[0])
			name += "::" + std::string(logChannel);
		Utils::Logger::Instance().Log(Utils::LogTypes::Voice, Utils::LogLevel::Error, "%s: %s", name.c_str(), logMessage);
	}

	unsigned int PermClientKickFromChannel(uint64 serverId, const struct ClientMiniExport *client, int toKickCount, struct ClientMiniExport *toKickClients, const char *reasonText)
	{
		// Only allow kicking if it's done from the local client
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
			return ERROR_permissions;
		auto localPeer = session->MembershipInfo.LocalPeerIndex;
		anyID localClientId;
		if (!GetPeerClient(localPeer, &localClientId))
			return ERROR_permissions;
		if (client->ID != localClientId)
			return ERROR_permissions;
		for (auto i = 0; i < toKickCount; i++)
		{
			// Don't let the host kick themself
			if (toKickClients[i].ID == localClientId)
				return ERROR_permissions;
		}
		return ERROR_ok;
	}

	unsigned int PermClientUpdate(uint64 serverId, anyID clientId, struct VariablesExport *variables)
	{
		// TODO: Disallow clients from setting their metadata?
		return ERROR_ok;
	}
}