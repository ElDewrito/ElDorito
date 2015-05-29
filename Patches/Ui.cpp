#include "Ui.h"

#include "../ElDorito.h"
#include "../ElMacros.h"
#include "../Patch.h"
#include "../BlamTypes.h"

namespace
{
	void __fastcall UI_MenuUpdateHook(void* a1, int unused, int menuIdToLoad);
	int UI_ShowHalo3StartMenu(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	char __fastcall UI_Forge_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct);
	char __fastcall UI_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct);
	void LocalizedStringHook();
	void LobbyMenuButtonHandlerHook();

	Patch CenteredCrosshairPatch(0x25FA43, { 0x31, 0xC0, 0x90, 0x90 });
}

namespace Patches
{
	namespace Ui
	{
		bool DialogShow; // todo: put this somewhere better
		unsigned int DialogStringId;
		int DialogArg1; // todo: figure out a better name for this
		int DialogFlags;
		unsigned int DialogParentStringId;
		uint8_t UIData[0x40];

		void Tick()
		{
			if (DialogShow)
			{
				typedef void*(__thiscall * OpenUIDialogByIdFunc)(void* a1, unsigned int dialogStringId, int a3, int dialogFlags, unsigned int parentDialogStringId);

				// fill UIData with proper data
				OpenUIDialogByIdFunc openui = (OpenUIDialogByIdFunc)0xA92780;
				openui(&UIData, DialogStringId, DialogArg1, DialogFlags, DialogParentStringId);

				// send UI notification
				uint32_t eax = (uint32_t)&UIData;
				uint32_t ecx = *(uint32_t*)0x5260254;
				*(DWORD*)(ecx + 8) = eax;

				eax = *(uint32_t*)0x5260254;
				eax = *(uint32_t*)eax;
				*(uint32_t*)0x5260254 = eax;

				DialogShow = false;
			}
		}

		void EnableCenteredCrosshairPatch(bool enable)
		{
			if (enable)
				CenteredCrosshairPatch.Apply();
			else
				CenteredCrosshairPatch.Reset();
		}

		void ApplyAll()
		{
			// English patch
			Patch(0x2333FD, { 0 }).Apply();

			// Update window title patch
			const uint8_t windowData[] = { 0x3A, 0x20, 0x45, 0x6C, 0x20, 0x44, 0x6F, 0x72, 0x69, 0x74, 0x6F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00 };
			Pointer::Base(0x159C02F).Write(windowData, sizeof(windowData));
			Pointer::Base(0x159C06F).Write(windowData, sizeof(windowData));
			Pointer::Base(0x1EAAEE0).Write(windowData, sizeof(windowData));

			// Fix for leave game button to show H3 pause menu
			Hook(0x3B6826, &UI_ShowHalo3StartMenu, HookFlags::IsCall).Apply();
			Patch::NopFill(Pointer::Base(0x3B6826 + 5), 1);

			// Fix "Network" setting in lobbies (change broken 0x100B7 menuID to 0x100B6)
			Patch(0x6C34B0, { 0xB6 }).Apply();

			// Fix gamepad option in settings (todo: find out why it doesn't detect gamepads
			// and find a way to at least enable pressing ESC when gamepad is enabled)
			Patch::NopFill(Pointer::Base(0x20D7F2), 2);

			// Fix menu update code to include missing mainmenu code
			Hook(0x6DFB73, &UI_MenuUpdateHook, HookFlags::IsCall).Apply();

			// Hacky fix to stop the game crashing when you move selection on UI
			// (todo: find out what's really causing this)
			Patch::NopFill(Pointer::Base(0x569D07), 3);

			// Sorta hacky way of getting game options screen to show when you press X on lobby
			TODO("find real way of showing the [X] Edit Game Options text, that might enable it to work without patching");
			Hook(0x721B8A, LobbyMenuButtonHandlerHook, HookFlags::IsJmpIfEqual).Apply();

			// Hook UI vftable's forge menu button handler, so arrow keys can act as bumpers
			// added side effect: analog stick left/right can also navigate through menus
			DWORD temp;
			DWORD temp2;
			VirtualProtect(Pointer(0x169EFD8), 4, PAGE_READWRITE, &temp);
			Pointer(0x169EFD8).Write<uint32_t>((uint32_t)&UI_Forge_ButtonPressHandlerHook);
			VirtualProtect(Pointer(0x169EFD8), 4, temp, &temp2);

			// Hook pause menu vftable button handler, to let us limit the button presses
			VirtualProtect(Pointer(0x16A0148), 4, PAGE_READWRITE, &temp);
			Pointer(0x16A0148).Write<uint32_t>((uint32_t)&UI_ButtonPressHandlerHook);
			VirtualProtect(Pointer(0x16A0148), 4, temp, &temp2);

			// Remove Xbox Live from the network menu
			Patch::NopFill(Pointer::Base(0x723D85), 0x17);
			Pointer::Base(0x723DA1).Write<uint8_t>(0);
			Pointer::Base(0x723DB8).Write<uint8_t>(1);
			Patch::NopFill(Pointer::Base(0x723DFF), 0x3);
			Pointer::Base(0x723E07).Write<uint8_t>(0);
			Pointer::Base(0x723E1C).Write<uint8_t>(0);

			// Localized string override hook
			Hook(0x11E040, LocalizedStringHook).Apply();

			// Remove "BUILT IN" text when choosing map/game variants by feeding the UI_SetVisiblityOfElement func a nonexistant string ID for the element (0x202E8 instead of 0x102E8)
			TODO("find way to fix this text instead of removing it, since the 0x102E8 element (subitem_edit) is used for other things like editing/viewing map variant metadata");
			Patch(0x705D6F, { 0x2 }).Apply();
		}

		void ApplyMapNameFixes()
		{
			uint32_t levelsGlobalPtr = Pointer::Base(0x149E2E0).Read<uint32_t>();
			if (!levelsGlobalPtr)
				return;

			TODO("map out these global arrays, content items seems to use same format");

			uint32_t numLevels = Pointer(levelsGlobalPtr + 0x34).Read<uint32_t>();
			if (numLevels != 6)
				return;

			const wchar_t* search[6] = { L"guardian", L"riverworld", L"s3d_avalanche", L"s3d_edge", L"s3d_reactor", L"s3d_turf" };
			const wchar_t* names[6] = { L"Guardian", L"Valhalla", L"Diamondback", L"Edge", L"Reactor", L"Icebox" };
			TODO("Get names/descs using string ids? Seems the unic tags have descs for most of the maps");
			const wchar_t* descs[6] = {
				L"Millennia of tending has produced trees as ancient as the Forerunner structures they have grown around. 2-6 players.",
				L"The crew of V-398 barely survived their unplanned landing in this gorge...this curious gorge. 6-16 players.",
				L"Hot winds blow over what should be a dead moon. A reminder of the power Forerunners once wielded. 6-16 players.",
				L"The remote frontier world of Partition has provided this ancient databank with the safety of seclusion. 6-16 players.",
				L"Being constructed just prior to the Invasion, its builders had to evacuate before it was completed. 6-16 players.",
				L"Though they dominate on open terrain, many Scarabs have fallen victim to the narrow streets of Earth's cities. 4-10 players."
			};
			for (uint32_t i = 0; i < numLevels; i++)
			{
				Pointer levelNamePtr = Pointer(levelsGlobalPtr + 0x54 + (0x360 * i) + 0x8);
				Pointer levelDescPtr = Pointer(levelsGlobalPtr + 0x54 + (0x360 * i) + 0x8 + 0x40);

				wchar_t levelName[0x20];
				memset(levelName, 0, sizeof(wchar_t) * 0x20);
				levelNamePtr.Read(levelName, sizeof(wchar_t) * 0x20);

				for (uint32_t y = 0; y < 6; y++)
				{
					if (wcscmp(search[y], levelName) == 0)
					{
						memset(levelNamePtr, 0, sizeof(wchar_t) * 0x20);
						wcscpy_s(levelNamePtr, 0x20, names[y]);

						memset(levelDescPtr, 0, sizeof(wchar_t) * 0x80);
						wcscpy_s(levelDescPtr, 0x80, descs[y]);
						break;
					}
				}
			}
		}
	}
}

namespace
{
	void __fastcall UI_MenuUpdateHook(void* a1, int unused, int menuIdToLoad)
	{
		bool shouldUpdate = *(DWORD*)((uint8_t*)a1 + 0x10) >= 0x1E;
		typedef void(__thiscall *UI_MenuUpdateFunc)(void* a1, int menuIdToLoad);
		UI_MenuUpdateFunc menuUpdate = (UI_MenuUpdateFunc)0xADF6E0;
		menuUpdate(a1, menuIdToLoad);

		if (shouldUpdate)
		{
			Patches::Ui::DialogStringId = menuIdToLoad;
			Patches::Ui::DialogArg1 = 0xFF;
			Patches::Ui::DialogFlags = 4;
			Patches::Ui::DialogParentStringId = 0x1000D;
			Patches::Ui::DialogShow = true;
		}
	}

	int UI_ShowHalo3StartMenu(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		Patches::Ui::DialogStringId = 0x10084;
		Patches::Ui::DialogArg1 = 0;
		Patches::Ui::DialogFlags = 4;
		Patches::Ui::DialogParentStringId = 0x1000C;
		Patches::Ui::DialogShow = true;

		return 1;
	}

	std::chrono::high_resolution_clock::time_point PrevTime = std::chrono::high_resolution_clock::now();
	char __fastcall UI_Forge_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct)
	{
		uint32_t btnCode = *(uint32_t*)(controllerStruct + 0x1C);

		auto CurTime = std::chrono::high_resolution_clock::now();
		auto timeSinceLastAction = std::chrono::duration_cast<std::chrono::milliseconds>(CurTime - PrevTime);

		if (btnCode == Blam::ButtonCodes::eButtonCodesLeft || btnCode == Blam::ButtonCodes::eButtonCodesRight)
		{
			if (timeSinceLastAction.count() < 200) // 200ms between button presses otherwise it spams the key
				return 1;

			PrevTime = CurTime;

			if (btnCode == Blam::ButtonCodes::eButtonCodesLeft) // analog left / arrow key left
				*(uint32_t*)(controllerStruct + 0x1C) = Blam::ButtonCodes::eButtonCodesLB;

			if (btnCode == Blam::ButtonCodes::eButtonCodesRight) // analog right / arrow key right
				*(uint32_t*)(controllerStruct + 0x1C) = Blam::ButtonCodes::eButtonCodesRB;
		}

		typedef char(__thiscall *UI_Forge_ButtonPressHandler)(void* a1, void* controllerStruct);
		UI_Forge_ButtonPressHandler buttonHandler = (UI_Forge_ButtonPressHandler)0xAE2180;
		return buttonHandler(a1, controllerStruct);
	}

	char __fastcall UI_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct)
	{
		uint32_t btnCode = *(uint32_t*)(controllerStruct + 0x1C);

		auto CurTime = std::chrono::high_resolution_clock::now();
		auto timeSinceLastAction = std::chrono::duration_cast<std::chrono::milliseconds>(CurTime - PrevTime);

		if (btnCode >= Blam::ButtonCodes::eButtonCodesLeft && btnCode <= Blam::ButtonCodes::eButtonCodesDown) // btnCode = 0x10 (sent on all arrow key presses) or 0x12 (arrow left) 0x13 (arrow right) 0x14 (arrow up) 0x15 (arrow down)
		{
			if (timeSinceLastAction.count() < 200) // 200ms between button presses otherwise it spams the key
				return 1;

			PrevTime = CurTime;
		}

		typedef char(__thiscall *UI_ButtonPressHandler)(void* a1, void* controllerStruct);
		UI_ButtonPressHandler buttonHandler = (UI_ButtonPressHandler)0xAB1BA0;
		return buttonHandler(a1, controllerStruct);
	}

	bool LocalizedStringHookImpl(int tagIndex, int stringId, wchar_t *outputBuffer)
	{
		const size_t MaxStringLength = 0x400;

		switch (stringId)
		{
		case 0x1010A: // start_new_campaign
		{
			// Get the version string, convert it to uppercase UTF-16, and return it
			std::string version = Utils::Version::GetVersionString();
			std::transform(version.begin(), version.end(), version.begin(), toupper);
			std::wstring unicodeVersion(version.begin(), version.end());
			swprintf(outputBuffer, MaxStringLength, L"ELDEWRITO %s", unicodeVersion.c_str());
			return true;
		}
		}
		return false;
	}

	__declspec(naked) void LocalizedStringHook()
	{
		__asm
		{
			// Run the hook implementation function and fallback to the original if it returned false
			push ebp
			mov ebp, esp
			push[ebp + 0x10]
			push[ebp + 0xC]
			push[ebp + 0x8]
			call LocalizedStringHookImpl
			add esp, 0xC
			test al, al
			jz fallback

			// Don't run the original function
			mov esp, ebp
			pop ebp
			ret

		fallback:
			// Execute replaced code and jump back to original function
			sub esp, 0x800
			mov edx, 0x51E049
			jmp edx
		}
	}

	__declspec(naked) void LobbyMenuButtonHandlerHook()
	{
		__asm
		{
			// call sub that handles showing game options
			mov ecx, esi
			push [edi+0x10]
			mov eax, 0xB225B0
			call eax
			// jump back to original function
			mov eax, 0xB21B9F
			jmp eax
		}
	}
}