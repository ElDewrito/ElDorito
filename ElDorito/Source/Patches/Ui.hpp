#pragma once
#include <cstdint>
#include <functional>
#include <map>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../Blam/Text/StringID.hpp"
#include "../Patch.hpp"

namespace Patches::Ui
{
	void ApplyAll();
	void ApplyAfterTagsLoaded();

	void ApplyMapNameFixes();
	void ApplyUIResolution();

	typedef std::function<void(HWND window)> CreateWindowCallback;

	// Registers a function to be called when the game window is created.
	void OnCreateWindow(CreateWindowCallback callback);

	void *ShowDialog(const Blam::Text::StringID p_DialogID, const int32_t p_Arg1 = 0, const int32_t p_Flags = 4, const Blam::Text::StringID p_ParentID = 0);

	void FindUiTagData();

	void TogglePTTSound(bool enabled);
	void ToggleSpeakingPlayerName(std::string name, bool speaking);
	void UpdateSpeakingPlayerWidget(bool mapLoaded);

	void UpdateHUDDistortion();

	void ShowLanBrowser();

	//These basically duplicate existing data from modules.
	//However they make accessing in hooks a whole lot easier...
	//Probably needs refactoring.
	extern bool enableCustomHUDColors;
	extern bool enableAllyBlueWaypointsFix;
	extern int customPrimaryHUDColor;
	extern int customSecondaryHUDColor;
}
