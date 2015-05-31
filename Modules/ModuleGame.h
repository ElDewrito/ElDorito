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
	bool CommandGameLogMode(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool CommandGameLogFilter(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool CommandGameInfo(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool CommandGameExit(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool CommandGameLoadMap(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool CommandGameShowUI(const std::vector<std::string>& Arguments, std::string& returnInfo);
}

namespace Modules
{
	class ModuleGame : public Utils::Singleton<ModuleGame>, public ModuleBase
	{
	public:
		int DebugFlags;
		std::vector<std::string> MapList;
		std::vector<std::string> FiltersExclude;
		std::vector<std::string> FiltersInclude;

		ModuleGame();
	};
}