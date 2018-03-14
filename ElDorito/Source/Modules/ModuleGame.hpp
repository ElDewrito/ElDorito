#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleGame : public Utils::Singleton<ModuleGame>, public ModuleBase
	{
	public:
		Command* VarLanguage;
		Command* VarSkipTitleSplash;
		Command* VarSkipIntroVideos;
		Command* VarLogName;
		Command* VarMenuURL;
		Command* VarRconPort;
		Command* VarMedalPack;
		Command* VarFirstRun;
		Command* VarExpandedScoreboard;
		Command* VarHideChat;
		Command* VarSuppressJuggling;
		Command* VarIconSet;
		Command* VarHideH3UI;
		Command* VarScreenshotsFolder;
		Command *VarScreenshotNoticeDisabled;
		Command* VarCefMedals;
		Command* VarFpsLimiter;
		Command* VarDiscordEnable;
		Command* VarDiscordAutoAccept;

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