#pragma once

#include "ModuleBase.h"

namespace
{
	std::string VariableGameDebugModeUpdate(const std::vector<std::string>& Arguments);
	std::string CommandGameInfo(const std::vector<std::string>& Arguments);
	std::string CommandGameExit(const std::vector<std::string>& Arguments);
	std::string CommandGameLoadMap(const std::vector<std::string>& Arguments);
	std::string CommandGameShowUI(const std::vector<std::string>& Arguments);
}

namespace Modules
{
	class ModuleGame : public Utils::Singleton<ModuleGame>, public ModuleBase
	{
	public:
		ModuleGame();

	private:
	};
}