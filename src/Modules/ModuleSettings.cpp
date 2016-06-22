#include "ModuleSettings.hpp"

namespace
{
	auto SSL_GetCharValue = reinterpret_cast<char(__cdecl *)(int)>(0x5047F0);
	auto SSL_GetIntValue = reinterpret_cast<int(__cdecl *)(int)>(0x5042A0);

	auto SSL_SetAntialiasing = reinterpret_cast<int(__stdcall *)(bool)>(0x79B240);
	auto SSL_SetAutoCentering = reinterpret_cast<int(__stdcall *)(bool)>(0x79B260);
	auto SSL_SetBrightness = reinterpret_cast<int(__stdcall *)(int)>(0x79B290);
	auto SSL_SetButtonLayout = reinterpret_cast<int(__stdcall *)(int)>(0x79B2B0);
	auto SSL_SetCameraFOV = reinterpret_cast<int(__stdcall *)(int)>(0x79B300);
	auto SSL_SetControlsMethod = reinterpret_cast<int(__stdcall *)(int)>(0x79B330);
	auto SSL_SetDetailsQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B350);
	auto SSL_SetDisplayHints = reinterpret_cast<int(__stdcall *)(bool)>(0x79B370);
	auto SSL_SetEffectsQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B390);
	auto SSL_SetFullscreen = reinterpret_cast<int(__stdcall *)(int)>(0x79B3B0);
	auto SSL_SetHUDShake = reinterpret_cast<int(__stdcall *)(bool)>(0x79B3D0);
	auto SSL_SetInvertLook = reinterpret_cast<int(__stdcall *)(bool)>(0x79B3F0);
	auto SSL_SetInvertMouse = reinterpret_cast<int(__stdcall *)(bool)>(0x79B420);
	auto SSL_SetKeyBinds = reinterpret_cast<int(__stdcall *)(void *, bool)>(0x79B440);
	auto SSL_SetLightingQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B760);
	auto SSL_SetLookSensitivity = reinterpret_cast<int(__stdcall *)(int)>(0x79B780);
	auto SSL_SetMasterVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B7D0);
	auto SSL_SetMotionBlur = reinterpret_cast<int(__stdcall *)(bool)>(0x79B860);
	auto SSL_SetMouseAcceleration = reinterpret_cast<int(__stdcall *)(int)>(0x79B880);
	auto SSL_SetMouseFilter = reinterpret_cast<int(__stdcall *)(bool)>(0x79B8A0);
	auto SSL_SetMouseSensitivityHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8C0);
	auto SSL_SetMouseSensitivityVehicleHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8E0);
	auto SSL_SetMouseSensitivityVehicleVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B900);
	auto SSL_SetMouseSensitivityVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B920);
	auto SSL_SetMusicVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B940);
	auto SSL_SetPostprocessQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B9C0);
	auto SSL_SetSFXVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B9E0);
	auto SSL_SetScreenResolution = reinterpret_cast<int(__stdcall *)(int, int)>(0x79BA30);
	auto SSL_SetShadowQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BAD0);
	auto SSL_SetTeamColor = reinterpret_cast<int(__stdcall *)(int)>(0x79BAF0);
	auto SSL_SetTextureFilteringQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BB10);
	auto SSL_SetTextureResolution = reinterpret_cast<int(__stdcall *)(int)>(0x79BB30);
	auto SSL_SetToggleCrouch = reinterpret_cast<int(__stdcall *)(int)>(0x79BB50);
	auto SSL_SetVSync = reinterpret_cast<int(__stdcall *)(int)>(0x79BB70);
	auto SSL_SetVoiceChatControl = reinterpret_cast<int(__stdcall *)(int)>(0x79BB90);
	auto SSL_SetVoiceChatVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79BBB0);
	auto SSL_SetVoiceVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79BBE0);

	bool VariableSettingsAntialiasingUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarAntialiasing->ValueInt;
		auto statusBool = (value != 0);

		if (value > 1)
			return false;

		SSL_SetAutoCentering(statusBool);

		std::stringstream ss;
		ss << "Antialiasing " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsAutoCenteringUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarAutoCentering->ValueInt;
		auto statusBool = (value != 0);

		if (value > 1)
			return false;

		SSL_SetAutoCentering(statusBool);

		std::stringstream ss;
		ss << "Auto-Centering " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsBrightnessUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarBrightness->ValueInt;

		if (value > 100)
			return false;

		SSL_SetBrightness(value);

		std::stringstream ss;
		ss << "Brightness set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsDetailsQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarDetailsQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetDetailsQuality(intValue);

		std::stringstream ss;
		ss << "Details quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsDisplayHintsUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarDisplayHints->ValueInt;
		auto statusBool = (value != 0);

		if (value > 1)
			return false;

		SSL_SetDisplayHints(statusBool);

		std::stringstream ss;
		ss << "Displaying of hints " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsEffectsQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarEffectsQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetEffectsQuality(intValue);

		std::stringstream ss;
		ss << "Effects quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsFullscreenUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarFullscreen->ValueInt;
		auto statusBool = (value != 0);

		if (value > 1)
			return false;

		SSL_SetFullscreen(statusBool);

		std::stringstream ss;
		ss << "Fullscreen " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}
}

namespace Modules
{
	ModuleSettings::ModuleSettings() : ModuleBase("Settings")
	{
		VarAntialiasing = AddVariableInt("Antialiasing", "antialiasing", "Controls whether antialiasing is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsAntialiasingUpdate);
		VarAutoCentering = AddVariableInt("AutoCentering", "autocenter", "Controls whether auto-centering is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsAutoCenteringUpdate);
		VarBrightness = AddVariableInt("Brightness", "brightness", "Controls the brightness level (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsBrightnessUpdate);
		VarDetailsQuality = AddVariableString("DetailsQuality", "details", "Controls whether the details quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsDetailsQualityUpdate);
		VarDisplayHints = AddVariableInt("DisplayHints", "hints", "Controls whether displaying of hints is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsDisplayHintsUpdate);
		VarEffectsQuality = AddVariableString("EffectsQuality", "effects", "Controls whether the effects quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsEffectsQualityUpdate);
		VarFullscreen = AddVariableInt("Fullscreen", "fullscreen", "Controls whether the game is windowed (0) or fullscreen (1)", eCommandFlagsArchived, 1, VariableSettingsFullscreenUpdate);
	}
}
