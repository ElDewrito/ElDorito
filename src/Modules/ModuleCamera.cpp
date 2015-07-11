#include "ModuleCamera.hpp"
#include <sstream>
#include "../ElDorito.hpp"
#include "../Patches/Ui.hpp"

// TODO: cleanup

namespace
{
	enum CameraDefinitionType : int
	{
		Position = 0,
		PositionShift = 1,
		LookShift = 2,
		Depth = 3,
		FieldOfView = 4,
		LookVectors = 5
	};

	// determine which camera definitions are editable based on the current camera mode
	bool __stdcall IsCameraDefinitionEditable(CameraDefinitionType definition)
	{
		auto mode = Utils::String::ToLower(Modules::ModuleCamera::Instance().VarCameraMode->ValueString);
		if (!mode.compare("third"))
		{
			if (definition == CameraDefinitionType::PositionShift ||
				definition == CameraDefinitionType::LookShift ||
				definition == CameraDefinitionType::Depth)
			{
				return true;
			}
		}
		else if (!mode.compare("flying") || !mode.compare("static"))
		{
			return true;
		}

		return false;
	}

	// hook @ 0x61440D - allows for the modification of specific camera components based on current perspective
	__declspec(naked) void UpdateCameraDefinitions()
	{
		__asm
		{
			pushad
			shr		esi, 1						; convert camera definition offset to an index
			push	esi							; CameraDefinitionType
			call	IsCameraDefinitionEditable
			test	al, al
			popad
			jnz		skip
			mov		eax, [eax + ecx * 4]		; get data(definition + item * 4)
			mov		[edi + ecx * 4], eax		; store it in the 3rd camera array
			skip:
			push	0614413h
			ret
		}
	}
	
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

		// update patches depending upon the current camera mode
		Modules::ModuleCamera::Instance().CameraPermissionHook.Apply(mode == "default");
		Modules::ModuleCamera::Instance().Debug1CameraPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().Debug2CameraPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().ThirdPersonPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().FirstPersonPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().DeadPersonPatch.Apply(mode == "default");
		Modules::ModuleCamera::Instance().HideHudPatch.Apply(mode != "flying" && mode != "static");

		// disable player movement while in flycam
		Pointer &playerControlGlobalsPtr = ElDorito::GetMainTls(GameGlobals::Input::TLSOffset)[0];
		playerControlGlobalsPtr(GameGlobals::Input::DisablePlayerInputIndex).Write(mode == "flying");

		// get new camera perspective function offset 
		size_t offset = 0x166ACB0;
		if (!mode.compare("first")) // c_first_person_camera
		{
			offset = 0x166ACB0;
		}
		else if (!mode.compare("third")) // c_following_camera
		{
			offset = 0x16724D4;

			// TOOD: set default camera shifts for better viewing angle
		}
		else if (!mode.compare("flying")) // c_flying_camera
		{
			// TODO: fix bug when transitioning to static/flycam from within a vehicle or during other special conditions

			offset = 0x16726D0;
		}
		else if (!mode.compare("static")) // c_static_camera
		{
			offset = 0x16728A8;
		}

		/*
		else if (!mode.compare("dead")) // c_dead_camera
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
		else if (!mode.compare("debug")) // c_camera
			offset = 0x1672130;
		else if (!mode.compare("debug2")) // c_null_camera - crashes
			offset = 0x165A6E4;
		else if (!mode.compare("unk4")) // c_authored_camera
			offset = 0x1672920;
		*/

		// update camera perspective function
		Pointer &directorPtr = ElDorito::GetMainTls(GameGlobals::Director::TLSOffset)[0];
		size_t oldOffset = directorPtr(GameGlobals::Director::CameraFunctionIndex).Read<size_t>();
		directorPtr(GameGlobals::Director::CameraFunctionIndex).Write(offset);

		// output old -> new perspective function information to console
		std::stringstream ss;
		ss << "0x" << std::hex << oldOffset << " -> 0x" << offset;
		returnInfo = ss.str();

		return true;
	}
}

namespace Modules
{
	ModuleCamera::ModuleCamera() : ModuleBase("Camera"), 
		CameraPermissionHook(0x21440D, UpdateCameraDefinitions),
		Debug1CameraPatch(0x325A80, 0x90, 6),
		Debug2CameraPatch(0x191525, 0x90, 6),
		ThirdPersonPatch(0x328640, 0x90, 6),
		FirstPersonPatch(0x25F420, 0x90, 6),
		DeadPersonPatch(0x329E6F, 0x90, 6),
		HideHudPatch(0x12B5A5C, { 0xC3, 0xF5, 0x48, 0x40 }), // 3.14f in hex form
		CenteredCrosshairPatch(0x25FA43, { 0x31, 0xC0, 0x90, 0x90 })
	{
		// TODO: commands for setting camera speed, positions, save/restore etc.

		VarCameraCrosshair = AddVariableInt("Crosshair", "crosshair", "Controls whether the crosshair should be centered", eCommandFlagsArchived, 0, VariableCameraCrosshairUpdate);
		VarCameraCrosshair->ValueIntMin = 0;
		VarCameraCrosshair->ValueIntMax = 1;

		VarCameraFov = AddVariableFloat("FOV", "fov", "The cameras field of view", eCommandFlagsArchived, 90.f, VariableCameraFovUpdate);
		VarCameraFov->ValueFloatMin = 55.f;
		VarCameraFov->ValueFloatMax = 155.f;

		VarCameraHideHud = AddVariableInt("HideHUD", "hud", "Toggles the HUD", eCommandFlagsArchived, 0, VariableCameraHideHudUpdate);
		VarCameraHideHud->ValueIntMin = 0;
		VarCameraHideHud->ValueIntMax = 1;

		this->VarCameraMode = AddVariableString("Mode", "camera_mode", "Camera mode, valid modes: default, first, third, flying, static", eCommandFlagsDontUpdateInitial, "default", VariableCameraModeUpdate);
	}

	void ModuleCamera::UpdatePosition()
	{
		auto mode = Utils::String::ToLower(Modules::ModuleCamera::Instance().VarCameraMode->ValueString);
		if (mode.compare("flying"))
			return;

		Pointer &directorGlobalsPtr = ElDorito::GetMainTls(GameGlobals::Director::TLSOffset)[0];
		Pointer &playerControlGlobalsPtr = ElDorito::GetMainTls(GameGlobals::Input::TLSOffset)[0];

		// TODO: allow these to be configurable settings
		float moveDelta = 0.04f;
		float lookDelta = 0.01f;

		// current values
		float hLookAngle = playerControlGlobalsPtr(0x30C).Read<float>();
		float vLookAngle = playerControlGlobalsPtr(0x310).Read<float>();
		float xPos = directorGlobalsPtr(0x834).Read<float>();
		float yPos = directorGlobalsPtr(0x838).Read<float>();
		float zPos = directorGlobalsPtr(0x83C).Read<float>();
		float xShift = directorGlobalsPtr(0x840).Read<float>();
		float yShift = directorGlobalsPtr(0x844).Read<float>();
		float zShift = directorGlobalsPtr(0x845).Read<float>();
		float hShift = directorGlobalsPtr(0x84C).Read<float>();
		float vShift = directorGlobalsPtr(0x850).Read<float>();
		float depth = directorGlobalsPtr(0x854).Read<float>();
		float fov = directorGlobalsPtr(0x858).Read<float>();
		float iForward = directorGlobalsPtr(0x85C).Read<float>();
		float jForward = directorGlobalsPtr(0x860).Read<float>();
		float kForward = directorGlobalsPtr(0x864).Read<float>();
		float iUp = directorGlobalsPtr(0x868).Read<float>();
		float jUp = directorGlobalsPtr(0x86C).Read<float>();
		float kUp = directorGlobalsPtr(0x870).Read<float>();
		float iRight = cos(hLookAngle + 3.14159265359 / 2);
		float jRight = sin(hLookAngle + 3.14159265359 / 2);

		// TODO: use shockfire's keyboard hooks instead

		// down
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			zPos -= moveDelta;
		}

		// up
		if (GetAsyncKeyState('E') & 0x8000)
		{
			zPos += moveDelta;
		}

		// forward
		if (GetAsyncKeyState('W') & 0x8000)
		{
			xPos += iForward * moveDelta;
			yPos += jForward * moveDelta;
			zPos += kForward * moveDelta;
		}

		// back
		if (GetAsyncKeyState('S') & 0x8000)
		{
			xPos -= iForward * moveDelta;
			yPos -= jForward * moveDelta;
			zPos -= kForward * moveDelta;
		}

		// left
		if (GetAsyncKeyState('A') & 0x8000)
		{
			xPos += iRight * moveDelta;
			yPos += jRight * moveDelta;
		}

		// right
		if (GetAsyncKeyState('D') & 0x8000)
		{
			xPos -= iRight * moveDelta;
			yPos -= jRight * moveDelta;
		}

		if (GetAsyncKeyState(VK_UP))
		{
			// TODO: look up
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			// TODO: look down
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			// TODO: look left
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			// TODO: look right
		}

		if (GetAsyncKeyState('Z') & 0x8000)
		{
			fov -= 0.003;
		}
		if (GetAsyncKeyState('C') & 0x8000)
		{
			fov += 0.003;
		}

		// update position
		directorGlobalsPtr(0x834).Write<float>(xPos);
		directorGlobalsPtr(0x838).Write<float>(yPos);
		directorGlobalsPtr(0x83C).Write<float>(zPos);

		// update look angles
		directorGlobalsPtr(0x85C).Write<float>(cos(hLookAngle) * cos(vLookAngle));
		directorGlobalsPtr(0x860).Write<float>(sin(hLookAngle) * cos(vLookAngle));
		directorGlobalsPtr(0x864).Write<float>(sin(vLookAngle));
		directorGlobalsPtr(0x868).Write<float>(-cos(hLookAngle) * sin(vLookAngle));
		directorGlobalsPtr(0x86C).Write<float>(-sin(hLookAngle) * sin(vLookAngle));
		directorGlobalsPtr(0x870).Write<float>(cos(vLookAngle));

		directorGlobalsPtr(0x858).Write<float>(fov);
	}
}