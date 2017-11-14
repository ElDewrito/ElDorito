#include "ModuleDiscord.hpp"
#include "ModuleServer.hpp"
#include "discord-rpc.h"
#include "../ElDorito.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/String.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Events.hpp"
#include "../Patches/Network.hpp"
#include "../ThirdParty/ratelimiter/rate_limiter_interface.hpp"
#include "../ThirdParty/ratelimiter/rate_limiter.hpp"

#include <ctime>

static const char* APPLICATION_ID = "378684431830876170";

namespace
{
	//The hook used for lobby type runs every frame. This limits that.
	RateLimiterInterface* lobbyLimiter = new RateLimiter();

	std::string VariantMapName;
	std::string LobbyTypeString;
	std::string isOnlineString;
	std::string localPlayerScore;
	std::string enemyScore;
	std::string HasTeamsString;
	Blam::Network::PlayerSession localPlayerSession;
	Blam::Players::PlayerProperties localPlayerProperties;
	int localPlayerTeamIndex;
	int localPlayerIndex;

	bool isOnline;
	int lobbyType;
	int serverPlayerLimit = 0;

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

		lobbyLimiter->set_rate(2.0);
	}

	void UpdatePresence()
	{
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));

		Blam::GameType gameType = (Blam::GameType)(int32_t)Pointer(0x023DAF18);
		auto* session = Blam::Network::GetActiveSession();
		auto get_multiplayer_scoreboard = (Blam::MutiplayerScoreboard*(*)())(0x00550B80);

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

		std::string GameVariantName(Utils::String::ThinString((wchar_t*)Pointer(0x23DAF4C)));
		std::string BaseMapName((char*)Pointer(0x22AB018)(0x1A4));
		std::string ServerNameClient = Modules::ModuleServer::Instance().VarServerNameClient->ValueString;

		//This is used in both lobbies count and in the FFA section below.
		//It uses peers - 1 so that it can work in lobbies
		if (session && (isOnline == true || BaseMapName != "mainmenu")) {
			int playerIdx = session->MembershipInfo.FindFirstPlayer();
			while (playerIdx != -1)
			{
				PlayerCount++;
				playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
			}
		}

		if (session && BaseMapName != "mainmenu")
		{
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
				if (localPlayerIndex == TopScoreIndex) {
					enemyScore = std::to_string(scoreboard->PlayerScores[SecondScoreIndex].Score);
				}
				else {
					enemyScore = std::to_string(scoreboard->PlayerScores[TopScoreIndex].Score);
				}
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

		discordPresence.largeImageKey = "default";
		std::string LobbyStateString = ("In an " + isOnlineString + " " + LobbyTypeString + " Lobby");
		std::string LobbyConnectedServerString = ("Connected to " + ServerNameClient);
		std::string InGameStateString = (LobbyTypeString + " on " + (isOnline == true ? (ServerNameClient) : " Local"));
		std::string InGameDetailString = (GameVariantName + " | " + localPlayerScore + " to " + enemyScore);
		std::string InGameLargeImageTextString = "Map: " + (VariantMapName.empty() ? BaseMapName : VariantMapName);
		std::string InGameSmallImageString = "game_" + Blam::GameTypeNames[(gameType + 1)];
		std::string InGameSmallImageTextString = HasTeamsString;

		if (BaseMapName == "mainmenu")
		{
			if (!(lobbyType == 2 || lobbyType == 3))
				discordPresence.state = "At The Main Menu";
			else
			{
				discordPresence.state = LobbyStateString.c_str();
				if (isOnline == true)
					discordPresence.details = LobbyConnectedServerString.c_str();
			}
		}
		else
		{
			discordPresence.state = InGameStateString.c_str();
			discordPresence.details = InGameDetailString.c_str();
			discordPresence.largeImageKey = BaseMapName.c_str();
			discordPresence.largeImageText = InGameLargeImageTextString.c_str();
			discordPresence.smallImageKey = InGameSmallImageString.c_str();
			discordPresence.smallImageText = InGameSmallImageTextString.c_str();
		}

		if (isOnline && (lobbyType == 2 || lobbyType == 3)) {
			discordPresence.partySize = PlayerCount;
			discordPresence.partyMax = serverPlayerLimit;
		}

		Discord_UpdatePresence(&discordPresence);
	}

	void UpdatePresenceStart() {
		UpdatePresence();
	}

	void UpdatePresenceMap(const char* mapPath) {
		UpdatePresence();
	}

	void UpdatePresenceLifecycleChanged(Blam::Network::LifeCycleState newState)
	{
		UpdatePresence();
	}

	void UpdatePresenceOnEvent(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
	{
		UpdatePresence();
	}

	void DiscordShutdown()
	{
		Discord_Shutdown();
	}

	bool CommandTestPresence(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
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
		Patches::Events::OnEvent(UpdatePresenceOnEvent);
		Patches::Network::OnLifeCycleStateChanged(UpdatePresenceLifecycleChanged);
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