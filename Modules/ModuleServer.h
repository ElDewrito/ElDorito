#pragma once

#include "ModuleBase.h"

namespace Modules
{
	class ModuleServer : public Utils::Singleton<ModuleServer>, public ModuleBase
	{
	public:
		Command* VarServerPassword;
		Command* VarServerCountdown;
		Command* VarServerMaxPlayers;
		Command* VarServerLanMode;
		BYTE SyslinkData[0x176];

		ModuleServer();
	};
}