#pragma once
#include "Hf2pExperimental.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Modules/ModuleSettings.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../ElDorito.hpp"
#include "../Patches/Ui.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Armor.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Web/Ui/WebTimer.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTime.hpp"

namespace
{
	void Hf2pInitHook();
	void Hf2pShutdownHook();
	void Hf2pTickHook();
	void Hf2pLoadPreferencesHook();

	void UI_StartMenuScreenWidget_OnDataItemSelectedHook();

	void OnMapLoaded(const char *mapPath);
}

namespace Patches
{
	void Hf2pExperimental::ApplyAll()
	{
		Hook(0x200630, Hf2pInitHook).Apply();
		// we no longer have sound_config.ps
		Patch(0x04858, { 0x90, 0x90 }).Apply();
		// skip over vfiles_plugin load
		Patch(0x010F1121, { 0xE9 }).Apply();
		// skip anti-cheat, watermark, account stuff
		Patch(0x200732, { 0xEB }).Apply();

		Hook(0x200790, Hf2pShutdownHook).Apply();
		Hook(0x105F0D, Hf2pTickHook, HookFlags::IsCall).Apply();
		Hook(0x10CB01, Hf2pLoadPreferencesHook, HookFlags::IsCall).Apply();

		Hook(0x6F740E, UI_StartMenuScreenWidget_OnDataItemSelectedHook).Apply();

		Patches::Core::OnMapLoaded(OnMapLoaded);
	}
}

namespace
{
	using namespace Blam::Input;

	const auto UI_ScreenWidget_Close = (void(__thiscall *)(void *thisptr, int a2))(0x00AB2830);
	const auto UI_ScreenManager_GetScreenWidget = (void* (__thiscall *)(void *thisptr, int a2, uint32_t nameId))(0x00AAB550);
	const auto UI_ScreenManager_AnyActiveScreens = (bool(__thiscall *)(void *thisptr, int a2))(0x00AAA970);
	const auto UI_GetScreenManager = (void*(*)())(0x00AAD930);

	void Hf2pInitHook()
	{
		*(uint32_t*)0x50CCB3C = 0;
		*(uint32_t*)0x244ED28 = 0;
		// InitSoundSystem
		((void(*)())(0x64E190))();
	}

	void Hf2pShutdownHook()
	{
		// ShutdownSoundSystem
		((void(*)())(0x652EE0))();
	}

	void SystemMenu()
	{
		static auto IsMainMenu = (bool(*)())(0x531E90);

		if (IsMainMenu())
			return;

		auto uiStartAction = Blam::Input::GetActionState(eGameActionUiStart);
		if (!(uiStartAction->Flags & eActionStateFlagsHandled) && uiStartAction->Ticks == 1)
		{
			auto screenManager = (void*)0x05260F34;
			if (!UI_ScreenManager_AnyActiveScreens(screenManager, 0))
			{
				uiStartAction->Flags |= eActionStateFlagsHandled;
				Patches::Ui::ShowDialog(0x10084, 0, 4, 0x1000C);
			}
		}
	}

	int GetSecondsRemainingUntilPlayerSpawn()
	{
		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		const auto player = Blam::Players::GetPlayers().Get(Blam::Players::GetLocalPlayer(0));
		return player ? Pointer(player)(0x2CBC).Read<int32_t>() : 0;
	}

	void Hf2pTickHook()
	{
		static auto InPrematchState = (bool(*)(char state))(0x005523A0);
		static auto UpdatePreMatchCamera = (bool(*)())(0x72D580);
		static auto InitMpDirector = (void(*)())(0x0072D560);
		static auto s_MatchStarted = false;
		static auto s_TimerStarted = false;
		static auto s_TimerLastTicked = 0;

		auto secondsUntilPlayerSpawn = GetSecondsRemainingUntilPlayerSpawn();

		// update pre-match camera
		if (InPrematchState(4))
		{
			if (!s_TimerStarted && secondsUntilPlayerSpawn > 0)
			{
				s_TimerStarted = true;
				s_TimerLastTicked = Blam::Time::GetGameTicks();
				Web::Ui::WebTimer::Start("startTimer", secondsUntilPlayerSpawn);
			}

			s_MatchStarted = UpdatePreMatchCamera();
		}
		else if (s_MatchStarted)
		{
			InitMpDirector();
			s_MatchStarted = false;
		}
		else
		{
			if (!s_TimerStarted && secondsUntilPlayerSpawn > 0)
			{
				s_TimerStarted = true;
				s_TimerLastTicked = Blam::Time::GetGameTicks();
				Web::Ui::WebTimer::Start("respawnTimer", secondsUntilPlayerSpawn);
			}
		}

		if (s_TimerStarted)
		{
			static auto s_LastTimerValue = 0;
			auto secondsUntilSpawn = GetSecondsRemainingUntilPlayerSpawn();

			if (secondsUntilSpawn != s_LastTimerValue)
			{
				s_LastTimerValue = secondsUntilSpawn;
				s_TimerLastTicked = Blam::Time::GetGameTicks();
				Web::Ui::WebTimer::Update(secondsUntilSpawn);

				if (secondsUntilSpawn <= 0)
				{
					s_TimerStarted = false;
					Web::Ui::WebTimer::End();
				}
			}
		}

		static auto IsMainMenu = (bool(*)())(0x00531E90);

		if (IsMainMenu())
		{
			// armour customizations on mainmenu
			Patches::Armor::UpdateUiPlayerModelArmor();
		}
		else
		{
			// pause menu
			SystemMenu();
		}
	}


	void CloseScreen(void* screenManager, uint32_t id)
	{
		const auto screen = UI_ScreenManager_GetScreenWidget(screenManager, 4, id);
		if (screen)
			UI_ScreenWidget_Close(screen, 0);
	}

	bool StartMenu_OnDataItemSelected(uint32_t nameId)
	{
		switch (nameId)
		{
		case 0x4055: // control_settings
			CloseScreen(UI_GetScreenManager(), 0x10084);
			Web::Ui::ScreenLayer::Show("settings", "{}");
			return true;
		}
		return false;
	}

	__declspec(naked) void UI_StartMenuScreenWidget_OnDataItemSelectedHook()
	{
		__asm
		{
			push[ebp - 4]
			call StartMenu_OnDataItemSelected
			test al, al
			jz DEFAULT
			add esp, 4
			xor eax, eax
			pop ebx
			mov esp, ebp
			pop ebp
			retn 0x10
			DEFAULT:
			mov eax, 0xAF7436
			jmp eax
		}
	}

	uint32_t LevelStringToInt(const std::string& value)
	{
		return value == "low" ? 0 : value == "medium" ? 1 : 2;
	}

	void Hf2pLoadPreferencesHook()
	{
		static auto Hf2pLoadPreferences = (void(*)())(0x50C830);
		Hf2pLoadPreferences();

		auto preferencesPtr = ElDorito::GetMainTls(0x18)[0];
		if (!preferencesPtr)
			return;

		auto& moduleSettings = Modules::ModuleSettings::Instance();

		int screenResolutionWidth, screenResolutionHeight;
		moduleSettings.GetScreenResolution(&screenResolutionWidth, &screenResolutionHeight);

		// a step towards eliminating the need preferences.dat at least for settings
		preferencesPtr(0x41BCC).Write<uint32_t>(moduleSettings.VarFullscreen->ValueInt);
		preferencesPtr(0x41C50).Write<uint32_t>(moduleSettings.VarMasterVolume->ValueInt);
		preferencesPtr(0x41C58).Write<uint32_t>(moduleSettings.VarMusicVolume->ValueInt);
		preferencesPtr(0x41C54).Write<uint32_t>(moduleSettings.VarSfxVolume->ValueInt);
		preferencesPtr(0x41BE0).Write<uint32_t>(screenResolutionWidth);
		preferencesPtr(0x41BE4).Write<uint32_t>(screenResolutionHeight);
	}

	void OnMapLoaded(const char* map)
	{
		const auto soundSystemPtr = (uint8_t**)(0x018BC9C8);
		if (!soundSystemPtr)
			return;

		const auto& mapStr = std::string(map);
		const std::string mainmenu("mainmenu");

		if (mapStr.length() >= mainmenu.length() && std::equal(mainmenu.rbegin(), mainmenu.rend(), mapStr.rbegin()))
		{
			// play main menu music from wigl
			static auto PlayMainMenuMusic = (void(__thiscall *)(void *thisptr))(0xAD5820);
			PlayMainMenuMusic((void*)0x5255A0C);

			// mute ambient sounds on the maim menu
			*(float*)((*soundSystemPtr) + 0x44) = 0.0f;
		}
		else
		{
			*(float*)((*soundSystemPtr) + 0x44) = 1.0f;
		}
	}
}
