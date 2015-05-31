#pragma once

#include "ModuleBase.h"

namespace
{
	bool VariableServerPasswordUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariableServerCountdownUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariableServerMaxPlayersUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);

	bool CommandServerConnect(const std::vector<std::string>& Arguments, std::string& returnInfo);
}

namespace Modules
{
	class ModuleServer : public Utils::Singleton<ModuleServer>, public ModuleBase
	{
	public:
		Command* VarServerPassword;
		Command* VarServerCountdown;
		Command* VarServerMaxPlayers;
		BYTE SyslinkData[0x176];

		ModuleServer();
	};
}