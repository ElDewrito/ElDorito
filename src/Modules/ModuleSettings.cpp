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
	auto SSL_SetMasterVolume = reinterpret_cast<int(__stdcall *)(int, char)>(0x79B7D0);
	auto SSL_SetMotionBlur = reinterpret_cast<int(__stdcall *)(bool)>(0x79B860);
	auto SSL_SetMouseAcceleration = reinterpret_cast<int(__stdcall *)(int)>(0x79B880);
	auto SSL_SetMouseFilter = reinterpret_cast<int(__stdcall *)(bool)>(0x79B8A0);
	auto SSL_SetMouseSensitivityHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8C0);
	auto SSL_SetMouseSensitivityVehicleHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8E0);
	auto SSL_SetMouseSensitivityVehicleVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B900);
	auto SSL_SetMouseSensitivityVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B920);
	auto SSL_SetMusicVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B940);
	auto SSL_SetPostprocessingQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B9C0);
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

		if (value < 0 || value > 1)
			return false;

		SSL_SetAntialiasing(statusBool);

		std::stringstream ss;
		ss << "Antialiasing " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsAutoCenteringUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarAutoCentering->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
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

		if (value < 0 || value > 100)
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
		ss << "Details Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsDisplayHintsUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarDisplayHints->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
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
		ss << "Effects Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsFullscreenUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarFullscreen->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetFullscreen(statusBool);

		std::stringstream ss;
		ss << "Fullscreen " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsHUDShakeUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarHUDShake->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetHUDShake(statusBool);

		std::stringstream ss;
		ss << "HUD Shake " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsInvertLookUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarInvertLook->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetInvertLook(statusBool);

		std::stringstream ss;
		ss << "Invert Look " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsInvertMouseUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarInvertMouse->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetInvertMouse(statusBool);

		std::stringstream ss;
		ss << "Invert Mouse " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsLightingQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarLightingQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetLightingQuality(intValue);

		std::stringstream ss;
		ss << "Lighting Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsLookSensitivityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarLookSensitivity->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetLookSensitivity(value);

		std::stringstream ss;
		ss << "Look Sensitivity set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMotionBlurUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMotionBlur->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetMotionBlur(statusBool);

		std::stringstream ss;
		ss << "Motion Blur " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseAccelerationUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseAcceleration->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseAcceleration(value);

		std::stringstream ss;
		ss << "Mouse Acceleration set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseFilterUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseFilter->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetMouseFilter(statusBool);

		std::stringstream ss;
		ss << "Mouse Filter " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityHorizontalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityHorizontal->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityHor(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Horizontal set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityVerticalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityVertical->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityVert(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Vertical set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityVehicleHorizontalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityVehicleHorizontal->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityVehicleHor(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Vehicle Horizontal set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityVehicleVerticalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityVehicleVertical->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityVehicleVert(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Vehicle Vertical set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsPostprocessingQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarPostprocessingQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetPostprocessingQuality(intValue);

		std::stringstream ss;
		ss << "Postprocessing Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsShadowQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarShadowQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetShadowQuality(intValue);

		std::stringstream ss;
		ss << "Shadow Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsTextureFilteringQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarTextureFilteringQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetTextureFilteringQuality(intValue);

		std::stringstream ss;
		ss << "Texture Filtering Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsTextureResolutionUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarTextureResolution->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetTextureResolution(intValue);

		std::stringstream ss;
		ss << "Texture Resolution set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsToggleCrouchUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarToggleCrouch->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetToggleCrouch(statusBool);

		std::stringstream ss;
		ss << "Toggle Crouch " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsVSyncUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarVSync->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetVSync(statusBool);

		std::stringstream ss;
		ss << "Vertical Sync " << (statusBool ? "enabled." : "disabled.");
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
		VarEffectsQuality = AddVariableString("EffectsQuality", "effects", "Controls whether the effects Quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsEffectsQualityUpdate);
		VarFullscreen = AddVariableInt("Fullscreen", "fullscreen", "Controls whether the game is windowed (0) or fullscreen (1)", eCommandFlagsArchived, 1, VariableSettingsFullscreenUpdate);
		VarHUDShake = AddVariableInt("HUDShake", "hud_shake", "Controls whether hud shake is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsHUDShakeUpdate);
		VarInvertLook = AddVariableInt("InvertLook", "invert_look", "Controls whether look-inversion is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsInvertLookUpdate);
		VarInvertMouse = AddVariableInt("InvertMouse", "invert_mouse", "Controls whether mouse-inversion is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsInvertMouseUpdate);
		VarLightingQuality = AddVariableString("LightingQuality", "lighting", "Controls whether the lighting quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsLightingQualityUpdate);
		VarLookSensitivity = AddVariableInt("LookSensitivity", "look_sensitivity", "Controls the controller look sensitivity (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsLookSensitivityUpdate);
		VarMotionBlur = AddVariableInt("MotionBlur", "motion_blur", "Controls whether motion blur is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsMotionBlurUpdate);
		VarMouseAcceleration = AddVariableInt("MouseAcceleration", "mouse_accel", "Controls the level of mouse acceleration (0 - 100)", eCommandFlagsArchived, 0, VariableSettingsMouseAccelerationUpdate);
		VarMouseFilter = AddVariableInt("MouseFilter", "mouse_filter", "Controls whether mouse-filtering is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsMouseFilterUpdate);
		VarMouseSensitivityHorizontal = AddVariableInt("MouseSensitivityHorizontal", "mouse_sensitivity_hor", "Controls the horizontal mouse sensitivity (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsMouseSensitivityHorizontalUpdate);
		VarMouseSensitivityVertical = AddVariableInt("MouseSensitivityVertical", "mouse_sensitivity_vert", "Controls the vertical mouse sensitivity (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsMouseSensitivityVerticalUpdate);
		VarMouseSensitivityVehicleHorizontal = AddVariableInt("MouseSensitivityVehicleHorizontal", "mouse_sensitivity_vehicle_hor", "Controls the horizontal mouse sensitivity for vehicles (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsMouseSensitivityVehicleHorizontalUpdate);
		VarMouseSensitivityVehicleVertical = AddVariableInt("MouseSensitivityVehicleVertical", "mouse_sensitivity_vehicle_vert", "Controls the vertical mouse sensitivity for vehicles (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsMouseSensitivityVehicleVerticalUpdate);
		VarPostprocessingQuality = AddVariableString("PostprocessingQuality", "postprocessing", "Controls whether the postprocessing quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsPostprocessingQualityUpdate);
		VarShadowQuality = AddVariableString("ShadowQuality", "shadows", "Controls whether the shadow quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsShadowQualityUpdate);
		VarTextureFilteringQuality = AddVariableString("TextureFilteringQuality", "texture_filtering", "Controls whether the texture-filtering quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsTextureFilteringQualityUpdate);
		VarTextureResolution = AddVariableString("TextureResolution", "textures", "Controls whether the texture resolution level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsTextureResolutionUpdate);
		VarToggleCrouch = AddVariableInt("ToggleCrouch", "toggle_crouch", "Controls whether crouch-toggling is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsToggleCrouchUpdate);
		VarVSync = AddVariableInt("VSync", "vsync", "Controls whether vertical sync is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsVSyncUpdate);
	}
}
