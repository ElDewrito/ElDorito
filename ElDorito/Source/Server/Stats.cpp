#include <WS2tcpip.h>
#include <fstream>
#include "Stats.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Patches/Events.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Utils/Logger.hpp"
#include "../ElDorito.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/HttpRequest.hpp"
#include "../ThirdParty/rapidjson/document.h"
#include "../Patches/Network.hpp"
#include <iomanip>


namespace Server::Stats
{
	//If we send stats right when the game ends, some of the team scores arent updated yet.
	//If we wait for the submit-stats lifecycle state to fire, some of the scores are already reset to 0.
	time_t sendStatsTime = 0;

	// retrieves master server endpoints from dewrito.json
	void GetStatsEndpoints(std::vector<std::string>& destVect)
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
				if (json.HasMember("statsServers"))
				{
					auto& statsArray = json["statsServers"];
					for (auto it = statsArray.Begin(); it != statsArray.End(); it++)
					{
						destVect.push_back((*it)["url"].GetString());
					}
				}
			}
		}
	}

	//Endpoint for getting information about players in the game. Data retrieved is set as a
	//variable that is synchronized to clients, and sent to the scoreboard (or any other screen layer) as json.
	DWORD WINAPI GetPlayersInfo_Thread(LPVOID lpParam)
	{

		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost() || Modules::ModuleServer::Instance().VarPlayerInfoEndpoint->ValueString.empty())
			return false;

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		uint32_t playerInfoBase = 0x2162E08;
		writer.StartObject();
		writer.Key("players");
		writer.StartArray();
		int peerIdx = session->MembershipInfo.FindFirstPeer();
		while (peerIdx != -1)
		{
			int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
			if (playerIdx != -1)
			{
				writer.StartObject();
				auto* player = &session->MembershipInfo.PlayerSessions[playerIdx];
				std::string name = Utils::String::ThinString(player->Properties.DisplayName);

				uint16_t team = Pointer(playerInfoBase + (5696 * playerIdx) + 32).Read<uint16_t>();

				char uid[17];
				Blam::Players::FormatUid(uid, player->Properties.Uid);

				Pointer pvpBase(0x23F5A98);

				writer.Key("name");
				writer.String(name.c_str());
				writer.Key("playerIndex");
				writer.Int(playerIdx);
				writer.Key("uid");
				writer.String(uid);
				writer.EndObject();
			}
			peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
		}
		writer.EndArray();
		writer.EndObject();

		std::string server = Modules::ModuleServer::Instance().VarPlayerInfoEndpoint->ValueString;
		HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

		try
		{
			std::string sendObject = s.GetString();
			if (!req.SendRequest(Utils::String::WidenString(server), L"POST", L"", L"", L"Content-Type: application/json\r\n", (void*)sendObject.c_str(), sendObject.length()))
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Unable to connect to player info endpoint");
			}
			// make sure the server replied with 200 OK
			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Invalid server query response.");
				return false;
			}

			// parse the json response
			std::string resp = std::string(req.responseBody.begin(), req.responseBody.end());
			rapidjson::Document json;
			if (json.Parse<0>(resp.c_str()).HasParseError() || !json.IsObject())
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Invalid json returned from player info endpoint.");
				return false;
			}
			Modules::ModuleServer::Instance().VarPlayersInfo->ValueString = resp;

		}
		catch (...)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Exception while getting player info");
		}

		return true;
	}

	DWORD WINAPI CommandServerAnnounceStats_Thread(LPVOID lpParam)
	{

		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost() || !Patches::Network::IsInfoSocketOpen())
			return false;

		std::vector<std::string> statsEndpoints;
		GetStatsEndpoints(statsEndpoints);

		if (statsEndpoints.size() == 0) {
			return false;
		}
		

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("gameVersion");
		writer.String(Utils::Version::GetVersionString().c_str());
		writer.Key("serverName");
		writer.String(Modules::ModuleServer::Instance().VarServerName->ValueString.c_str());
		writer.Key("serverPort");
		writer.Int(Modules::ModuleServer::Instance().VarServerPort->ValueInt);
		writer.Key("port");
		writer.Int(Pointer(0x1860454).Read<uint32_t>());
		writer.Key("hostPlayer");
		writer.String(Modules::ModulePlayer::Instance().VarPlayerName->ValueString.c_str());

		writer.Key("game");
		writer.StartObject();

		writer.Key("sprintEnabled");
		writer.Bool(Modules::ModuleServer::Instance().VarServerSprintEnabled->ValueInt != 0);
		writer.Key("sprintUnlimitedEnabled");
		writer.Bool(Modules::ModuleServer::Instance().VarServerSprintUnlimited->ValueInt != 0);
		writer.Key("maxPlayers");
		writer.Int(Modules::ModuleServer::Instance().VarServerMaxPlayers->ValueInt);

		std::string mapName((char*)Pointer(0x22AB018)(0x1A4));
		std::wstring mapVariantName((wchar_t*)Pointer(0x1863ACA));
		std::wstring variantName((wchar_t*)Pointer(0x23DAF4C));

		Pointer &gameModePtr = ElDorito::GetMainTls(GameGlobals::GameInfo::TLSOffset)[0](GameGlobals::GameInfo::GameMode);
		uint32_t gameMode = gameModePtr.Read<uint32_t>();
		int32_t variantType = Pointer(0x023DAF18).Read<int32_t>();
		if (gameMode == 3)
		{
			if (mapName == "mainmenu")
			{
				mapName = std::string((char*)Pointer(0x19A5E49));
				variantName = std::wstring((wchar_t*)Pointer(0x179254));
				variantType = Pointer(0x179250).Read<uint32_t>();
			}
		}

		writer.Key("mapName");
		writer.String(Utils::String::ThinString(mapVariantName).c_str());
		writer.Key("mapFile");
		writer.String(mapName.c_str());
		writer.Key("variant");
		writer.String(Utils::String::ThinString(variantName).c_str());
		if (variantType >= 0 && variantType < Blam::GameTypeCount)
		{
			writer.Key("variantType");
			writer.String(Blam::GameTypeNames[variantType].c_str());
		}
		uint32_t TeamMode = Pointer(0x019A6210).Read<uint32_t>();
		writer.Key("teamGame");
		writer.Bool(TeamMode != 0);

		if (TeamMode == 1){
			writer.Key("teamScores");
			writer.StartArray();

			auto engineGlobalsPtr = ElDorito::GetMainTls(0x48);
			if (engineGlobalsPtr)
			{
				auto engineGobals = engineGlobalsPtr[0](0x101F4);
				for (int t = 0; t < 8; t++)
				{
					auto teamscore = engineGobals(t * 0x1A).Read<Blam::TEAM_SCORE>();
					writer.Int(teamscore.Score);
				}

			}
			writer.EndArray();
		}
		writer.EndObject();
		uint32_t playerInfoBase = 0x2162E08;
		Pointer p(0x023F1724);
		writer.Key("players");
		writer.StartArray();
		int peerIdx = session->MembershipInfo.FindFirstPeer();
		while (peerIdx != -1)
		{
			int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
			if (playerIdx != -1)
			{

				auto playerStats = Blam::Players::GetStats(playerIdx);
				writer.StartObject();
				auto* player = &session->MembershipInfo.PlayerSessions[playerIdx];
				std::string name = Utils::String::ThinString(player->Properties.DisplayName);

				struct in_addr inAddr;
				inAddr.S_un.S_addr = session->GetPeerAddress(peerIdx).ToInAddr();
				char ipStr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &inAddr, ipStr, sizeof(ipStr));

				uint16_t team = Pointer(playerInfoBase + (5696 * playerIdx) + 32).Read<uint16_t>();

				char uid[17];
				Blam::Players::FormatUid(uid, player->Properties.Uid);

				Pointer pvpBase(0x23F5A98);

				writer.Key("name");
				writer.String(name.c_str());
				writer.Key("ip");
				writer.String(ipStr);
				writer.Key("team");
				writer.Int(team);
				writer.Key("playerIndex");
				writer.Int(playerIdx);
				writer.Key("uid");
				writer.String(uid);
				std::stringstream color;
				color << "#" << std::setw(6) << std::setfill('0') << std::hex << player->Properties.Customization.Colors[Blam::Players::ColorIndices::Primary];
				writer.Key("primaryColor");
				writer.String(color.str().c_str());
				writer.Key("playerGameStats");
				writer.StartObject();
				writer.Key("score");
				writer.Int(playerStats.Score);
				writer.Key("kills");
				writer.Int(playerStats.Kills);
				writer.Key("assists");
				writer.Int(playerStats.Assists);
				writer.Key("deaths");
				writer.Int(playerStats.Deaths);
				writer.Key("betrayals");
				writer.Int(playerStats.Betrayals);
				writer.Key("timeSpentAlive");
				writer.Int(playerStats.TimeSpentAlive);
				writer.Key("suicides");
				writer.Int(playerStats.Suicides);
				writer.Key("bestStreak");
				writer.Int(playerStats.BestStreak);
				writer.EndObject();

				//MEDALS
				writer.Key("playerMedals");
				writer.StartArray();

				for (int i = 0; i < Blam::Tags::Objects::MedalType::MedalCount; i++)
				{
					if (playerStats.Medals[i] > 0)
					{
						writer.StartObject();
						writer.Key("medalName");
						writer.String(Blam::Tags::Objects::MedalTypeNames[i].c_str());
						writer.Key("count");
						writer.Int(playerStats.Medals[i]);
						writer.EndObject();
					}
				}
				//The sniper headshots medal is broken, so lets get it manually by grabbing the headshots from each sniper
				uint16_t SniperRifleHeadshots = p((playerIdx * 0x438) + 0x1F6).Read<uint16_t>();
				uint16_t BeamRifleHeadshots = p((playerIdx * 0x438) + 0x202).Read<uint16_t>();

				writer.StartObject();
				writer.Key("medalName");
				writer.String("SniperHeadshots");
				writer.Key("count");
				writer.Int(SniperRifleHeadshots + BeamRifleHeadshots);
				writer.EndObject();

				writer.EndArray();

				//WEAPONS
				writer.Key("playerWeapons");
				writer.StartArray();
				for (int i = 0; i < Blam::Tags::Objects::DamageReportingType::DamageCount; i++)
				{
					if (playerStats.WeaponStats[i].Initialized == 1)
					{
						writer.StartObject();
						writer.Key("weaponName");
						writer.String(Blam::Tags::Objects::DamageReportingTypeNames[i].c_str());
						writer.Key("weaponIndex");
						writer.Int(i + 1);
						writer.Key("kills");
						writer.Int(playerStats.WeaponStats[i].Kills);
						writer.Key("killedBy");
						writer.Int(playerStats.WeaponStats[i].KilledBy);
						writer.Key("betrayalsWith");
						writer.Int(playerStats.WeaponStats[i].BetrayalsWith);
						writer.Key("suicidesWith");
						writer.Int(playerStats.WeaponStats[i].SuicidesWith);
						writer.Key("headshotsWith");
						writer.Int(playerStats.WeaponStats[i].HeadshotsWith);

						writer.EndObject();
					}

				}
				writer.EndArray();

				writer.Key("otherStats");
				writer.StartObject();

				int nemesisIdx = 0;
				uint16_t nemesisKills = 0;
				int currentLocation = 0x02;
				for (int loc = 0; loc < 16; loc++)
				{
					uint16_t currKills = pvpBase((playerIdx * 0x40) + currentLocation).Read<uint16_t>();
					if (currKills > nemesisKills)
					{
						nemesisKills = currKills;
						nemesisIdx = loc;
					}
					currentLocation += 0x04;
				}

				writer.Key("nemesisIndex");
				writer.Int(nemesisIdx);

				writer.Key("kingsKilled");
				writer.Int(playerStats.KingsKilled);
				writer.Key("humansInfected");
				writer.Int(playerStats.HumansInfected);
				writer.Key("zombiesKilled");
				writer.Int(playerStats.ZombiesKilled);
				writer.Key("timeInHill");
				writer.Int(playerStats.TimeInHill);
				writer.Key("timeControllingHill");
				writer.Int(playerStats.TimeControllingHill);
				writer.EndObject();

				writer.Key("playerVersusPlayerKills");
				writer.StartArray();

				int pvpIndex = 0x00;
				for (int loc = 0; loc < 16; loc++)
				{
					writer.Int(pvpBase((playerIdx * 0x40) + pvpIndex).Read<uint16_t>());
					pvpIndex += 0x04;
				}

				writer.EndArray();
				writer.EndObject();
			}
			peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
		}
		writer.EndArray();
		writer.EndObject();


		for (auto server : statsEndpoints)
		{
			HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

			try
			{
				std::string sendObject = s.GetString();
				if (!req.SendRequest(Utils::String::WidenString(server), L"POST", L"", L"", L"Content-Type: application/json\r\n", (void*)sendObject.c_str(), sendObject.length()))
				{
					Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Unable to connect to stats server");
				}
			}
			catch (...)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Exception while sending stats to server");
			}

		}		

		return true;
	}
	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{
		switch (newState)
		{
			case Blam::Network::eLifeCycleStateStartGame:
			{
				auto thread = CreateThread(NULL, 0, GetPlayersInfo_Thread, (LPVOID)"", 0, NULL);
				break;
			}

		}
	}
	void OnEvent(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
	{
		if (event->NameStringId == 262221) //Game Ended event
		{
			time(&sendStatsTime);
		}
		if (event->NameStringId == 262214) //player joined
		{
			auto thread = CreateThread(NULL, 0, GetPlayersInfo_Thread, (LPVOID)"", 0, NULL);
		}
	}
}


namespace Server::Stats
{
	void Init()
	{
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);
		Patches::Events::OnEvent(OnEvent);
		
	}
	void Tick()
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
			return;

		time_t curTime1;
		time(&curTime1);

		if (sendStatsTime != 0)
		{

			//If we send stats right when the game ends, some of the team scores arent updated yet.
			//If we wait for the submit-stats lifecycle state to fire, some of the scores are already reset to 0.
			auto elapsed = curTime1 - sendStatsTime;
			if (elapsed > 1)
			{
				auto thread = CreateThread(NULL, 0, CommandServerAnnounceStats_Thread, (LPVOID)"", 0, NULL);
				sendStatsTime = 0;
			}
		}
	}
}
