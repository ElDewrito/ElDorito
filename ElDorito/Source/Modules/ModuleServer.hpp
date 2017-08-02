#pragma once

#include "ModuleBase.hpp"
#include <cstdint>

namespace Modules
{
	class ModuleServer : public Utils::Singleton<ModuleServer>, public ModuleBase
	{
	public:
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
		Command* VarServerMode;
		Command* VarServerLobbyType;
		Command* VarServerSprintEnabled;
		Command* VarServerSprintEnabledClient;
		Command* VarServerBottomlessClipEnabled;
		Command* VarServerBottomlessClipEnabledClient;
		Command* VarServerSprintUnlimited;
		Command* VarServerSprintUnlimitedClient;
		Command* VarServerDualWieldEnabled;
		Command* VarServerDualWieldEnabledClient;
		Command* VarServerAssassinationEnabled;
		Command* VarFloodFilterEnabled;
		Command* VarFloodMessageScoreShort;
		Command* VarFloodMessageScoreLong;
		Command* VarFloodTimeoutScore;
		Command* VarFloodTimeoutSeconds;
		Command* VarFloodTimeoutResetSeconds;
		Command* VarChatLogEnabled;
		Command* VarChatLogPath;
		Command* VarServerMapVotingTime;
		Command* VarServerVotingEnabled;
		Command* VarServerNumberOfRevotesAllowed;
		Command* VarServerNumberOfVotingOptions;
		Command* VarServerAutoHost;
		Command* VarStatsServer;
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
		Command* VarPlayerInfoEndpoint;
		Command* VarMaxTeamSize;
		Command* VarSignalServerPort;
		Command* VarSendChatToRconClients;

		//Veto System Commands
		Command* VarNumberOfVetoVotes;
		Command* VarVetoSystemEnabled;
		Command* VarVetoSystemSelectionType;
		Command* VarVetoVoteTime;
		Command* VarVetoWinningOptionShownTime;
		Command* VarVetoVotePassPercentage;

		uint8_t SyslinkData[0x176];

		ModuleServer();
	};
}