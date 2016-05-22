#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleServer : public Utils::Singleton<ModuleServer>, public ModuleBase
	{
	public:
		Command* VarServerName;
		Command* VarServerPassword;
		Command* VarServerCountdown;
		Command* VarServerMaxPlayers;
		Command* VarServerPort;
		Command* VarServerShouldAnnounce;
		Command* VarServerMode;
		Command* VarServerLobbyType;
		Command* VarServerSprintEnabled;
		Command* VarServerSprintEnabledClient;
		Command* VarServerSprintUnlimited;
		Command* VarServerSprintUnlimitedClient;
		Command* VarServerGravityEnabled;
		Command* VarServerGravityEnabledClient;
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

		BYTE SyslinkData[0x176];

		ModuleServer();
	};
}
