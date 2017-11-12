#include "ModuleDiscord.hpp"
#include "../ElDorito.hpp"
#include "discord-rpc.h"
#include "../Utils/Logger.hpp"
#include "../Utils/String.hpp"
#include "ModuleServer.hpp"
#include "../Patches/Core.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Patches/Ui.hpp"
#include "../Patches/Network.hpp"

#include <ctime>

static const char* APPLICATION_ID = "378684431830876170";

namespace
{
	std::time_t roundStartTime; //Unimplemented so far
	std::time_t roundEndTime;

	int isOnline = 0;

	void DiscordReady()
	{
#ifdef _DEBUG
		printf("Discord Rich Presence is ready!");
#endif 
	}

	void handleDiscordError(int errorCode, const char* message)
	{
		std::string output = "-Discord Error-\nCode: " + std::to_string(errorCode) + "\nMessage: " + message;
		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, output);
	}

	void handleDiscordDisconnected(int errorCode, const char* message)
	{
		std::string output = "-Discord Error-\nCode: " + std::to_string(errorCode) + "\nMessage: " + message;
		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Warning, output);
	}

	/* This handles joining games. handleDiscordJoinRequest sends discord a secret,
	   which is then sent to the other game and is handled by this message. */
	void handleDiscordJoinGame(const char* joinSecret)
	{
		/* Not yet implemented. You need to be approved to use this feature, but it
		   is feasable. Probably can just send the IP in clear unless auth is
		   implemented someday. */
	}

	void handleDiscordSpectateGame(const char* spectateSecret)
	{
		//Unimplemented, won't ever be implemented.
	}

	/* This handles recieving requests from discord. It replies to discord with a
		secret, which is then accepted by the other game's handleDiscordJoinGame. */
	void handleDiscordJoinRequest(const DiscordJoinRequest* request) {
		//Unimplemented yet
	}

	void InitDiscord()
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = DiscordReady;
		handlers.errored = handleDiscordError;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.joinGame = handleDiscordJoinGame;
		handlers.spectateGame = handleDiscordSpectateGame;
		handlers.joinRequest = handleDiscordJoinRequest;
		Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
	}

	void UpdatePresence()
	{
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));

		Blam::Players::PLAYER_STATS localPlayerStats = Blam::Players::GetStats(Blam::Players::GetLocalPlayer(0));

		//2 = Multiplayer, 3 = Forge
		int lobbyType = Blam::Network::GetLobbyType();

		std::string GameVariantName(Utils::String::ThinString((wchar_t*)Pointer(0x23DAF4C)));
		std::string BaseMapName((char*)Pointer(0x22AB018)(0x1A4));
		std::string VariantMapName(Utils::String::ThinString((wchar_t*)Pointer(0x1863ACA)).c_str());
		std::string ServerNameClient = Modules::ModuleServer::Instance().VarServerNameClient->ValueString;
		std::string LobbyTypeString;
		std::string isOnlineString;

		switch (isOnline) {
		case 0:
			isOnlineString = "Online";
			break;
		case 1:
			isOnlineString = "Offline";
			break;
		default:
			isOnlineString = "Undefined";
			break;
		}

		switch (lobbyType) {
		case 2:
			LobbyTypeString = "Multiplayer";
			break;
		case 3:
			LobbyTypeString = "Forge";
			break;
		default:
			LobbyTypeString = "Undefined";
			break;
		}

		discordPresence.largeImageKey = "default";
		std::string LobbyStateString = ("In an " + isOnlineString + " " + LobbyTypeString + " Lobby");
		std::string LobbyConnectedServerString = ("Connected to " + ServerNameClient);
		std::string IngameStateString = (LobbyTypeString + (isOnline == 1 ? (" on " + ServerNameClient) : " Offline"));
		std::string IngameDetailString = ("Playing " + GameVariantName + " on " + (VariantMapName.empty() ? BaseMapName : VariantMapName));

		if (BaseMapName == "mainmenu")
		{
			if (!(lobbyType == 2 || lobbyType == 3))
				discordPresence.state = "At The Main Menu";
			else
			{
				discordPresence.state = LobbyStateString.c_str();
				if (isOnline == 2)
					discordPresence.details = LobbyConnectedServerString.c_str();
			}
		}
		else
		{
			discordPresence.state = IngameStateString.c_str();
			discordPresence.details = IngameDetailString.c_str();
			discordPresence.largeImageKey = BaseMapName.c_str();
		}

		Discord_UpdatePresence(&discordPresence);
	}

	void UpdatePresenceStart() {
		UpdatePresence();
	}

	void UpdatePresenceMap(const char* mapPath) {
		UpdatePresence();
	}

	void UpdatePresenceLobbyType(int _lobbyType) {
		UpdatePresence();
	}

	void UpdatePresenceLobbyisOnline(int _isOnline) {
		isOnline = _isOnline;
		UpdatePresence();
	}

	void DiscordShutdown() {
		Discord_Shutdown();
	}

	bool CommandTestPresence(const std::vector<std::string>& Arguments, std::string& returnInfo) {
		UpdatePresence();
		return true;
	}
}

namespace Modules
{
	ModuleDiscord::ModuleDiscord() : ModuleBase("Discord")
	{
		TestPresence = AddCommand("TestDiscordRichPresence", "testpres", "Tests the Rich Presence Integration with Discord.", eCommandFlagsNone, CommandTestPresence);
		Patches::Core::OnGameStart(UpdatePresenceStart);
		Patches::Core::OnShutdown(DiscordShutdown);
		Patches::Core::OnMapLoaded(UpdatePresenceMap);
		Patches::Ui::OnGameModeChanged(UpdatePresenceLobbyType);
		Patches::Network::OnNetworkModeChanged(UpdatePresenceLobbyisOnline);
	}
	void ModuleDiscord::DiscordInit()
	{
		::InitDiscord();
	}
	void ModuleDiscord::PresenceUpdate()
	{
		::UpdatePresence();
	}
}