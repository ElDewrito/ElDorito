#include "Camera.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../ElDorito.hpp"
#include "../Modules/ModuleCamera.hpp"
#include <cstdint>

using namespace Patches::Camera;

namespace
{
	float GetScriptedCameraFovHook();
	void DeadCameraFovHook();
	void __fastcall c_following_camera__update_hook(void *ptr, void *unused, int localPlayerIndex, int a4, uint8_t *state);
	void CameraModeChangedHook();

	void __stdcall RunCameraChangedCallbacks();
	std::vector<CameraChangeCallback> OnCameraChangedCallbacks;

	bool tagsInitiallLoaded = false;
}

namespace Patches::Camera
{
	void OnCameraChange(CameraChangeCallback callback)
	{
		OnCameraChangedCallbacks.push_back(callback);
	}

	void ApplyAll()
	{
		Hook(0x32E18C, GetScriptedCameraFovHook, HookFlags::IsCall).Apply();
		Hook(0x2122C5, DeadCameraFovHook).Apply();
		Pointer(0x016724DC).Write(uint32_t(&c_following_camera__update_hook));
		Hook(0x193370, CameraModeChangedHook, HookFlags::IsCall).Apply();

		OnCameraChange(UpdateActiveFOVForCamera);
	}

	void ApplyAfterTagsLoaded()
	{
		tagsInitiallLoaded = true;
	}

	void IncreaseLOD()
	{
		// LOD patches compliments of zedd
		// Patch for lod dword_176DB40 (2.0) -> dword_176DB50 (10.0)
		Patch(0x665EB7 + 4, { 0x50 }).Apply();
		// Patch to apply cine lod to everything
		Patch::NopFill(Pointer::Base(0x61962C), 2);
	}

	float activeFOV;

	void UpdateActiveFOV()
	{
		if (!tagsInitiallLoaded)
			return;

		Pointer directorPtr(ElDorito::GetMainTls(GameGlobals::Director::TLSOffset)[0]);
		auto cameraFunc = directorPtr(GameGlobals::Director::CameraFunctionIndex).Read<size_t>();

		UpdateActiveFOVForCamera(cameraFunc);
	}

	void UpdateActiveFOVForCamera(int cameraFunc)
	{
		switch (cameraFunc) //Add cases as required.
		{
		case 0x166ACB0: //c_first_person_camera
		case 0x165A6E4: //c_null_camera
			activeFOV = Modules::ModuleCamera::Instance().VarCameraFirstFov->ValueFloat;
			Pointer::Base(0x1F01D98).Write(activeFOV);
			Pointer::Base(0x149D42C).Write(activeFOV);
			break;

		case 0x16724D4: //c_following_camera
		case 0x16725DC: //c_dead_camera
		case 0x167265C: //c_orbiting_camera
		case 0x167280C: //c_scripted_camera
		case 0x16726D0: //c_flying_camera
		case 0x16728A8: //c_static_camera
		default:
			activeFOV = Modules::ModuleCamera::Instance().VarCameraThirdFov->ValueFloat;
			break;
		}
	}
}

namespace
{
	void __stdcall RunCameraChangedCallbacks()
	{
		Pointer directorPtr(ElDorito::GetMainTls(GameGlobals::Director::TLSOffset)[0]);
		auto cameraFunc = directorPtr(GameGlobals::Director::CameraFunctionIndex).Read<size_t>();

		// Dispatch the event to handlers
		for (auto &&callback : OnCameraChangedCallbacks)
			callback(cameraFunc);
	}

	float GetScriptedCameraFovHook()
	{
		const auto game_is_mainmenu = (bool(*)())(0x00531E90);

		auto fov = 0.0f;

		if (game_is_mainmenu())
		{
			fov = 70.0f;
		}
		else
		{
			auto scriptedCameraFov = *(float*)0x018ECE00;
			if (scriptedCameraFov == 0.0f)
				fov = *(float*)0x18BB1C8;
			else
				fov = scriptedCameraFov;
		}

		return fov * 0.017453292f;
	}

	float *GetActiveFOVPointer()
	{
		return &Patches::Camera::activeFOV;
	}

	__declspec(naked) void DeadCameraFovHook()
	{
		__asm
		{
			test dword ptr[ecx], 0x800
			jnz NOT_DEAD
			pushad
			call GetActiveFOVPointer
			movss xmm0, [eax]
			popad
			mov eax, 0x006122E0
			jmp eax
			NOT_DEAD:
			mov eax, 0x0061230A
			jmp eax
		}
	}

	void __fastcall c_following_camera__update_hook(void *thisptr, void *unused, int localPlayerIndex, int a4, uint8_t *state)
	{
		struct s_camera_definition
		{
			Blam::Math::RealVector3D Position;
			Blam::Math::RealVector3D PositionShift;
			float LookShift[2];
			float Depth;
			float FieldOfView;
			Blam::Math::RealVector3D Forward;
			Blam::Math::RealVector3D Up;
			// ...
		};

		const auto c_following_camera__update = (void(__thiscall *)(void *ptr, int localPlayerIndex, int a4, uint8_t *state))(0x00729730);
		const auto &moduleCamera = Modules::ModuleCamera::Instance();

		c_following_camera__update(thisptr, localPlayerIndex, a4, state);

		auto &definition = *(s_camera_definition*)(state + 0x4);

		// scale fov by prefered fov
		definition.FieldOfView = definition.FieldOfView / moduleCamera.VarCameraThirdFov->DefaultValueFloat * Patches::Camera::activeFOV;

		// scale position shift and depth with target object scale
		auto targetObjectIndex = *(uint32_t*)((uint8_t*)thisptr + 0x4);
		if (targetObjectIndex == -1)
			return;
		auto targetObject = Blam::Objects::Get(targetObjectIndex);
		if (!targetObject)
			return;	
		auto s = std::pow(targetObject->Scale, 0.5);
		definition.Depth *= s;
		definition.PositionShift *= s;
	}

	__declspec(naked) void CameraModeChangedHook()
	{
		__asm
		{
			//execute custom code
			call RunCameraChangedCallbacks

			//perform original instruction
			movss xmm0, [ebp + 0xC]

			//return to eldorado code
			ret
		}
	}
}
