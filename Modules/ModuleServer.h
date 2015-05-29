#pragma once

#include "ModuleBase.h"

namespace
{
	std::string VariableServerPasswordUpdate(const std::vector<std::string>& Arguments);
	std::string VariableServerCountdownUpdate(const std::vector<std::string>& Arguments);
	std::string CommandServerConnect(const std::vector<std::string>& Arguments);
}

namespace Modules
{
	class ModuleServer : public Utils::Singleton<ModuleServer>, public ModuleBase
	{
	public:
		Command* VarServerPassword;
		Command* VarServerCountdown;
		BYTE SyslinkData[0x176];

		ModuleServer();
	};
}