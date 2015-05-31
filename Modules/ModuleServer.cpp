#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "ModuleServer.h"
#include <sstream>
#include "../ElDorito.h"
#include "../ThirdParty/SHA256.h"

extern BYTE passwordHash[0x20];
extern bool usingPassword;

namespace
{
	bool VariableServerPasswordUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::string password = Modules::ModuleServer::Instance().VarServerPassword->ValueString;

		usingPassword = password.length() > 0;

		if (usingPassword)
		{
			SHA256_CTX ctx;
			sha256_init(&ctx);
			sha256_update(&ctx, (unsigned char*)password.c_str(), password.length());
			sha256_final(&ctx, passwordHash);
		}
		else
		{
			returnInfo = "Server password unset.";
			return true;
		}

		return true;
	}

	bool VariableServerCountdownUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		unsigned long seconds = Modules::ModuleServer::Instance().VarServerCountdown->ValueInt;

		Pointer::Base(0x153708).Write<uint8_t>((uint8_t)seconds + 0); // player control
		Pointer::Base(0x153738).Write<uint8_t>((uint8_t)seconds + 4); // camera position
		Pointer::Base(0x1521D1).Write<uint8_t>((uint8_t)seconds + 4); // ui timer

		// Fix team notification
		if (seconds == 4)
			Pointer::Base(0x1536F0).Write<uint8_t>(2);
		else
			Pointer::Base(0x1536F0).Write<uint8_t>(3);

		return true;
	}

	bool VariableServerMaxPlayersUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		typedef char(__cdecl *NetworkSquadSessionSetMaximumPlayerCountFunc)(int count);
		auto network_squad_session_set_maximum_player_count = (NetworkSquadSessionSetMaximumPlayerCountFunc)0x439BA0;
		char ret = network_squad_session_set_maximum_player_count(Modules::ModuleServer::Instance().VarServerMaxPlayers->ValueInt);
		if (ret == 0)
		{
			returnInfo = "Failed to update max player count, are you hosting a lobby?";
			return false;
		}

		return true;
	}

	bool CommandServerConnect(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() <= 0)
		{
			returnInfo = "Invalid arguments.";
			return false;
		}

		std::string address = Arguments[0];
		std::string password = "";

		if (Arguments.size() >= 2)
			password = Arguments[1];

		uint32_t rawIpaddr = 0;
		uint16_t rawPort = 11774;
		BYTE passwordHash[0x20];
		memset(passwordHash, 0x11, 0x20);

		size_t portOffset = address.find(':');
		auto host = address;
		if (portOffset != std::string::npos && portOffset + 1 < address.size())
		{
			auto port = address.substr(portOffset + 1);
			rawPort = (uint16_t)std::stoi(port);
			host = address.substr(0, portOffset);
		}

		struct addrinfo* info = NULL;
		INT retval = getaddrinfo(host.c_str(), NULL, NULL, &info);
		if (retval != 0)
		{
			int error = WSAGetLastError();
			std::string ret = "Unable to lookup " + address + " (" + std::to_string(retval) + "): ";
			if (error != 0)
			{
				if (error == WSAHOST_NOT_FOUND)
					ret += "Host not found.";
				else if (error == WSANO_DATA)
					ret += "No data record found.";
				else
					ret += "Function failed with error " + std::to_string(error) + ".";
			}
			else
				ret += "Unknown error.";
			returnInfo = ret;
			return false;
		}

		struct addrinfo *ptr = NULL;
		for (ptr = info; ptr != NULL; ptr = ptr->ai_next)
		{
			if (ptr->ai_family != AF_INET)
				continue; // not ipv4

			rawIpaddr = ntohl(((sockaddr_in*)ptr->ai_addr)->sin_addr.S_un.S_addr);
			break;
		}

		if (!rawIpaddr)
		{
			returnInfo = "Unable to lookup " + address + ": No records found.";;
			return false;
		}

		if (password.length() > 0)
		{
			SHA256_CTX ctx;
			sha256_init(&ctx);
			sha256_update(&ctx, (unsigned char*)password.c_str(), password.length());
			sha256_final(&ctx, passwordHash);
		}

		// set up our syslink data struct
		memset(Modules::ModuleServer::Instance().SyslinkData, 0, 0x176);
		*(uint32_t*)Modules::ModuleServer::Instance().SyslinkData = 1;

		memcpy(Modules::ModuleServer::Instance().SyslinkData + 0x9E, passwordHash, 0x20);

		*(uint32_t*)(Modules::ModuleServer::Instance().SyslinkData + 0x170) = rawIpaddr;
		*(uint16_t*)(Modules::ModuleServer::Instance().SyslinkData + 0x174) = rawPort;

		// set syslink stuff to point at our syslink data
		Pointer::Base(0x1E8E6D8).Write<uint32_t>(1);
		Pointer::Base(0x1E8E6DC).Write<uint32_t>((uint32_t)&Modules::ModuleServer::Instance().SyslinkData);

		// tell the game to start joining
		Pointer::Base(0x1E40BA8).Write<int64_t>(-1);
		Pointer::Base(0x1E40BB0).Write<uint32_t>(1);
		Pointer::Base(0x1E40BB4).Write(passwordHash, 0x10);
		Pointer::Base(0x1E40BD4).Write(passwordHash + 0x10, 0x10);
		Pointer::Base(0x1E40BE4).Write<uint32_t>(1);

		returnInfo = "Attempting connection to " + address + "...";
		return true;
	}
}

namespace Modules
{
	ModuleServer::ModuleServer() : ModuleBase("Server")
	{
		VarServerPassword = AddVariableString("Password", "password", "The server password, must be set before starting a game", "", VariableServerPasswordUpdate);

		VarServerCountdown = AddVariableInt("Countdown", "countdown", "The number of seconds to wait at the start of the game", 20, VariableServerCountdownUpdate);
		VarServerCountdown->ValueIntMin = 0;
		VarServerCountdown->ValueIntMax = 20;

		VarServerMaxPlayers = AddVariableInt("MaxPlayers", "maxplayers", "Maximum number of connected players", 16, VariableServerMaxPlayersUpdate);
		VarServerMaxPlayers->ValueIntMin = 1;
		VarServerMaxPlayers->ValueIntMax = 16;

		VarServerLanMode = AddVariableInt("LanMode", "lanmode", "Enables clients to detect this game over a network", 0);
		VarServerLanMode->ValueIntMin = 0;
		VarServerLanMode->ValueIntMax = 1;

		AddCommand("Connect", "connect", "Begins establishing a connection to a server", CommandServerConnect, { "host:port The server info to connect to", "password(string) The password for the server, if any is set" });
	}
}