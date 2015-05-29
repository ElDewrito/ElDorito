#pragma once

#include "ModuleBase.h"

enum DebugLoggingModes
{
	Network = 1,
	SSL = 2,
	UI = 4,
	Game1 = 8,
	Game2 = 16,
};

namespace
{
	std::string CommandGameLogMode(const std::vector<std::string>& Arguments);
	std::string CommandGameLogFilter(const std::vector<std::string>& Arguments);
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

		int DebugFlags;
		std::vector<std::string> MapList;
		std::vector<std::string> FiltersExclude;
		std::vector<std::string> FiltersInclude;
	private:
	};
}