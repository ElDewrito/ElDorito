#pragma once
#include "Hf2pExperimental.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Modules/ModuleSettings.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModuleCamera.hpp"
#include "../Modules/ModuleInput.hpp"
#include "../ElDorito.hpp"
#include "../Patches/Ui.hpp"
#include "../Patches/Core.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Web/Ui/WebTimer.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Web/Ui/WebScoreboard.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Game/Armor.hpp"
#include "../Forge/PrematchCamera.hpp"
#include "../Forge/ForgeUtil.hpp"

namespace
{
	struct Preferences
	{
		byte Unknown00;
		byte IsDirty;
		word Unknown2;
		dword Unknown4;
		dword Unknown8;
		dword UnknownC;
		dword Unknown10;
		dword Unknown14;
		dword Unknown18;
		dword Unknown1C;
		dword Unknown20;
		dword Language;
		dword Unknown28;
		dword Unknown2C[2];
		dword MinorVersion;
		dword Unknown38;
		dword Unknown3C;
		dword Unknown40;
		dword Unknown44;
		dword Unknown48;
		dword Unknown4C;
		byte Unknown50[0x4C];
		byte Unknown98[0x78];
		dword Unknown110;
		dword Unknown114;
		byte Unknown118[0x80];
		byte Unknown19C[0x44];
		byte Unknown1E0;
		dword Unknown1E4;
		dword Unknown1E8;
		byte Unknown1EC[0x148];
		dword Unknown330;
		byte Unknown334[0x40];
		dword Unknown398;
		dword Unknown378;
		byte Unknown380;
		byte Unknown381[1135];
		dword Unknown7F0;
		byte Unknown7F4[0xFC];
		dword Unknown8F0;
		byte Unknown8F4[0xE19C];
		dword UnknownEA90;
		dword UnknownEA94;
		dword UnknownEA98;
		byte UnknownEA9C[0xFC];
		dword UnknownEB98;
		byte UnknownEB9C[0xE19C];
		dword Unknown1CD38;
		dword Unknown1CD3C;
		dword Unknown1CD40;
		dword Unknown1CD44;
		dword Unknown1CD48;
		dword Unknown1CD4C;
		dword Unknown1CD50;
		dword Unknown1CD54;
		byte Unknown1CD58[0x1A55D];
		dword Unknown372B8;
		dword DisplaySubtitles;
		dword Fullscreen;
		dword Unknown372C4;
		dword Contrast;
		dword Brightness;
		dword Unknown372D0;
		dword ScreenResolutionWidth;
		dword ScreenResolutionHeight;
		dword TextureResolution;
		dword ShadowQuality;
		dword TextureFilteringQuality;
		dword LightingQuality;
		dword EffectsQuality;
		dword DetailsQuality;
		dword PostprocessingQuality;
		byte MotionBlur;
		byte VSync;
		byte Antialiasing;
		byte Unknown372FB;
		byte HideWatermark;
		byte Unknown372D[3];
		dword Unknown37300;
		byte Unknown37304[0x38];
		dword MasterVolume;
		dword SfxVolume;
		dword MusicVolume;
		dword VoiceVolume;
		dword VoiceChatControl;
		dword VoiceChatVolume;
		byte ToggleCrouch;
		byte HUDShake;
		byte Unknown37356;
		byte Unknown37357;
		dword TeamColor;
		real CameraFov;
		byte Unknown37360;
		byte Unknown37361;
		byte Unknown37362;
		byte Unknown37363;
		dword ControlsMethod;
		dword MouseSensitivityVertical;
		dword MouseSensitivityHorizontal;
		dword MouseSensitivityVehicleVertical;
		dword MouseSensitivityVehicleHorizontal;
		dword MouseAcceleration;
		byte MouseFilter;
		byte InvertMouse;
		word Unknown3737E;
		dword Unknown37380;
		byte Bindings[0x208];
		byte Unknown3758C[0xA72];
	};
	static_assert(sizeof(Preferences) == 0x38000, "Invalid Preferences size");

	void Hf2pInitHook();
	void Hf2pShutdownHook();
	void Hf2pTickHook();
	void Hf2pLoadPreferencesHook();
	bool SpawnTimerDisplayHook(int hudIndex, wchar_t* buff, int len, int a4);
	void game_engine_tick_hook();
	int game_engine_get_delta_ticks_hook();
	void __fastcall c_game_engine_object_runtime_manager__update_hook(void *thisptr, void *unused);

	void UI_StartMenuScreenWidget_OnDataItemSelectedHook();

	void OnMapLoaded(const char *mapPath);

	void SpawnTimerUpdate();
}

namespace Patches::Hf2pExperimental
{
	void ApplyAll()
	{
		Hook(0x200630, Hf2pInitHook).Apply();
		// we no longer have game.cfg or sound_config.ps
		Patch(0x24F6B0, { 0xC3 }).Apply();
		Patch(0x24EF50, { 0xC3 }).Apply();
		// skip over vfiles_plugin load
		Patch(0x010F1121, { 0xE9 }).Apply();
		// skip config, anti-cheat, watermark, account stuff
		Patch(0x2006F0, { 0xC3 }).Apply();
		// prevent display/graphics settings from resetting when preferences.dat doesn't exist
		Patch(0x622920, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3 }).Apply();

		Hook(0x200790, Hf2pShutdownHook).Apply();
		Hook(0x105F0D, Hf2pTickHook, HookFlags::IsCall).Apply();
		Hook(0x10CB01, Hf2pLoadPreferencesHook, HookFlags::IsCall).Apply();

		Hook(0x6F740E, UI_StartMenuScreenWidget_OnDataItemSelectedHook).Apply();

		Hook(0x6963C6, SpawnTimerDisplayHook, HookFlags::IsCall).Apply();
		// fixes race condition with client respawn timer
		Patch(0x1391B5, { 0xEB }).Apply();

		// NOTE: this is not a proper fix. The pre-match camera needs to be looked at
		Hook(0x1527F7, game_engine_tick_hook, HookFlags::IsCall).Apply();
		// infection
		Hook(0x5DD4BE, game_engine_get_delta_ticks_hook, HookFlags::IsCall).Apply();
		// runtime (weapon drop times)
		Hook(0x152783, c_game_engine_object_runtime_manager__update_hook, HookFlags::IsCall).Apply();

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
	const auto UI_PlaySound = (void(*)(int index, uint32_t uiseTagIndex))(0x00AA5CD0);

	const auto game_engine_in_state = (bool(*)(uint8_t state))(0x005523A0);

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
			auto isUsingController = *(bool*)0x0244DE98;
			auto screenManager = (void*)0x05260F34;
			const auto start_menu = 0x10084;

			if (!UI_ScreenManager_AnyActiveScreens(screenManager, 0) && !UI_ScreenManager_GetScreenWidget(screenManager, 4, 0x000100BE)) // in_progress
			{
				uiStartAction->Flags |= eActionStateFlagsHandled;
				UI_PlaySound(7, -1);
				Patches::Ui::ShowDialog(start_menu, 0, 4, 0x1000C);
			}
			else if (isUsingController)
			{
				auto screenWidget = UI_ScreenManager_GetScreenWidget(screenManager, 4, start_menu);
				if (screenWidget)
				{
					uiStartAction->Flags |= eActionStateFlagsHandled;
					UI_ScreenWidget_Close(screenWidget, 1);
				}
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
		const auto camera_update_scripted = (void(*)(int type, Blam::Math::RealVector3D *position,
			Blam::Math::RealVector3D *orientation, float a4, __int16 a5, uint32_t objectIndex))(0x0072DD70);
		const auto MatrixScale4x3_GetEulerAngles = (void(*)(Blam::Math::RealMatrix4x3 *matrix, Blam::Math::RealVector3D *outOrientation))(0x005B3670);
		const auto sub_592320 = (void(*)(int a1))(0x592320);

		static auto InPrematchState = (bool(*)(char state))(0x005523A0);
		static auto UpdatePreMatchCamera = (bool(*)())(0x72D580);
		static auto InitMpDirector = (void(*)())(0x0072D560);
		static auto IsMapLoading = (bool(*)())(0x005670E0);
		static auto IsMainMenu = (bool(*)())(0x00531E90);
		static auto s_MatchStarted = false;
		static auto s_TimerStarted = false;
		static auto s_TimerLastTicked = 0;
		static auto s_CameraObjectIndex = -1;

		auto secondsUntilPlayerSpawn = GetSecondsRemainingUntilPlayerSpawn();

		// update pre-match camera
		if (InPrematchState(4))
		{
			if (s_CameraObjectIndex == -1)
				s_CameraObjectIndex = Forge::PrematchCamera::FindCameraObject();

			if (s_CameraObjectIndex != -1)
			{
				Blam::Math::RealMatrix4x3 cameraObjectTransform;
				Blam::Math::RealVector3D cameraObjectOrientation;
				Forge::GetObjectTransformationMatrix(s_CameraObjectIndex, &cameraObjectTransform);
				MatrixScale4x3_GetEulerAngles(&cameraObjectTransform, &cameraObjectOrientation);


				sub_592320(1);
				camera_update_scripted(0, &cameraObjectTransform.Position, &cameraObjectOrientation, 0, 0, -1);
			}
			else
			{
				UpdatePreMatchCamera();
			}
			

			s_MatchStarted = true;
		}
		else if (s_MatchStarted)
		{
			InitMpDirector();
			s_MatchStarted = false;
			s_CameraObjectIndex = -1;
		}

		SpawnTimerUpdate();

		if (!IsMapLoading())
		{
			if (IsMainMenu())
			{
				// armour customizations on mainmenu
				Game::Armor::UpdateUiPlayerModelArmor();
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
			UI_PlaySound(3, -1); // A Button
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
		const auto& moduleSettings = Modules::ModuleSettings::Instance();
		const auto& moduleCamera = Modules::ModuleCamera::Instance();
		const auto& moduleInput = Modules::ModuleInput::Instance();

		const auto sub_50A520 = (void(*)())(0x0050A520);
		sub_50A520();

		Preferences* preferences = ElDorito::GetMainTls(0x18)[0];
		if (!preferences)
			return;

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
		preferences->HideWatermark = 1;
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

		preferences->Unknown00 = 1;
		preferences->IsDirty = 1;

		*(uint32_t*)ElDorito::GetMainTls(0x18)[0](0x8400C) = 1; // needed for dirtying
	}

	void ApplyDefaultScreenFx()
	{
		static const auto GetLocalPlayerUnitObjectIndex = (uint32_t(*)(int playerIndex))(0x00589CC0);
		static const auto SpawnScreenEffect = (void(*)(uint32_t tagIndex, uint32_t unitObjectIndex, int a3, void* a4, void* a5))(0x683060);

		auto scnrDefinitionPtr = (Blam::Tags::Scenario::Scenario**)0x022AAEB4;
		if (!scnrDefinitionPtr)
			return;

		auto scnrDefinition = *scnrDefinitionPtr;
		if (scnrDefinition->DefaultScreenFx.TagIndex == -1)
			return;

		auto unitObjectIndex = GetLocalPlayerUnitObjectIndex(*(uint32_t*)0x018BF52C);

		uint8_t unk[0x3C] = { 0 };
		SpawnScreenEffect(scnrDefinition->DefaultScreenFx.TagIndex, unitObjectIndex, 0, &unk, &unk);
	}

	void OnMapLoaded(const char* map)
	{
		const auto soundSystemPtr = (uint8_t**)(0x018BC9C8);
		if (!soundSystemPtr)
			return;

		const auto& mapStr = std::string(map);
		const std::string mainmenu("mainmenu");

		// this can be removed when we have scripts
		ApplyDefaultScreenFx();

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

	void SpawnTimerUpdate()
	{
		const auto ui_play_sound = (void(*)(int index, uint32_t uise))(0x00AA5CD0);
		const auto game_engine_round_in_progress = (bool(*)())(0x00550F90);

		static auto lastBeep = 0;

		Blam::Players::PlayerDatum *player{ nullptr };
		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == Blam::DatumHandle::Null || !(player = Blam::Players::GetPlayers().Get(playerIndex)))
			return;

		auto secondsUntilSpawn = Pointer(player)(0x2CBC).Read<int>();
		auto firstTimeSpawning = Pointer(player)(0x4).Read<uint32_t>() & 8;

		if(game_engine_round_in_progress() || player->SlaveUnit == Blam::DatumHandle::Null)
		{
			if (secondsUntilSpawn != lastBeep && secondsUntilSpawn < 3)
			{
				lastBeep = secondsUntilSpawn;
				ui_play_sound(13, -1);
			}
		}
	}

	bool SpawnTimerDisplayHook(int hudIndex, wchar_t* buff, int len, int a4)
	{
		const auto game_engine_round_in_progress = (bool(*)())(0x00550F90);
		const auto sub_6E4AA0 = (bool(__cdecl *)(int a1, wchar_t *DstBuf, int bufflen, char a4))(0x6E4AA0);
		if (sub_6E4AA0(hudIndex, buff, len, a4))
			return true;
		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == Blam::DatumHandle::Null)
			return false;
		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return false;

		auto secondsUntilSpawn = Pointer(player)(0x2CBC).Read<int>();
		if (player->SlaveUnit == Blam::DatumHandle::Null && secondsUntilSpawn > 0)
		{
			if (!game_engine_round_in_progress())
				return false;

			auto firstTimeSpawning = Pointer(player)(0x4).Read<uint32_t>() & 8;

			if(firstTimeSpawning)
				swprintf(buff, L"Spawn in %d", secondsUntilSpawn);
			else
				swprintf(buff, L"Respawning in %d", secondsUntilSpawn);

			return true;
		}

		return false;
	}


	void game_engine_tick_hook()
	{
		const auto game_get_current_engine = (void*(*)())(0x005CE150);

		auto engine = game_get_current_engine();
		if (!engine)
			return;

		if (!game_engine_in_state(4))
			(*(void(__thiscall **)(void*))(*(DWORD *)engine + 0x68))(engine);
	}

	int game_engine_get_delta_ticks_hook()
	{
		auto engineGobals = ElDorito::GetMainTls(0x48)[0];
		if (!engineGobals)
			return 0;

		const auto &moduleServer = Modules::ModuleServer::Instance();

		auto delta = (Blam::Time::GetGameTicks() - engineGobals(0xE10c).Read<uint32_t>()) - Blam::Time::SecondsToTicks(moduleServer.VarServerCountdown->ValueInt + 5.0f);
		if (delta < 0)
			delta = 0;

		return delta;
	}

	void __fastcall c_game_engine_object_runtime_manager__update_hook(void *thisptr, void *unused)
	{
		const auto c_game_engine_object_runtime_manager__update = (void(__thiscall*)(void *thisptr))(0x005913E0);

		if (!game_engine_in_state(4))
			c_game_engine_object_runtime_manager__update(thisptr);
	}
}
