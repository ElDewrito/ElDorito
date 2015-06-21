#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "ModuleServer.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include "../ElDorito.h"
#include "../ThirdParty/SHA256.h"
#include "../ThirdParty/HttpRequest.h"
#include "../ThirdParty/rapidjson/document.h"
#include "../Patches/Network.h"

namespace
{
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

	// retrieves master server announce endpoints from dewrito.json
	void GetAnnounceEndpoints(std::vector<std::string>& destVect)
	{
		std::string prefix("http://");

		std::ifstream in("dewrito.json", std::ios::in | std::ios::binary);
		if (in && in.is_open())
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize((unsigned int)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			rapidjson::Document json;
			if (!json.Parse<0>(contents.c_str()).HasParseError() && json.IsObject())
			{
				if (json.HasMember("masterServers"))
				{
					auto& mastersArray = json["masterServers"];
					for (auto it = mastersArray.Begin(); it != mastersArray.End(); it++)
					{
						destVect.push_back((*it)["announce"].GetString());
					}
				}
			}
		}
	}

	DWORD WINAPI CommandServerAnnounce_Thread(LPVOID lpParam)
	{
		//std::stringstream ss;
		std::vector<std::string> announceEndpoints;

		GetAnnounceEndpoints(announceEndpoints);

		for (auto server : announceEndpoints)
		{
			HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");
			//ss << "Announcing to " << server << "..." << std::endl;

			if (!req.SendRequest(Utils::String::WidenString(server + "?port=" + Modules::ModuleServer::Instance().VarServerPort->ValueString), L"GET", L"", L"", NULL, 0))
			{
				//ss << "Unable to connect to master server. (error: " << req.lastError << "/" << std::to_string(GetLastError()) << ")" << std::endl << std::endl;
				continue;
			}

			/* TODO: return below to the user somehow, maybe using a log file
			// make sure the server replied with 200 OK
			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
				ss << "Invalid master server announce response." << std::endl << std::endl;
				continue;
			}

			auto respHdr = req.responseHeader.substr(0, expected.length());
			if (respHdr.compare(expected))
			{
				ss << "Invalid master server announce response." << std::endl << std::endl;
				continue;
			}

			// parse the json response
			std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
			rapidjson::Document json;
			if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
			{
				ss << "Invalid master server JSON response." << std::endl << std::endl;
				continue;
			}

			if (!json.HasMember("result"))
			{
				ss << "Master server JSON response is missing data." << std::endl << std::endl;
				continue;
			}

			auto& result = json["result"];
			if (result["code"].GetInt() != 0)
			{
				ss << "Master server returned error code " << result["code"].GetInt() << " (" << result["msg"].GetString() << ")" << std::endl << std::endl;
				continue;
			}

			ss << "Announce OK" << std::endl << std::endl;*/
		}

		return true;
	}

	DWORD WINAPI CommandServerUnannounce_Thread(LPVOID lpParam)
	{
		//std::stringstream ss;
		std::vector<std::string> announceEndpoints;

		GetAnnounceEndpoints(announceEndpoints);

		for (auto server : announceEndpoints)
		{
			HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");
			//ss << "Unannouncing to " << server << "..." << std::endl;

			if (!req.SendRequest(Utils::String::WidenString(server + "?port=" + Modules::ModuleServer::Instance().VarServerPort->ValueString + "&shutdown=true"), L"GET", L"", L"", NULL, 0))
			{
				//ss << "Unable to connect to master server. (error: " << req.lastError << "/" << std::to_string(GetLastError()) << ")" << std::endl << std::endl;
				continue;
			}

			/* TODO: return below to the user somehow, maybe using a log file
			// make sure the server replied with 200 OK
			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
			ss << "Invalid master server unannounce response." << std::endl << std::endl;
			continue;
			}

			auto respHdr = req.responseHeader.substr(0, expected.length());
			if (respHdr.compare(expected))
			{
			ss << "Invalid master server unannounce response." << std::endl << std::endl;
			continue;
			}

			// parse the json response
			std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
			rapidjson::Document json;
			if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
			{
			ss << "Invalid master server JSON response." << std::endl << std::endl;
			continue;
			}

			if (!json.HasMember("result"))
			{
			ss << "Master server JSON response is missing data." << std::endl << std::endl;
			continue;
			}

			auto& result = json["result"];
			if (result["code"].GetInt() != 0)
			{
			ss << "Master server returned error code " << result["code"].GetInt() << " (" << result["msg"].GetString() << ")" << std::endl << std::endl;
			continue;
			}

			ss << "Unannounce OK" << std::endl << std::endl;*/
		}

		return true;
	}

	bool CommandServerAnnounce(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!Patches::Network::IsInfoSocketOpen())
			return false;

		auto thread = CreateThread(NULL, 0, CommandServerAnnounce_Thread, (LPVOID)&Arguments, 0, NULL);
		returnInfo = "Announcing to master servers...";
		return true;
	}

	bool CommandServerUnannounce(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!Patches::Network::IsInfoSocketOpen())
			return false;

		auto thread = CreateThread(NULL, 0, CommandServerUnannounce_Thread, (LPVOID)&Arguments, 0, NULL);
		returnInfo = "Unannouncing to master servers...";
		return true;
	}

	bool VariableServerShouldAnnounceUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!Modules::ModuleServer::Instance().VarServerShouldAnnounce->ValueInt) // if we're setting Server.ShouldAnnounce to false unannounce ourselves too
			CommandServerUnannounce(Arguments, std::string());

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
		if (Arguments.size() > 1)
			password = Arguments[1];

		uint32_t rawIpaddr = 0;
		int httpPort = 11775;

		size_t portOffset = address.find(':');
		auto host = address;
		if (portOffset != std::string::npos && portOffset + 1 < address.size())
		{
			auto port = address.substr(portOffset + 1);
			httpPort = (uint16_t)std::stoi(port);
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

		// query the server
		HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

		std::wstring usernameStr = L"";
		std::wstring passwordStr = L"";
		if (!password.empty())
		{
			usernameStr = L"dorito";
			passwordStr = Utils::String::WidenString(password);
		}

		if (!req.SendRequest(Utils::String::WidenString("http://" + host + ":" + std::to_string(httpPort) + "/"), L"GET", usernameStr, passwordStr, NULL, 0))
		{
			returnInfo = "Unable to connect to server. (error: " + std::to_string(req.lastError) + "/" + std::to_string(GetLastError()) + ")";
			return false;
		}

		// make sure the server replied with 200 OK
		std::wstring expected = L"HTTP/1.1 200 OK";
		if (req.responseHeader.length() < expected.length())
		{
			returnInfo = "Invalid server query response.";
			return false;
		}

		auto respHdr = req.responseHeader.substr(0, expected.length());
		if (respHdr.compare(expected))
		{
			returnInfo = "Invalid server query header response.";
			return false;
		}

		// parse the json response
		std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
		rapidjson::Document json;
		if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
		{
			returnInfo = "Invalid server query JSON response.";
			return false;
		}

		// make sure the json has all the members we need
		if (!json.HasMember("gameVersion") || !json["gameVersion"].IsString() ||
			!json.HasMember("eldewritoVersion") || !json["eldewritoVersion"].IsString() ||
			!json.HasMember("port") || !json["port"].IsNumber())
		{
			returnInfo = "Server query JSON response is missing data.";
			return false;
		}

		if (!json.HasMember("xnkid") || !json["xnkid"].IsString() ||
			!json.HasMember("xnaddr") || !json["xnaddr"].IsString())
		{
			returnInfo = "Incorrect password specified.";
			return false;
		}

		std::string gameVer = json["gameVersion"].GetString();
		std::string edVer = json["eldewritoVersion"].GetString();

		std::string ourGameVer((char*)Pointer(0x199C0F0));
		std::string ourEdVer = Utils::Version::GetVersionString();
		if (ourGameVer.compare(gameVer))
		{
			returnInfo = "Server is running a different game version.";
			return false;
		}

		if (ourEdVer.compare(edVer))
		{
			returnInfo = "Server is running a different ElDewrito version.";
			return false;
		}

		std::string xnkid = json["xnkid"].GetString();
		std::string xnaddr = json["xnaddr"].GetString();
		if (xnkid.length() != (0x10 * 2) || xnaddr.length() != (0x10 * 2))
		{
			returnInfo = "Server query XNet info is invalid.";
			return false;
		}
		uint16_t gamePort = (uint16_t)json["port"].GetInt();

		BYTE xnetInfo[0x20];
		Utils::String::HexStringToBytes(xnkid, xnetInfo, 0x10);
		Utils::String::HexStringToBytes(xnaddr, xnetInfo + 0x10, 0x10);

		// set up our syslink data struct
		memset(Modules::ModuleServer::Instance().SyslinkData, 0, 0x176);
		*(uint32_t*)Modules::ModuleServer::Instance().SyslinkData = 1;

		memcpy(Modules::ModuleServer::Instance().SyslinkData + 0x9E, xnetInfo, 0x20);

		*(uint32_t*)(Modules::ModuleServer::Instance().SyslinkData + 0x170) = rawIpaddr;
		*(uint16_t*)(Modules::ModuleServer::Instance().SyslinkData + 0x174) = gamePort;

		// set syslink stuff to point at our syslink data
		Pointer::Base(0x1E8E6D8).Write<uint32_t>(1);
		Pointer::Base(0x1E8E6DC).Write<uint32_t>((uint32_t)&Modules::ModuleServer::Instance().SyslinkData);

		// tell the game to start joining
		Pointer::Base(0x1E40BA8).Write<int64_t>(-1);
		Pointer::Base(0x1E40BB0).Write<uint32_t>(1);
		Pointer::Base(0x1E40BB4).Write(xnetInfo, 0x10);
		Pointer::Base(0x1E40BD4).Write(xnetInfo + 0x10, 0x10);
		Pointer::Base(0x1E40BE4).Write<uint32_t>(1);

		returnInfo = "Attempting connection to " + address + "...";
		return true;
	}
}

namespace Modules
{
	ModuleServer::ModuleServer() : ModuleBase("Server")
	{
		VarServerName = AddVariableString("Name", "server_name", "The name of the server", eCommandFlagsArchived, "HaloOnline Server");

		VarServerPassword = AddVariableString("Password", "password", "The server password", eCommandFlagsArchived, "");

		VarServerCountdown = AddVariableInt("Countdown", "countdown", "The number of seconds to wait at the start of the game", eCommandFlagsArchived, 5, VariableServerCountdownUpdate);
		VarServerCountdown->ValueIntMin = 0;
		VarServerCountdown->ValueIntMax = 20;

		VarServerMaxPlayers = AddVariableInt("MaxPlayers", "maxplayers", "Maximum number of connected players", eCommandFlagsArchived, 16, VariableServerMaxPlayersUpdate);
		VarServerMaxPlayers->ValueIntMin = 1;
		VarServerMaxPlayers->ValueIntMax = 16;

		VarServerPort = AddVariableInt("Port", "server_port", "The port number the HTTP server runs on, game uses different one", eCommandFlagsArchived, 11775);
		VarServerPort->ValueIntMin = 1;
		VarServerPort->ValueIntMax = 0xFFFF;

		VarServerShouldAnnounce = AddVariableInt("ShouldAnnounce", "should_announce", "Controls whether the server will be announced to the master servers", eCommandFlagsArchived, 1, VariableServerShouldAnnounceUpdate);
		VarServerShouldAnnounce->ValueIntMin = 0;
		VarServerShouldAnnounce->ValueIntMax = 1;

		AddCommand("Connect", "connect", "Begins establishing a connection to a server", eCommandFlagsNone, CommandServerConnect, { "host:port The server info to connect to", "password(string) The password for the server" });
		AddCommand("Announce", "announce", "Announces this server to the master servers", eCommandFlagsNone, CommandServerAnnounce);
		AddCommand("Unannounce", "unannounce", "Notifies the master servers to remove this server", eCommandFlagsNone, CommandServerUnannounce);
	}
}