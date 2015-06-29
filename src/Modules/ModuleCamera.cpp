#include "ModuleCamera.hpp"
#include <sstream>
#include "../ElDorito.hpp"
#include "../Patches/Ui.hpp"

namespace
{
	bool VariableCameraCrosshairUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		unsigned long value = Modules::ModuleCamera::Instance().VarCameraCrosshair->ValueInt;

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		Modules::ModuleCamera::Instance().CenteredCrosshairPatch.Apply(!statusBool);

		returnInfo = "Centered crosshair " + status;
		return true;
	}

	bool VariableCameraFovUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		float value = Modules::ModuleCamera::Instance().VarCameraFov->ValueFloat;

		Pointer::Base(0x1F01D98).Write(value);
		Pointer::Base(0x149D42C).Write(value);

		return true;
	}

	bool VariableCameraHideHudUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		unsigned long value = Modules::ModuleCamera::Instance().VarCameraHideHud->ValueInt;

		std::string status = "shown.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "hidden.";

		Modules::ModuleCamera::Instance().HideHudPatch.Apply(!statusBool);

		returnInfo = "HUD " + status;
		return true;
	}

	bool VariableCameraModeUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto mode = Utils::String::ToLower(Modules::ModuleCamera::Instance().VarCameraMode->ValueString);

		size_t offset = 0x166ACB0;
		if (!mode.compare("first")) // c_first_person_camera
			offset = 0x166ACB0;
		else if (!mode.compare("third")) // c_following_camera
			offset = 0x16724D4;
		/*else if (!mode.compare("dead")) // c_dead_camera
			offset = 0x16725DC;
		else if (!mode.compare("scripted")) // c_scripted_camera
			offset = 0x167280C;
		else if (!mode.compare("pancam1")) // c_director
			offset = 0x165A64C;
		else if (!mode.compare("pancam2"))
			offset = 0x165A680;
		else if (!mode.compare("pancam3"))
			offset = 0x165A674;
		else if (!mode.compare("unk1")) // c_orbiting_camera
			offset = 0x167265C;
		else if (!mode.compare("flying")) // c_flying_camera
			offset = 0x16726D0;
		else if (!mode.compare("unk3")) // c_static_camera
			offset = 0x16728A8;
		else if (!mode.compare("unk4")) // c_authored_camera
			offset = 0x1672920;
		else if (!mode.compare("debug1")) // c_camera
			offset = 0x1672130;
		else if (!mode.compare("debug2")) // c_null_camera
			offset = 0x165A6E4;*/

		Pointer &directorPtr = ElDorito::GetMainTls(GameGlobals::Director::TLSOffset)[0];

		size_t oldOffset = directorPtr(GameGlobals::Director::CameraFunctionIndex).Read<size_t>();
		directorPtr(GameGlobals::Director::CameraFunctionIndex).Write(offset);

		Modules::ModuleCamera::Instance().Debug1CameraPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().Debug2CameraPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().ThirdPersonPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().FirstPersonPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().DeadPersonPatch.Apply(mode == "default");

		std::stringstream ss;
		ss << "0x" << std::hex << oldOffset << " -> 0x" << offset;
		returnInfo = ss.str();
		return true;
	}
}

namespace Modules
{
	ModuleCamera::ModuleCamera() : ModuleBase("Camera"), Debug1CameraPatch(0x325A80, 0x90, 6),
		Debug2CameraPatch(0x191525, 0x90, 6),
		ThirdPersonPatch(0x328640, 0x90, 6),
		FirstPersonPatch(0x25F420, 0x90, 6),
		DeadPersonPatch(0x329E6F, 0x90, 6),
		HideHudPatch(0x12B5A5C, { 0xC3, 0xF5, 0x48, 0x40 }), // 3.14f in hex form
		CenteredCrosshairPatch(0x25FA43, { 0x31, 0xC0, 0x90, 0x90 })
	{
		VarCameraCrosshair = AddVariableInt("Crosshair", "crosshair", "Controls whether the crosshair should be centered", eCommandFlagsArchived, 0, VariableCameraCrosshairUpdate);
		VarCameraCrosshair->ValueIntMin = 0;
		VarCameraCrosshair->ValueIntMax = 1;

		VarCameraFov = AddVariableFloat("FOV", "fov", "The cameras field of view", eCommandFlagsArchived, 90.f, VariableCameraFovUpdate);
		VarCameraFov->ValueFloatMin = 55.f;
		VarCameraFov->ValueFloatMax = 155.f;

		VarCameraHideHud = AddVariableInt("HideHUD", "hud", "Toggles the HUD", eCommandFlagsArchived, 0, VariableCameraHideHudUpdate);
		VarCameraHideHud->ValueIntMin = 0;
		VarCameraHideHud->ValueIntMax = 1;

		this->VarCameraMode = AddVariableString("Mode", "camera_mode", "Camera mode, valid modes: default, first, third", eCommandFlagsDontUpdateInitial, "default", VariableCameraModeUpdate);
	}
}