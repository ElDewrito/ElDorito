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

namespace Modules
{
	class ModuleGame : public Utils::Singleton<ModuleGame>, public ModuleBase
	{
	public:
		Command* VarMedalsZip;

		int DebugFlags;
		std::vector<std::string> MapList;
		std::vector<std::string> FiltersExclude;
		std::vector<std::string> FiltersInclude;

		ModuleGame();
	};
}