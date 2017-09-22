#include "LoadingScreen.hpp"
#include "../Patch.hpp"
#include "../Modules/ModuleBase.hpp"

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
		typedef void(*BeginLoadingPtr)(uint32_t totalBytes);
		auto BeginLoading = reinterpret_cast<BeginLoadingPtr>(0x711C00);
		BeginLoading(totalBytes);

		if (ActiveUi)
			ActiveUi->Begin(totalBytes);
	}

	void UpdateLoadingProgressHook(uint32_t bytes)
	{
		typedef void(*UpdateLoadingProgressPtr)(uint32_t bytes);
		auto UpdateLoadingProgress = reinterpret_cast<UpdateLoadingProgressPtr>(0x711C30);
		UpdateLoadingProgress(bytes);

		if (ActiveUi)
			ActiveUi->UpdateProgress(bytes);
	}

	void HideLoadingScreenHook()
	{
		typedef void(*HideLoadingScreenPtr)();
		auto HideLoadingScreen = reinterpret_cast<HideLoadingScreenPtr>(0x52EE20);
		HideLoadingScreen();

		Modules::CommandMap::Instance().ExecuteCommand("game.hideh3ui 0");

		if (ActiveUi)
			ActiveUi->Hide();
	}
}