#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleGame : public Utils::Singleton<ModuleGame>, public ModuleBase
	{
	public:
		Command* VarLanguageID;
		Command* VarSkipTitleSplash;
		Command* VarLogName;
		Command* VarMenuURL;
		Command* VarRconPort;
		Command* VarMedalPack;
		Command* VarFirstRun;
		Command* VarAnnouncerVol;
		Command* VarExpandedScoreboard;
		Command* VarHideChat;
		Command* VarSuppressJuggling;

		int DebugFlags;

		std::vector<std::string> CustomMapList;
		std::vector<std::string> MapList;
		std::vector<std::string> MedalPackList;
		std::vector<std::string> FiltersExclude;
		std::vector<std::string> FiltersInclude;

		ModuleGame();
		void UpdateMapList();
		void UpdateCustomMapList();
	};
}