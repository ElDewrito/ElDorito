#include "DiscordRPC.h"

#include <string.h>
#include <time.h>

#include "../Blam/BlamEvents.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Events.hpp"
#include "../Patches/Network.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Pointer.hpp"
#include "../CommandMap.hpp"
#include "../Utils/String.hpp"

#include "../Utils/Logger.hpp"

static const char* APPLICATION_ID = "378984448022020112";

namespace
{
	std::string VariantMapName;
	std::string LobbyTypeString;
	std::string isOnlineString;
	std::string localPlayerScore;
	std::string enemyScore;
	std::string HasTeamsString;
	Blam::Network::PlayerSession localPlayerSession;
	Blam::Players::PlayerProperties localPlayerProperties;
	uint32_t gameType;
	int localPlayerTeamIndex;
	int localPlayerIndex;

	const std::string TeamColor[] = {
		"Red",
		"Blue",
		"Green",
		"Orange",
		"Purple",
		"Gold",
		"Brown",
		"Pink"
	};

	const std::string InfectionColor[] = {
		"Human",
		"Zombie",
		"Green",
		"Orange",
		"Purple",
		"Gold",
		"Brown",
		"Pink"
	};

	void handleDiscordReady()
	{
		//stub
	}

	void handleDiscordDisconnected(int errorCode, const char *message)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Discord-RPC: %s", std::string(message));
	}

	void handleDiscordError(int errorCode, const char *message)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Discord-RPC: %s", std::string(message));
	}

	void handleDiscordJoin(const char *joinSecret)
	{
		Modules::CommandMap::Instance().ExecuteCommand("connect " + std::string(joinSecret));
	}

	void handleDiscordSpectate(const char *spectateSecret)
	{
		//rip
	}

	void handleDiscordJoinRequest(const DiscordJoinRequest *joinRequest)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(jsonBuffer);
		
		writer.StartObject();
		writer.Key("userId");
		writer.String(joinRequest->userId);
		writer.Key("username");
		writer.String(joinRequest->username);
		writer.Key("avatar");
		writer.String(joinRequest->avatar);
		writer.EndObject();

		Web::Ui::ScreenLayer::Notify("discord-joinrequest", jsonBuffer.GetString(), true);
	}

	void PresenceUpdate()
	{
		memset(&Discord::DiscordRPC::Instance().discordPresence, 0, sizeof(Discord::DiscordRPC::discordPresence));

		auto* session = Blam::Network::GetActiveSession();
		auto get_multiplayer_scoreboard = (Blam::MutiplayerScoreboard*(*)())(0x00550B80);

		bool isOnline;
		int lobbyType;
		int serverPlayerLimit = 0;

		//2 = Multiplayer, 3 = Forge
		lobbyType = Blam::Network::GetLobbyType();
		//3 = Online, 4 = Offline
		if (Blam::Network::GetNetworkMode() == 3) isOnline = 1;
		else if (Blam::Network::GetNetworkMode() == 4) isOnline = 0;

		serverPlayerLimit = Modules::ModuleServer::Instance().VarServerMaxPlayers->ValueInt;

		//These are used both for FFA and Teams.
		int TopScoreIndex = 0;
		int SecondScoreIndex = 0;
		int PlayerCount = 0;
		int TeamPlayerCount[8];

		int enemyPlayerIdx = 0;

		std::string GameVariantName(Utils::String::ThinString((wchar_t*)Pointer(0x23DAF4C)));
		std::string BaseMapName((char*)Pointer(0x22AB018)(0x1A4));
		std::string ServerNameClient = Modules::ModuleServer::Instance().VarServerNameClient->ValueString;

		//This is used in both lobbies count and in the FFA section below.
		//It uses peers - 1 so that it can work in lobbies
		if (session && (isOnline == true || BaseMapName != "mainmenu")) {
			int playerIdx = session->MembershipInfo.FindFirstPlayer();
			localPlayerIndex = session->MembershipInfo.GetPeerPlayer(localPlayerSession.PeerIndex);
			while (playerIdx != -1)
			{
				PlayerCount++;
				playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
				enemyPlayerIdx = (localPlayerIndex + 1 > 15 ? 0 : localPlayerIndex + 1);
			}
		}

		if (session && BaseMapName != "mainmenu")
		{
			gameType = session->Parameters.GameVariant.Get()->GameType;
			VariantMapName = Utils::String::ThinString(session->Parameters.MapVariant.Get()->ContentHeader.Name);
			localPlayerSession = session->MembershipInfo.GetLocalPlayerSession();
			auto* scoreboard = get_multiplayer_scoreboard();
			localPlayerProperties = localPlayerSession.Properties;
			localPlayerTeamIndex = localPlayerProperties.TeamIndex;
			localPlayerIndex = session->MembershipInfo.GetPeerPlayer(localPlayerSession.PeerIndex);

			if (session->HasTeams())
			{
				HasTeamsString = ((gameType != 10U ? TeamColor[localPlayerTeamIndex] : InfectionColor[localPlayerTeamIndex]) + " Team");
				int playerIdx = session->MembershipInfo.FindFirstPlayer();
				int teamIdx;
				int enemyTeamIdx = (localPlayerIndex + 1 > 7 ? 0 : localPlayerIndex + 1); //This will be used for enemy score in the case there are two or less teams.
				while (playerIdx != -1)
				{
					teamIdx = session->MembershipInfo.PlayerSessions[playerIdx].Properties.TeamIndex;
					TeamPlayerCount[teamIdx]++;
					if (teamIdx != localPlayerTeamIndex) enemyTeamIdx = teamIdx;
					playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
				}
				int TeamCount = 0;
				for (int t = 0; t < 8; t++)
					if (TeamPlayerCount[t] > 0) TeamCount++;
				for (int t = 0; t < 8; t++) {
					if (scoreboard->TeamScores[t].Score > scoreboard->TeamScores[TopScoreIndex].Score) TopScoreIndex = t;
					else if (scoreboard->TeamScores[t].Score > scoreboard->TeamScores[SecondScoreIndex].Score) SecondScoreIndex = t;
				}
				if (TeamCount > 2) {
					if (localPlayerTeamIndex == TopScoreIndex) {
						enemyScore = std::to_string(scoreboard->TeamScores[SecondScoreIndex].Score);
					}
					else {
						enemyScore = std::to_string(scoreboard->TeamScores[TopScoreIndex].Score);
					}
				}
				else enemyScore = std::to_string(scoreboard->TeamScores[enemyTeamIdx].Score);
				localPlayerScore = std::to_string(scoreboard->TeamScores[localPlayerTeamIndex].Score);
			}
			else
			{
				HasTeamsString = "FFA";
				for (int t = 0; t < 16; t++) {
					if (scoreboard->PlayerScores[t].Score > scoreboard->PlayerScores[TopScoreIndex].Score) TopScoreIndex = t;
					else if (scoreboard->PlayerScores[t].Score > scoreboard->PlayerScores[SecondScoreIndex].Score || PlayerCount == 2) SecondScoreIndex = t;
				}
				if (PlayerCount > 2) {
					if (localPlayerIndex == TopScoreIndex) {
						enemyScore = std::to_string(scoreboard->PlayerScores[SecondScoreIndex].Score);
					}
					else {
						enemyScore = std::to_string(scoreboard->PlayerScores[TopScoreIndex].Score);
					}
				}
				else enemyScore = std::to_string(scoreboard->PlayerScores[enemyPlayerIdx].Score);
				localPlayerScore = std::to_string(scoreboard->PlayerScores[localPlayerIndex].Score);
			}
		}

		switch (isOnline) {
		case 0:
			isOnlineString = "Offline";
			break;
		case 1:
			isOnlineString = "Online";
			break;
		}

		switch (lobbyType) {
		case 2:
			LobbyTypeString = "MP";
			break;
		case 3:
			LobbyTypeString = "Forge";
			break;
		default:
			LobbyTypeString = "Undefined";
			break;
		}

		Discord::DiscordRPC::Instance().discordPresence.largeImageKey = "default";
		std::string LobbyStateString = ("In an " + isOnlineString + " " + LobbyTypeString + " Lobby");
		std::string LobbyConnectedServerString = ("Connected to " + ServerNameClient);
		std::string InGameStateString = (LobbyTypeString + " on " + (isOnline == true ? (ServerNameClient) : " Local"));
		std::string InGameDetailString = (GameVariantName + " | " + localPlayerScore + " to " + enemyScore);
		std::string InGameLargeImageTextString = "Map: " + (VariantMapName.empty() ? BaseMapName : VariantMapName);
		//changes the color of the image depending on the team color
		std::string InGameSmallImageString = (session->HasTeams() ? Utils::String::ToLower(gameType != 10U ? TeamColor[localPlayerTeamIndex] : InfectionColor[localPlayerTeamIndex]) : "red") + "_" + Blam::GameTypeNames[(int)(gameType)];
		std::string InGameSmallImageTextString = HasTeamsString;

		if (BaseMapName == "mainmenu")
		{
			if (!(lobbyType == 2 || lobbyType == 3))
				Discord::DiscordRPC::Instance().discordPresence.state = "At The Main Menu";
			else
			{
				Discord::DiscordRPC::Instance().discordPresence.state = LobbyStateString.c_str();
				if (isOnline == true)
					Discord::DiscordRPC::Instance().discordPresence.details = LobbyConnectedServerString.c_str();
			}
		}
		else
		{
			Discord::DiscordRPC::Instance().discordPresence.state = InGameStateString.c_str();
			Discord::DiscordRPC::Instance().discordPresence.details = InGameDetailString.c_str();
			Discord::DiscordRPC::Instance().discordPresence.largeImageKey = BaseMapName.c_str();
			Discord::DiscordRPC::Instance().discordPresence.largeImageText = InGameLargeImageTextString.c_str();
			Discord::DiscordRPC::Instance().discordPresence.smallImageKey = InGameSmallImageString.c_str();
			Discord::DiscordRPC::Instance().discordPresence.smallImageText = InGameSmallImageTextString.c_str();
		}

		if (isOnline && (lobbyType == 2 || lobbyType == 3)) {
			Discord::DiscordRPC::Instance().discordPresence.partySize = PlayerCount;
			Discord::DiscordRPC::Instance().discordPresence.partyMax = serverPlayerLimit;
		}

		Discord_UpdatePresence(&Discord::DiscordRPC::Instance().discordPresence);
	}

	void MapLoaded(const char* mappath)
	{
		PresenceUpdate();
	}

	void Event(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
	{
		PresenceUpdate();
	}

	void Lifecycle(Blam::Network::LifeCycleState newState)
	{
		PresenceUpdate();
	}
}

namespace Discord
{

	void DiscordRPC::UpdatePresence()
	{
		PresenceUpdate();
	}

	DiscordRPC::DiscordRPC()
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = handleDiscordReady;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.errored = handleDiscordError;
		handlers.joinGame = handleDiscordJoin;
		handlers.spectateGame = handleDiscordSpectate;
		handlers.joinRequest = handleDiscordJoinRequest;
		Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

		Patches::Core::OnShutdown(Discord_Shutdown);
		Patches::Core::OnGameStart(PresenceUpdate);
		Patches::Core::OnMapLoaded(MapLoaded);
		Patches::Events::OnEvent(Event);
		Patches::Network::OnLifeCycleStateChanged(Lifecycle);
	}

	void DiscordRPC::ReplyToJoinRequest(const char* userId, int reply)
	{
		Discord_Respond(userId, reply);
	}

	void DiscordRPC::Update()
	{
#ifdef DISCORD_DISABLE_IO_THREAD
		Discord_UpdateConnection();
#endif
		Discord_RunCallbacks();
	}
}