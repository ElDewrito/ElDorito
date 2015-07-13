#pragma once

#include "ModuleBase.hpp"

enum DebugLoggingModes
{
	eDebugLoggingModeNetwork = 1,
	eDebugLoggingModeSSL = 2,
	eDebugLoggingModeUI = 4,
	eDebugLoggingModeGame1 = 8,
	eDebugLoggingModeGame2 = 16,
};

namespace Modules
{
	class ModuleGame : public Utils::Singleton<ModuleGame>, public ModuleBase
	{
	public:
		Command* VarLanguageID;
		Command* VarSkipLauncher;
		Command* VarLogName;
		Command* VarMenuURL;

		int DebugFlags;
		std::vector<std::string> MapList;
		std::vector<std::string> FiltersExclude;
		std::vector<std::string> FiltersInclude;

		ModuleGame();
	};
}