#include "LoadingScreen.hpp"
#include "../Patch.hpp"

using namespace Patches::LoadingScreen;

namespace
{
	void ShowLoadingScreenHook(const char *mapPath, short unk);
	void BeginLoadingHook(uint32_t totalBytes);
	void UpdateLoadingProgressHook(uint32_t bytes);
	void HideLoadingScreenHook();

	std::shared_ptr<LoadingScreenUi> ActiveUi;
}

namespace Patches::LoadingScreen
{
	void ApplyAll()
	{
		Hook(0x167935, ShowLoadingScreenHook, HookFlags::IsCall).Apply();
		Hook(0x161E1E, BeginLoadingHook, HookFlags::IsCall).Apply();
		Hook(0x2EBA07, UpdateLoadingProgressHook, HookFlags::IsCall).Apply();
		Hook(0x167ABA, HideLoadingScreenHook, HookFlags::IsCall).Apply();

		// Force a jump so that the loading screen never renders
		Patch(0x1064E7, { 0xEB }).Apply();
		// fixes a rare issue were the game wouldn't render anything before the mainmenu finished loading
		// See issue #323
		Patch(0x167899, { 0xEB }).Apply();
	}

	void SetUi(std::shared_ptr<LoadingScreenUi> ui)
	{
		ActiveUi = ui;
	}
}

namespace
{
	void ShowLoadingScreenHook(const char *mapPath, short unk)
	{
		typedef void(*ShowLoadingScreenPtr)(const char *mapPath, short unk);
		auto ShowLoadingScreen = reinterpret_cast<ShowLoadingScreenPtr>(0x52EE40);
		ShowLoadingScreen(mapPath, unk);

		if (ActiveUi)
			ActiveUi->Show(mapPath);
	}

	void BeginLoadingHook(uint32_t totalBytes)
	{
		if (ActiveUi)
			ActiveUi->Begin(totalBytes);
	}

	void UpdateLoadingProgressHook(uint32_t bytes)
	{
		if (ActiveUi)
			ActiveUi->UpdateProgress(bytes);
	}

	void HideLoadingScreenHook()
	{
		typedef void(*HideLoadingScreenPtr)();
		auto HideLoadingScreen = reinterpret_cast<HideLoadingScreenPtr>(0x52EE20);
		HideLoadingScreen();

		if (ActiveUi)
			ActiveUi->Hide();
	}
}