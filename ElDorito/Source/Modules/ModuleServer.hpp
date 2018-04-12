#pragma once

#include "ModuleBase.hpp"
#include <cstdint>
#include <unordered_set>

namespace Modules
{
	class ModuleServer : public Utils::Singleton<ModuleServer>, public ModuleBase
	{
	public:
		Command* VarHitMarkersEnabled;
		Command* VarHitMarkersEnabledClient;
		Command* VarServerName;
		Command* VarServerNameClient;
		Command* VarServerMessage;
		Command* VarServerMessageClient;
		Command* VarServerPassword;
		Command* VarServerCountdown;
		Command* VarServerCountdownLobby;
		Command* VarServerMaxPlayers;
		Command* VarServerPort;
		Command* VarServerGamePort;
		Command* VarServerShouldAnnounce;
		Command* VarServerSprintEnabled;
		Command* VarServerSprintEnabledClient;
		Command* VarServerBottomlessClipEnabled;
		Command* VarServerBottomlessClipEnabledClient;
		Command* VarServerSprintUnlimited;
		Command* VarServerSprintUnlimitedClient;
		Command* VarServerDualWieldEnabled;
		Command* VarServerDualWieldEnabledClient;
		Command* VarServerDedicated;
		Command* VarServerDedicatedClient;
		Command* VarServerAssassinationEnabled;
		Command* VarServerAssassinationEnabledClient;
		Command* VarFloodFilterEnabled;
		Command* VarFloodMessageScoreShort;
		Command* VarFloodMessageScoreLong;
		Command* VarFloodTimeoutScore;
		Command* VarFloodTimeoutSeconds;
		Command* VarFloodTimeoutResetSeconds;
		Command* VarChatLogEnabled;
		Command* VarChatLogFile;
		Command* VarServerMapVotingTime;
		Command* VarServerVotingEnabled;
		Command* VarServerNumberOfRevotesAllowed;
		Command* VarServerNumberOfVotingOptions;
		Command* VarRconPassword;
		Command* VarServerTeamShuffleEnabled;
		Command* VarServerTimeBetweenVoteEndAndGameStart;
		Command* VarServerVotingDuplicationLevel;
		Command* VarServerVotePassPercentage;
		Command* VarTempBanDuration;
		Command* VarChatCommandKickPlayerEnabled;
		Command* VarChatCommandEndGameEnabled;
		Command* VarChatCommandVoteTime;
		Command* VarChatCommandShuffleTeamsEnabled;
		Command* VarPlayersInfo;
		Command* VarPlayersInfoClient;
		Command* VarNumTeams;
		Command* VarMinTeamSize;
		Command* VarSignalServerPort;
		Command* VarSendChatToRconClients;
		Command* VarHttpServerCacheTime;

		//Veto System Commands
		Command* VarNumberOfVetoVotes;
		Command* VarVetoSystemEnabled;
		Command* VarVetoSystemSelectionType;
		Command* VarVetoVoteTime;
		Command* VarVetoWinningOptionShownTime;
		Command* VarVetoVotePassPercentage;
		Command* VarVotingJsonPath;
		Command* VarVetoJsonPath;

		std::unordered_set<std::string> NonAllowedNames = std::unordered_set<std::string>{};
		void refreshNonAllowedNames();

		uint8_t SyslinkData[0x176];

		ModuleServer();
	};
}