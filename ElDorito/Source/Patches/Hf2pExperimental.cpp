#pragma once
#include "Hf2pExperimental.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Modules/ModuleSettings.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModuleCamera.hpp"
#include "../Modules/ModuleInput.hpp"
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
	struct Preferences
	{
		uint8_t Unknown00;
		uint8_t Unknown01;
		uint8_t Unknown[0x1E];
		uint8_t Unknown20[0x78];
		uint8_t Unknown98[0x78];
		uint32_t Unknown110;
		uint32_t Unknown114;
		uint8_t Unknown118[0x80];
		uint8_t Unknown198[0x198]; // campaign?
		uint32_t Unknown330;
		uint32_t Unknown334;
		uint8_t Unknown338[0xE718]; // game varaint at 0x33c
		uint8_t UnknownEA50[0xE2A8]; // map variant at 0xEA58
		uint8_t Unknown1CCF8[0x24E80];
		uint8_t Unknown41B78[0x4C];
		uint32_t Unknown41BC0;
		uint8_t Unknown41BC5;
		uint8_t Unknown41BC6;
		uint8_t Unknown41BC7;
		uint8_t Unknown41BC8;
		uint8_t Fullscreen;
		uint32_t Unknown41BD0;
		uint32_t Contrast;
		uint32_t Brightness;
		uint32_t Unknown41BDC;
		uint32_t ScreenResolutionWidth;
		uint32_t ScreenResolutionHeight;
		uint32_t Unknown41BE8;
		uint32_t Unknown41BEC;
		uint32_t TextureResolution;
		uint32_t ShadowQuality;
		uint32_t TextureFilteringQuality;
		uint32_t LightingQuality;
		uint32_t EffectsQuality;
		uint32_t DetailsQuality;
		uint32_t PostprocessingQuality;
		uint8_t MotionBlur;
		uint8_t VSync;
		uint8_t Antialiasing;
		uint8_t Unknown41C0F;
		uint8_t ShowWatermark;
		uint32_t DisplaySubtitles;
		uint32_t DisplayAdapter;
		uint32_t Unknown41C1C;
		uint32_t Unknown41C20;
		uint32_t Unknown41C24;
		uint32_t Unknown41C28;
		uint32_t Unknown41C2C;
		uint32_t Unknown41C30;
		uint32_t Unknown41C34;
		uint32_t Unknown41C38;
		uint32_t Unknown41C3C;
		uint32_t Unknown41C40;
		uint32_t Unknown41C44;
		uint32_t Unknown41C48;
		uint32_t Unknown41C4C;
		uint32_t MasterVolume;
		uint32_t SfxVolume;
		uint32_t MusicVolume;
		uint32_t VoiceVolume;
		uint32_t VoiceChatControl;
		uint32_t VoiceChatVolume;
		uint8_t ToggleCrouch;
		uint8_t HUDShake;
		uint8_t Unknown41C6A;
		uint8_t Unknown41C6B;
		uint32_t TeamColor;
		float CameraFov;
		uint32_t ControlsMethod;
		uint32_t MouseSensitivityVertical;
		uint32_t MouseSensitivityHorizontal;
		uint32_t MouseSensitivityVehicleVertical;
		uint32_t MouseSensitivityVehicleHorizontal;
		uint32_t MouseAcceleration;
		uint8_t MouseFilter;
		uint8_t InvertMouse;
		uint8_t Unknown41C8E;
		uint8_t Unknown41C8F;
		uint32_t Unknown41C90;
		uint8_t Bindings[0x17c];
		uint8_t Unknown41E10[0x1F0];
	};

	static_assert(sizeof(Preferences) == 0x42000, "Invalid Preferences size");

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
		// prevent display/graphics settings from resetting when preferences.dat doesn't exist
		Patch(0x622920, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3 }).Apply();

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
		static auto IsMapLoading = (bool(*)())(0x005670E0);
		static auto IsMainMenu = (bool(*)())(0x00531E90);
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

			// TODO: find a better game ended indication
			if (IsMapLoading())
			{
				s_TimerStarted = false;
				Web::Ui::WebTimer::End();
			}
		}

		if (!IsMapLoading())
		{
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
		// TODO: look into some of these functions further
		static auto sub_50A190 = (void(__thiscall *)(void* thisptr))(0x50A190);
		static auto sub_65C990 = (DWORD *(__thiscall*)(void* thisptr, int a2))(0x65C990);
		static auto sub_50AD30 = (void(__cdecl*)(void* a1, void* a2))(0x50AD30);
		static auto sub_50AC70 = (void(__cdecl*)(void* a1))(0x50AC70);
		static auto sub_65CA30 = (void(__thiscall*)(void *thisptr))(0x65CA30);

		const auto& moduleSettings = Modules::ModuleSettings::Instance();
		const auto& moduleCamera = Modules::ModuleCamera::Instance();
		const auto& moduleInput = Modules::ModuleInput::Instance();

		Preferences* preferences = ElDorito::GetMainTls(0x18)[0];
		if (!preferences)
			return;

		memset(preferences, 0, 0x42000u);
		*(DWORD *)(preferences->Unknown20) = 0x33;
		*(DWORD *)(preferences->Unknown20 + 0x24) = -1;
		*(DWORD *)(preferences->Unknown20 + 0x28) = -1;
		*(DWORD *)(preferences->Unknown20 + 0x34) = 0;
		preferences->Unknown41BC0 = 1;
		preferences->DisplaySubtitles = 0;
		preferences->Unknown41BC8 = 0;

		uint8_t osInfo[0x34];
		sub_65C990(osInfo, preferences->Unknown41BC8);

		preferences->Unknown41BDC = 4;
		sub_50AD30(osInfo, &preferences->Unknown41BDC); // display preferences

		preferences->Unknown41BD0 = 0;
		preferences->Unknown41C6A = 1;

		sub_50AC70(&preferences->ControlsMethod);

		int screenResolutionWidth, screenResolutionHeight;
		moduleSettings.GetScreenResolution(&screenResolutionWidth, &screenResolutionHeight);

		preferences->Fullscreen = uint8_t(moduleSettings.VarFullscreen->ValueInt);
		preferences->Contrast = moduleSettings.VarContrast->ValueInt;
		preferences->Brightness = moduleSettings.VarBrightness->ValueInt;
		preferences->ScreenResolutionWidth = screenResolutionWidth;
		preferences->ScreenResolutionHeight = screenResolutionHeight;
		preferences->TextureResolution = LevelStringToInt(moduleSettings.VarTextureResolution->ValueString);
		preferences->ShadowQuality = LevelStringToInt(moduleSettings.VarShadowQuality->ValueString);
		preferences->TextureFilteringQuality = LevelStringToInt(moduleSettings.VarTextureFilteringQuality->ValueString);
		preferences->LightingQuality = LevelStringToInt(moduleSettings.VarLightingQuality->ValueString);
		preferences->EffectsQuality = LevelStringToInt(moduleSettings.VarEffectsQuality->ValueString);
		preferences->DetailsQuality = LevelStringToInt(moduleSettings.VarDetailsQuality->ValueString);
		preferences->PostprocessingQuality = LevelStringToInt(moduleSettings.VarPostprocessingQuality->ValueString);
		preferences->MotionBlur = uint8_t(moduleSettings.VarMotionBlur->ValueInt);
		preferences->VSync = uint8_t(moduleSettings.VarVSync->ValueInt);
		preferences->Antialiasing = uint8_t(moduleSettings.VarAntialiasing->ValueInt);
		preferences->ShowWatermark = uint8_t(moduleSettings.VarShadowQuality->ValueInt);
		preferences->MasterVolume = moduleSettings.VarMasterVolume->ValueInt;
		preferences->SfxVolume = moduleSettings.VarSfxVolume->ValueInt;
		preferences->MusicVolume = moduleSettings.VarMusicVolume->ValueInt;
		preferences->ToggleCrouch = uint8_t(moduleSettings.VarToggleCrouch->ValueInt);
		preferences->HUDShake = uint8_t(moduleSettings.VarHUDShake->ValueInt);
		preferences->TeamColor = moduleSettings.VarPlayerMarkerColors->ValueInt;
		preferences->CameraFov = moduleCamera.VarCameraFov->ValueFloat;
		preferences->ControlsMethod = moduleSettings.VarGamepadEnabled->ValueInt;
		preferences->MouseSensitivityHorizontal = moduleSettings.VarMouseSensitivityHorizontal->ValueInt;
		preferences->MouseSensitivityVertical = moduleSettings.VarMouseSensitivityVertical->ValueInt;
		preferences->MouseSensitivityVehicleHorizontal = moduleSettings.VarMouseSensitivityVehicleHorizontal->ValueInt;
		preferences->MouseSensitivityVehicleVertical = moduleSettings.VarMouseSensitivityVehicleVertical->ValueInt;
		preferences->MouseAcceleration = moduleSettings.VarMouseAcceleration->ValueInt;
		preferences->MouseFilter = uint8_t(moduleSettings.VarMouseFilter->ValueInt);
		preferences->InvertMouse = uint8_t(moduleSettings.VarInvertMouse->ValueInt);

		memmove(ElDorito::GetMainTls(0x18)[0](0x42020), &preferences->Unknown00 + 0x20, 0x41DF0u);

		sub_65CA30(osInfo); // osInfo dtor

		preferences->Unknown00 = 1;
		preferences->Unknown01 = 1;
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
