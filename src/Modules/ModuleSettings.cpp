#include "ModuleSettings.hpp"

namespace
{
	auto SSL_GetCharValue = reinterpret_cast<char(__cdecl *)(int)>(0x5047F0);
	auto SSL_GetIntValue = reinterpret_cast<int(__cdecl *)(int)>(0x5042A0);

	auto SetAntialiasing = reinterpret_cast<int(__stdcall *)(int)>(0x79B240);
	auto SetAutoCentering = reinterpret_cast<int(__stdcall *)(bool)>(0x79B260);
	auto SetBrightness = reinterpret_cast<int(__stdcall *)(int)>(0x79B290);
	auto SetButtonLayout = reinterpret_cast<int(__stdcall *)(int)>(0x79B2B0);
	auto SetCameraFOV = reinterpret_cast<int(__stdcall *)(int)>(0x79B300);
	auto SetControlsMethod = reinterpret_cast<int(__stdcall *)(int)>(0x79B330);
	auto SetDetailsQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B350);
	auto SetDisplayHints = reinterpret_cast<int(__stdcall *)(bool)>(0x79B370);
	auto SetEffectsQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B390);
	auto SetFullscreen = reinterpret_cast<int(__stdcall *)(int)>(0x79B3B0);
	auto SetHUDShake = reinterpret_cast<int(__stdcall *)(bool)>(0x79B3D0);
	auto SetInvertLook = reinterpret_cast<int(__stdcall *)(bool)>(0x79B3F0);
	auto SetInvertMouse = reinterpret_cast<int(__stdcall *)(bool)>(0x79B420);
	auto SetKeyBinds = reinterpret_cast<int(__stdcall *)(void *, bool)>(0x79B440);
	auto SetLightingQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B760);
	auto SetLookSensitivity = reinterpret_cast<int(__stdcall *)(int)>(0x79B780);
	auto SetMasterVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B7D0);
	auto SetMotionBlur = reinterpret_cast<int(__stdcall *)(bool)>(0x79B860);
	auto SetMouseAcceleration = reinterpret_cast<int(__stdcall *)(int)>(0x79B880);
	auto SetMouseFilter = reinterpret_cast<int(__stdcall *)(bool)>(0x79B8A0);
	auto SetMouseSensitivityHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8C0);
	auto SetMouseSensitivityVehicleHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8E0);
	auto SetMouseSensitivityVehicleVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B900);
	auto SetMouseSensitivityVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B920);
	auto SetMusicVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B940);
	auto SetPostprocessQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B9C0);
	auto SetSFXVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B9E0);
	auto SetScreenResolution = reinterpret_cast<int(__stdcall *)(int, int)>(0x79BA30);
	auto SetShadowQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BAD0);
	auto SetTeamColor = reinterpret_cast<int(__stdcall *)(int)>(0x79BAF0);
	auto SetTextureFilteringQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BB10);
	auto SetTextureResolution = reinterpret_cast<int(__stdcall *)(int)>(0x79BB30);
	auto SetToggleCrouch = reinterpret_cast<int(__stdcall *)(int)>(0x79BB50);
	auto SetVSync = reinterpret_cast<int(__stdcall *)(int)>(0x79BB70);
	auto SetVoiceChatControl = reinterpret_cast<int(__stdcall *)(int)>(0x79BB90);
	auto SetVoiceChatVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79BBB0);
	auto SetVoiceVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79BBE0);

	bool VariableSettingsFullscreenUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		unsigned long value = Modules::ModuleSettings::Instance().VarFullscreen->ValueInt;

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		SetFullscreen(statusBool ? 1 : 0);

		returnInfo = "Fullscreen " + status;
		return true;
	}
}

namespace Modules
{
	ModuleSettings::ModuleSettings() : ModuleBase("Settings")
	{
		VarFullscreen = AddVariableInt("Fullscreen", "fullscreen", "Controls whether the game is windowed or fullscreen", eCommandFlagsArchived, 1, VariableSettingsFullscreenUpdate);
	}
}
