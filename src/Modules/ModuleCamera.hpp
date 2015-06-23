#pragma once

#include "ModuleBase.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamTypes.hpp"
#include <unordered_map>

namespace Modules
{
	class ModuleCamera : public Utils::Singleton<ModuleCamera>, public ModuleBase
	{
	public:
		Command* VarCameraCrosshair;
		Command* VarCameraFov;
		Command* VarCameraHideHud;
		Command* VarCameraMode;

		// patches to stop camera mode from changing
		Patch Debug1CameraPatch;
		Patch Debug2CameraPatch;
		Patch ThirdPersonPatch;
		Patch FirstPersonPatch;
		Patch DeadPersonPatch;

		Patch HideHudPatch;

		ModuleCamera();

		//std::unordered_map<std::string, CameraType> CameraTypeStrings;
		//std::unordered_map<CameraType, size_t> CameraTypeFunctions;
	};
}