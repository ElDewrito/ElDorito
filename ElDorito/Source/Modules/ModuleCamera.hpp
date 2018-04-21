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
		Command* VarCameraCenteredCrosshairFirst;
		Command* VarCameraCenteredCrosshairThird;
		Command* VarCameraFov;
		Command* VarCameraHideHud;
		Command* VarCameraMode;
		Command* VarCameraSpeed;
		Command* VarCameraSave;
		Command* VarCameraLoad;
		Command* VarCameraPosition;
		Command* VarCameraShowCoordinates;

		// patches to stop camera mode from changing
		Patch Debug1CameraPatch;
		Patch Debug2CameraPatch;
		Patch ThirdPersonPatch;
		Patch ThirdPersonPatch2;
		Patch FirstPersonPatch;
		Patch DeadPersonPatch;

		Patch StaticILookVectorPatch;
		Patch StaticKLookVectorPatch;

		Patch HideHudPatch;
		Patch CenteredCrosshairFirstPersonPatch;
		Patch CenteredCrosshairThirdPersonPatch;

		Patch ShowCoordinatesPatch;

		Hook CameraPermissionHook;
		Hook CameraPermissionHookAlt1;
		Hook CameraPermissionHookAlt2;
		Hook CameraPermissionHookAlt3;

		ModuleCamera();

		void UpdatePosition();
		bool IsFlying() const;
	};
}