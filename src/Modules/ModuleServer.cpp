#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "ModuleServer.hpp"
#include <sstream>
#include <fstream>
#include "../ElDorito.hpp"
#include "../Patches/Network.hpp"
#include "../Patches/PlayerUid.hpp"
#include "../Utils/DebugLog.hpp"

#include "../ThirdParty/HttpRequest.hpp"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../VoIP/TeamspeakClient.hpp"
#include "../Utils/Cryptography.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Console/GameConsole.hpp"
#include "../Server/VariableSynchronization.hpp"
#include "../Patches/Sprint.hpp"
#include "../Server/BanList.hpp"

namespace
{
	std::string IpToString(const Blam::Network::NetworkAddress &addr)
	{
		struct in_addr inAddr;
		inAddr.S_un.S_addr = addr.ToInAddr();
		char ipStr[INET_ADDRSTRLEN];
		if (!inet_ntop(AF_INET, &inAddr, ipStr, sizeof(ipStr)))
			return "";
		return std::string(ipStr);
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
		network_squad_session_set_maximum_player_count(Modules::ModuleServer::Instance().VarServerMaxPlayers->ValueInt);
		return true;
	}

	// retrieves master server endpoints from dewrito.json
	void GetEndpoints(std::vector<std::string>& destVect, std::string endpointType)
	{
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
						destVect.push_back((*it)[endpointType.c_str()].GetString());
					}
				}
			}
		}
	}

	DWORD WINAPI CommandServerAnnounce_Thread(LPVOID lpParam)
	{
		std::stringstream ss;
		std::vector<std::string> announceEndpoints;

		GetEndpoints(announceEndpoints, "announce");

		for (auto server : announceEndpoints)
		{
			HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

			try
			{
				if (!req.SendRequest(Utils::String::WidenString(server + "?port=" + Modules::ModuleServer::Instance().VarServerPort->ValueString), L"GET", L"", L"", L"", NULL, 0))
				{
					ss << "Unable to connect to master server " << server << " (error: " << req.lastError << "/" << std::to_string(GetLastError()) << ")" << std::endl << std::endl;
					continue;
				}
			}
			catch(...) // TODO: find out what exception is being caused
			{
				ss << "Exception during master server announce request to " << server << std::endl << std::endl;
				continue;
			}

			// make sure the server replied with 200 OK
			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
				ss << "Invalid master server announce response from " << server << std::endl << std::endl;
				continue;
			}

			auto respHdr = req.responseHeader.substr(0, expected.length());
			if (respHdr.compare(expected))
			{
				ss << "Invalid master server announce response from " << server << std::endl << std::endl;
				continue;
			}

			// parse the json response
			std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
			rapidjson::Document json;
			if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
			{
				ss << "Invalid master server JSON response from " << server << std::endl << std::endl;
				continue;
			}

			if (!json.HasMember("result"))
			{
				ss << "Master server JSON response from " << server << " is missing data." << std::endl << std::endl;
				continue;
			}

			auto& result = json["result"];
			if (result["code"].GetInt() != 0)
			{
				ss << "Master server " << server << " returned error code " << result["code"].GetInt() << " (" << result["msg"].GetString() << ")" << std::endl << std::endl;
				continue;
			}
		}

		std::string errors = ss.str();
		if (errors.length() > 0)
			Utils::DebugLog::Instance().Log("Announce", ss.str());

		return true;
	}

	DWORD WINAPI CommandServerUnannounce_Thread(LPVOID lpParam)
	{
		std::stringstream ss;
		std::vector<std::string> announceEndpoints;

		GetEndpoints(announceEndpoints, "announce");

		for (auto server : announceEndpoints)
		{
			HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

			try
			{
				if (!req.SendRequest(Utils::String::WidenString(server + "?port=" + Modules::ModuleServer::Instance().VarServerPort->ValueString + "&shutdown=true"), L"GET", L"", L"", L"", NULL, 0))
				{
					ss << "Unable to connect to master server " << server << " (error: " << req.lastError << "/" << std::to_string(GetLastError()) << ")" << std::endl << std::endl;
					continue;
				}
			}
			catch (...)
			{
				ss << "Exception during master server unannounce request to " << server << std::endl << std::endl;
				continue;
			}

			// make sure the server replied with 200 OK
			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
				ss << "Invalid master server unannounce response from " << server << std::endl << std::endl;
				continue;
			}

			auto respHdr = req.responseHeader.substr(0, expected.length());
			if (respHdr.compare(expected))
			{
				ss << "Invalid master server unannounce response from " << server << std::endl << std::endl;
				continue;
			}

			// parse the json response
			std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
			rapidjson::Document json;
			if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
			{
				ss << "Invalid master server JSON response from " << server << std::endl << std::endl;
				continue;
			}

			if (!json.HasMember("result"))
			{
				ss << "Master server JSON response from " << server << " is missing data." << std::endl << std::endl;
				continue;
			}

			auto& result = json["result"];
			if (result["code"].GetInt() != 0)
			{
				ss << "Master server " << server << " returned error code " << result["code"].GetInt() << " (" << result["msg"].GetString() << ")" << std::endl << std::endl;
				continue;
			}
		}

		std::string errors = ss.str();
		if (errors.length() > 0)
			Utils::DebugLog::Instance().Log("Unannounce", ss.str());

		return true;
	}

	DWORD WINAPI CommandServerAnnounceStats_Thread(LPVOID lpParam)
	{
		std::stringstream ss;
		std::vector<std::string> statsEndpoints;
		auto& debugLog = Utils::DebugLog::Instance();

		GetEndpoints(statsEndpoints, "stats");

		//typedef int(__cdecl *Game_GetLocalPlayerDatumIdxFunc)(int localPlayerIdx);
		//Game_GetLocalPlayerDatumIdxFunc Game_GetLocalPlayerDatumIdx = reinterpret_cast<Game_GetLocalPlayerDatumIdxFunc>(0x589C30);
		//uint16_t playerIdx = (uint16_t)(Game_GetLocalPlayerDatumIdx(0) & 0xFFFF);
		// above wont work since we're on a different thread without the proper TLS data :(

		auto& localPlayers = ElDorito::GetMainTls(GameGlobals::LocalPlayers::TLSOffset)[0];
		uint16_t playerIdx = (uint16_t)(localPlayers(GameGlobals::LocalPlayers::Player0DatumIdx).Read<uint32_t>() & 0xFFFF);

		auto& playersGlobal = ElDorito::GetMainTls(GameGlobals::Players::TLSOffset)[0];
		int32_t team = playersGlobal(0x54 + GameGlobals::Players::TeamOffset + (playerIdx * GameGlobals::Players::PlayerEntryLength)).Read<int32_t>();

		int16_t score = playersGlobal(0x54 + GameGlobals::Players::ScoreBase + (playerIdx * GameGlobals::Players::ScoresEntryLength)).Read<int16_t>();
		int16_t kills = playersGlobal(0x54 + GameGlobals::Players::KillsBase + (playerIdx * GameGlobals::Players::ScoresEntryLength)).Read<int16_t>();
		int16_t deaths = playersGlobal(0x54 + GameGlobals::Players::DeathsBase + (playerIdx * GameGlobals::Players::ScoresEntryLength)).Read<int16_t>();
		// unsure about assists
		int16_t assists = playersGlobal(0x54 + GameGlobals::Players::AssistsBase + (playerIdx * GameGlobals::Players::ScoresEntryLength)).Read<int16_t>();

		// TODO: get an ID for this match
		int32_t gameId = 0x1337BEEF;

		// build our stats announcement
		rapidjson::StringBuffer statsBuff;
		rapidjson::Writer<rapidjson::StringBuffer> statsWriter(statsBuff);
		statsWriter.StartObject();
		statsWriter.Key("gameId");
		statsWriter.Int(gameId);
		statsWriter.Key("score");
		statsWriter.Int(score);
		statsWriter.Key("kills");
		statsWriter.Int(kills);
		statsWriter.Key("assists");
		statsWriter.Int(assists);
		statsWriter.Key("deaths");
		statsWriter.Int(deaths);
		statsWriter.Key("team");
		statsWriter.Int(team);
		statsWriter.Key("medals");
		statsWriter.StartArray();

		// TODO: log each medal earned during the game and output them here
		/*statsWriter.String("doublekill");
		statsWriter.String("triplekill");
		statsWriter.String("overkill");
		statsWriter.String("unfreakingbelieveable");*/

		statsWriter.EndArray();
		statsWriter.EndObject();

		std::string statsObject = statsBuff.GetString();
		// todo: look into using JSON Web Tokens (JWT) that use JSON Web Signature (JWS), instead of using our own signature stuff
		std::string statsSignature;
		if (!Utils::Cryptography::CreateRSASignature(Patches::PlayerUid::GetFormattedPrivKey(), (void*)statsObject.c_str(), statsObject.length(), statsSignature))
		{
			ss << "Failed to create stats RSA signature!";
			debugLog.Log("AnnounceStats", ss.str());
			return 0;
		}

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("statsVersion");
		writer.Int(1);
		writer.Key("stats");
		writer.String(statsObject.c_str()); // write stats object as a string instead of object so that the string matches up exactly with what we signed (also because there's no easy way to append a writer..)
		writer.Key("publicKey");
		writer.String(Modules::ModulePlayer::Instance().VarPlayerPubKey->ValueString.c_str());
		writer.Key("signature");
		writer.String(statsSignature.c_str());
		writer.EndObject();

		std::string sendObject = s.GetString();

		for (auto server : statsEndpoints)
		{
			HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

			try
			{
				if (!req.SendRequest(Utils::String::WidenString(server), L"POST", L"", L"", L"Content-Type: application/json\r\n", (void*)sendObject.c_str(), sendObject.length()))
				{
					ss << "Unable to connect to master server " << server << " (error: " << req.lastError << "/" << std::to_string(GetLastError()) << ")" << std::endl << std::endl;
					continue;
				}
			}
			catch (...)
			{
				ss << "Exception during master server stats announce request to " << server << std::endl << std::endl;
				continue;
			}

			// make sure the server replied with 200 OK
			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
				ss << "Invalid master server stats response from " << server << std::endl << std::endl;
				continue;
			}

			auto respHdr = req.responseHeader.substr(0, expected.length());
			if (respHdr.compare(expected))
			{
				ss << "Invalid master server stats response from " << server << std::endl << std::endl;
				continue;
			}

			// parse the json response
			std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
			rapidjson::Document json;
			if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
			{
				ss << "Invalid master server JSON response from " << server << std::endl << std::endl;
				continue;
			}

			if (!json.HasMember("result"))
			{
				ss << "Master server JSON response from " << server << " is missing data." << std::endl << std::endl;
				continue;
			}

			auto& result = json["result"];
			if (result["code"].GetInt() != 0)
			{
				ss << "Master server " << server << " returned error code " << result["code"].GetInt() << " (" << result["msg"].GetString() << ")" << std::endl << std::endl;
				continue;
			}
		}

		std::string errors = ss.str();
		if (errors.length() > 0)
			debugLog.Log("AnnounceStats", ss.str());

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

	bool CommandServerAnnounceStats(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		//if (!IsEndGame())
		//	return false;

		auto thread = CreateThread(NULL, 0, CommandServerAnnounceStats_Thread, (LPVOID)&Arguments, 0, NULL);
		returnInfo = "Announcing stats to master servers...";
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
		// TODO: move this into a thread so that non-responding hosts don't lag the game
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

		freeaddrinfo(info);

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

		if (!req.SendRequest(Utils::String::WidenString("http://" + host + ":" + std::to_string(httpPort) + "/"), L"GET", usernameStr, passwordStr, L"", NULL, 0))
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

		// start voip
		if (Modules::ModuleVoIP::Instance().VarVoIPEnabled->ValueInt == 1) 
		{
			//Make sure teamspeak is stopped before we try to start it.
			StopTeamspeakClient();
			CreateThread(0, 0, StartTeamspeakClient, 0, 0, 0);
		}

		returnInfo = "Attempting connection to " + address + "...";
		return true;
	}

	int FindPlayerByName(const std::string &name)
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
			return -1;
		auto membership = &session->MembershipInfo;
		for (auto peerIdx = membership->FindFirstPeer(); peerIdx >= 0; peerIdx = membership->FindNextPeer(peerIdx))
		{
			auto playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
			if (playerIdx == -1)
				continue;
			auto* player = &membership->PlayerSessions[playerIdx];
			if (Utils::String::ThinString(player->DisplayName) == name)
				return playerIdx;
		}
		return -1;
	}

	std::vector<int> FindPlayersByUid(uint64_t uid)
	{
		std::vector<int> indices;
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
			return indices;
		auto membership = &session->MembershipInfo;
		for (auto peerIdx = membership->FindFirstPeer(); peerIdx >= 0; peerIdx = membership->FindNextPeer(peerIdx))
		{
			auto playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
			if (playerIdx == -1)
				continue;
			auto* player = &membership->PlayerSessions[playerIdx];
			if (player->Uid == uid)
				indices.push_back(playerIdx);
		}
		return indices;
	}

	void BanIp(const std::string &ip)
	{
		auto banList = Server::LoadDefaultBanList();
		banList.AddIp(ip);
		Server::SaveDefaultBanList(banList);
	}

	bool UnbanIp(const std::string &ip)
	{
		auto banList = Server::LoadDefaultBanList();
		if (!banList.RemoveIp(ip))
			return false;
		Server::SaveDefaultBanList(banList);
		return true;
	}

	enum class KickType
	{
		Kick,
		Ban
	};

	bool DoKickPlayerCommand(KickType type, const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() <= 0)
		{
			returnInfo = "Invalid arguments";
			return false;
		}
		auto kickPlayerName = Utils::String::Join(Arguments);
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
		{
			returnInfo = "You must be hosting a game to use this command";
			return false;
		}
		auto playerIdx = FindPlayerByName(kickPlayerName);
		if (playerIdx < 0)
		{
			returnInfo = "Player \"" + kickPlayerName + "\" not found.";
			return false;
		}
		auto ip = IpToString(session->GetPeerAddress(session->MembershipInfo.GetPlayerPeer(playerIdx)));
		if (!Blam::Network::BootPlayer(playerIdx, 4))
		{
			returnInfo = "Failed to kick player " + kickPlayerName;
			return false;
		}
		if (type == KickType::Ban)
		{
			BanIp(ip);
			returnInfo = "Added IP " + ip + " to the ban list\n";
		}
		returnInfo += "Issued kick request for player " + kickPlayerName + " (" + std::to_string(playerIdx) + ")";
		return true;
	}

	bool DoKickUidCommand(KickType type, const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 1)
		{
			returnInfo = "Invalid arguments";
			return false;
		}
		uint64_t uid;
		if (!Patches::PlayerUid::ParseUid(Arguments[0], &uid))
		{
			returnInfo = "Invalid UID";
			return false;
		}
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
		{
			returnInfo = "You must be hosting a game to use this command";
			return false;
		}
		auto indices = FindPlayersByUid(uid);
		if (indices.size() == 0)
		{
			returnInfo = "Player with UID " + Arguments[0] + " not found.";
			return false;
		}
		auto success = false;
		for (auto playerIdx : indices)
		{
			auto ip = IpToString(session->GetPeerAddress(session->MembershipInfo.GetPlayerPeer(playerIdx)));
			auto kickPlayerName = Utils::String::ThinString(session->MembershipInfo.PlayerSessions[playerIdx].DisplayName);
			if (!Blam::Network::BootPlayer(playerIdx, 4))
			{
				returnInfo += "Failed to kick player " + kickPlayerName + "\n";
				continue;
			}
			if (type == KickType::Ban)
			{
				BanIp(ip);
				returnInfo += "Added IP " + ip + " to the ban list\n";
			}
			returnInfo += "Issued kick request for player " + kickPlayerName + " (" + std::to_string(playerIdx) + ")\n";
			success = true;
		}
		return success;
	}

	bool DoKickIndexCommand(KickType type, const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 1)
		{
			returnInfo = "Invalid arguments";
			return false;
		}
		auto index = -1;
		size_t pos = 0;
		try
		{
			index = std::stoi(Arguments[0], &pos);
		}
		catch (std::logic_error&)
		{
		}
		if (pos != Arguments[0].length() || index < 0 || index > 15)
		{
			returnInfo = "Invalid player index";
			return false;
		}
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
		{
			returnInfo = "You must be hosting a game to use this command";
			return false;
		}
		auto player = &session->MembershipInfo.PlayerSessions[index];
		auto kickPlayerName = Utils::String::ThinString(player->DisplayName);
		auto ip = IpToString(session->GetPeerAddress(player->PeerIndex));
		if (!Blam::Network::BootPlayer(index, 4))
		{
			returnInfo = "Failed to kick player " + kickPlayerName;
			return false;
		}
		if (type == KickType::Ban)
		{
			BanIp(ip);
			returnInfo = "Added IP " + ip + " to the ban list\n";
		}
		returnInfo += "Issued kick request for player " + kickPlayerName + " (" + std::to_string(index) + ")";
		return true;
	}

	bool CommandServerKickPlayer(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return DoKickPlayerCommand(KickType::Kick, Arguments, returnInfo);
	}

	bool CommandServerBanPlayer(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return DoKickPlayerCommand(KickType::Ban, Arguments, returnInfo);
	}

	bool CommandServerKickPlayerUid(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return DoKickUidCommand(KickType::Kick, Arguments, returnInfo);
	}

	bool CommandServerBanPlayerUid(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return DoKickUidCommand(KickType::Ban, Arguments, returnInfo);
	}

	bool CommandServerKickPlayerIndex(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return DoKickIndexCommand(KickType::Kick, Arguments, returnInfo);
	}

	bool CommandServerBanPlayerIndex(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return DoKickIndexCommand(KickType::Ban, Arguments, returnInfo);
	}

	bool CommandServerBan(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 2)
		{
			returnInfo = "Invalid arguments";
			return false;
		}
		auto banType = Arguments[0];
		if (banType == "ip")
		{
			auto ip = Arguments[1];
			BanIp(ip);
			returnInfo = "Added IP " + ip + " to the ban list";
			return true;
		}
		returnInfo = "Unsupported ban type " + banType;
		return false;
	}

	bool CommandServerUnban(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 2)
		{
			returnInfo = "Invalid arguments";
			return false;
		}
		auto banType = Arguments[0];
		if (banType == "ip")
		{
			auto ip = Arguments[1];
			if (!UnbanIp(ip))
			{
				returnInfo = "IP " + ip + " is not banned";
				return false;
			}
			returnInfo = "Removed IP " + ip + " from the ban list";
			return true;
		}
		returnInfo = "Unsupported ban type " + banType;
		return false;
	}

	bool CommandServerListPlayers(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::stringstream ss;

		// TODO: check if player is in a lobby
		// TODO: find an addr where we can find this data in clients memory
		// so people could use it to find peoples UIDs and report them for cheating etc

		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			returnInfo = "No session found, are you hosting a game?";
			return false;
		}

		if (!session->IsHost())
		{
			returnInfo = "You must be hosting a game to use this command";
			return false;
		}

		int peerIdx = session->MembershipInfo.FindFirstPeer();
		while (peerIdx != -1)
		{
			int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
			if (playerIdx != -1)
			{
				auto* player = &session->MembershipInfo.PlayerSessions[playerIdx];
				auto name = Utils::String::ThinString(player->DisplayName);
				auto ip = session->GetPeerAddress(peerIdx);
				ss << std::dec << "[" << playerIdx << "] \"" << name << "\" (uid: " << std::hex << player->Uid << ", ip: " << IpToString(ip) << ")" << std::endl;
			}

			peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
		}

		returnInfo = ss.str();
		return true;
	}

	bool CommandServerMode(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{

		typedef bool(__cdecl *SetGameOnlineFunc)(int a1);
		const SetGameOnlineFunc Server_Set_Mode = reinterpret_cast<SetGameOnlineFunc>(0xA7F950);
		bool retVal = Server_Set_Mode(Modules::ModuleServer::Instance().VarServerMode->ValueInt);
		if (retVal)
		{
			returnInfo = "Changed game mode to " + Modules::ModuleServer::Instance().VarServerMode->ValueString;
			return true;
		}
		returnInfo = "Hmm, weird. Are you at the main menu?";
		return true;
	}

	bool CommandServerLobbyType(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		typedef bool(__cdecl *SetLobbyType)(int a1);
		const SetLobbyType Server_Lobby_Type = reinterpret_cast<SetLobbyType>(0xA7EE70);
		bool retVal1 = Server_Lobby_Type(Modules::ModuleServer::Instance().VarServerLobbyType->ValueInt);
		if (retVal1)
		{
			returnInfo = "Changed lobby type to " + Modules::ModuleServer::Instance().VarServerLobbyType->ValueString;
			return true;
		}
		return false;
	}

	const uint16_t PingId = 0xF00D;
	bool CommandServerPing(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() > 1)
		{
			returnInfo = "Invalid arguments";
			return false;
		}

		auto session = Blam::Network::GetActiveSession();
		if (!session)
		{
			returnInfo = "No session available";
			return false;
		}

		// If an IP address was passed, send to that address
		if (Arguments.size() == 1)
		{
			struct in_addr inAddr;
			if (!inet_pton(AF_INET, Arguments[0].c_str(), &inAddr))
			{
				returnInfo = "Invalid IPv4 address";
				return false;
			}

			auto blamAddress = Blam::Network::NetworkAddress::FromInAddr(inAddr.S_un.S_addr, 11774);
			if (!session->Gateway->Ping(blamAddress, PingId))
			{
				returnInfo = "Failed to send ping packet";
				return false;
			}
			return true;
		}
		
		// Otherwise, send to the host
		if (!session->IsEstablished())
		{
			returnInfo = "You are not in a game. Use \"ping <ip>\" instead.";
			return false;
		}
		if (session->IsHost())
		{
			returnInfo = "You can't ping yourself. Use \"ping <ip>\" to ping someone else.";
			return false;
		}
		auto membership = &session->MembershipInfo;
		auto channelIndex = membership->PeerChannels[membership->HostPeerIndex].ChannelIndex;
		if (channelIndex == -1)
		{
			returnInfo = "You are not connected to a game. Use \"ping <ip>\" instead.";
			return false;
		}
		session->Observer->Ping(0, channelIndex, PingId);
		return true;
	}

	void PongReceived(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency)
	{
		if (id != PingId)
			return; // Only show pings sent by the ping command

		// PONG <ip> <timestamp> <latency>
		auto ipStr = IpToString(from);
		auto message = "PONG " + ipStr + " " + std::to_string(timestamp) + " " + std::to_string(latency) + "ms";
		GameConsole::Instance().consoleQueue.pushLineFromGameToUI(message);
	}

	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{
		switch (newState)
		{
		case Blam::Network::eLifeCycleStateNone:
			// Switch to global chat on the main menu
			GameConsole::Instance().SwitchToGlobalChat();
			GameConsole::Instance().gameChatQueue.visible = false;
			break;
		case Blam::Network::eLifeCycleStatePreGame:
		case Blam::Network::eLifeCycleStateInGame:
			// Switch to game chat when joining a game
			GameConsole::Instance().gameChatQueue.visible = true;
			GameConsole::Instance().SwitchToGameChat();
			break;
		}
	}

	bool SprintEnabledChanged(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto &serverModule = Modules::ModuleServer::Instance();
		auto enabled = serverModule.VarServerSprintEnabledClient->ValueInt != 0;
		Patches::Sprint::Enable(enabled);
		return true;
	}

	bool UnlimitedSprintEnabledChanged(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto &serverModule = Modules::ModuleServer::Instance();
		auto unlimited = serverModule.VarServerSprintUnlimitedClient->ValueInt != 0;
		Patches::Sprint::SetUnlimited(unlimited);
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

		AddCommand("Connect", "connect", "Begins establishing a connection to a server", eCommandFlagsRunOnMainMenu, CommandServerConnect, { "host:port The server info to connect to", "password(string) The password for the server" });
		AddCommand("Announce", "announce", "Announces this server to the master servers", eCommandFlagsHostOnly, CommandServerAnnounce);
		AddCommand("Unannounce", "unannounce", "Notifies the master servers to remove this server", eCommandFlagsHostOnly, CommandServerUnannounce);

		AddCommand("AnnounceStats", "announcestats", "Announces the players stats to the masters at the end of the game", eCommandFlagsNone, CommandServerAnnounceStats);

		AddCommand("KickPlayer", "k", "Kicks a player from the game by name (host only)", eCommandFlagsHostOnly, CommandServerKickPlayer, { "playername The name of the player to kick" });
		AddCommand("KickBanPlayer", "kb", "Kicks and IP bans a player from the game by name (host only)", eCommandFlagsHostOnly, CommandServerBanPlayer, { "playername The name of the player to ban" });

		AddCommand("KickUid", "ku", "Kicks players from the game by UID (host only)", eCommandFlagsHostOnly, CommandServerKickPlayerUid, { "uid The UID of the players to kick" });
		AddCommand("KickBanUid", "kbu", "Kicks and IP bans players from the game by UID (host only)", eCommandFlagsHostOnly, CommandServerBanPlayerUid, { "uid The UID of the players to ban" });

		AddCommand("KickIndex", "ki", "Kicks a player from the game by index (host only)", eCommandFlagsHostOnly, CommandServerKickPlayerIndex, { "index The index of the player to kick" });
		AddCommand("KickBanIndex", "kbi", "Kicks and IP bans a player from the game by index (host only)", eCommandFlagsHostOnly, CommandServerBanPlayerIndex, { "index The index of the player to ban" });

		AddCommand("AddBan", "addban", "Adds to the ban list (does NOT kick anyone)", eCommandFlagsNone, CommandServerBan, { "type The ban type (only \"ip\" is supported for now)", "val The value to add to the ban list" });
		AddCommand("Unban", "unban", "Removes from the ban list", eCommandFlagsNone, CommandServerUnban, { "type The ban type (only \"ip\" is supported for now)", "val The value to remove from the ban list" });

		AddCommand("ListPlayers", "list", "Lists players in the game (currently host only)", eCommandFlagsHostOnly, CommandServerListPlayers);

		AddCommand("Ping", "ping", "Ping a server", eCommandFlagsNone, CommandServerPing, { "[ip] The IP address of the server to ping. Omit to ping the host." });
		
		VarServerMode = AddVariableInt("Mode", "mode", "Changes the game mode for the server. 0 = Xbox Live (Open Party); 1 = Xbox Live (Friends Only); 2 = Xbox Live (Invite Only); 3 = Online; 4 = Offline;", eCommandFlagsNone, 4, CommandServerMode);
		VarServerMode->ValueIntMin = 0;
		VarServerMode->ValueIntMax = 4;

		VarServerLobbyType = AddVariableInt("LobbyType", "lobbytype", "Changes the lobby type for the server. 0 = Campaign; 1 = Matchmaking; 2 = Multiplayer; 3 = Forge; 4 = Theater;", eCommandFlagsDontUpdateInitial, 2, CommandServerLobbyType);
		VarServerLobbyType->ValueIntMin = 0;
		VarServerLobbyType->ValueIntMax = 4;

		VarServerSprintEnabled = AddVariableInt("SprintEnabled", "sprint", "Controls whether sprint is enabled on the server", static_cast<CommandFlags>(eCommandFlagsArchived | eCommandFlagsReplicated), 1);
		VarServerSprintEnabledClient = AddVariableInt("SprintEnabledClient", "sprint_client", "", eCommandFlagsInternal, 1, SprintEnabledChanged);
		Server::VariableSynchronization::Synchronize(VarServerSprintEnabled, VarServerSprintEnabledClient);

		VarServerSprintUnlimited = AddVariableInt("UnlimitedSprint", "unlimited_sprint", "Controls whether unlimited sprint is enabled on the server", static_cast<CommandFlags>(eCommandFlagsArchived | eCommandFlagsReplicated), 0);
		VarServerSprintUnlimitedClient = AddVariableInt("UnlimitedSprintClient", "unlimited_sprint_client", "", eCommandFlagsInternal, 0, UnlimitedSprintEnabledChanged);
		Server::VariableSynchronization::Synchronize(VarServerSprintUnlimited, VarServerSprintUnlimitedClient);

		VarServerDualWieldEnabled = AddVariableInt("DualWieldEnabled", "dualwield", "Controls whether dual wielding is enabled on the server", static_cast<CommandFlags>(eCommandFlagsArchived | eCommandFlagsReplicated), 1);
		VarServerDualWieldEnabledClient = AddVariableInt("DualWieldEnabledClient", "dualwield_client", "", eCommandFlagsInternal, 0);
		Server::VariableSynchronization::Synchronize(VarServerDualWieldEnabled, VarServerDualWieldEnabledClient);

#ifdef _DEBUG
		// Synchronization system testing
		auto syncTestServer = AddVariableInt("SyncTest", "synctest", "Sync test server", eCommandFlagsHidden);
		auto syncTestClient = AddVariableInt("SyncTestClient", "synctestclient", "Sync test client", eCommandFlagsHidden);
		Server::VariableSynchronization::Synchronize(syncTestServer, syncTestClient);
#endif

		Patches::Network::OnPong(PongReceived);
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);
	}
}
