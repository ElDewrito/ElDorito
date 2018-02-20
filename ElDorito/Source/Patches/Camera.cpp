#include "Camera.hpp"
#include "../Modules/ModuleCamera.hpp"
#include <cstdint>

namespace
{
	float GetScriptedCameraFovHook();
	float ThirdPersonFovHook(uint32_t unitObjectIndex, float fov, int scopeLevel);
	void DeadCameraFovHook();
}

namespace Patches::Camera
{
	void ApplyAll()
	{
		Hook(0x32E18C, GetScriptedCameraFovHook, HookFlags::IsCall).Apply();
		Hook(0x32990C, ThirdPersonFovHook, HookFlags::IsCall).Apply();
		Hook(0x2122C5, DeadCameraFovHook).Apply();
	}

	void IncreaseLOD()
	{
		// LOD patches compliments of zedd
		// Patch for lod dword_176DB40 (2.0) -> dword_176DB50 (10.0)
		Patch(0x665EB7 + 4, { 0x50 }).Apply();
		// Patch to apply cine lod to everything
		Patch::NopFill(Pointer::Base(0x61962C), 2);
	}
}

namespace
{
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

	float ThirdPersonFovHook(uint32_t unitObjectIndex, float fov, int scopeLevel)
	{
		const auto sub_B44080 = (float(*)(uint32_t unitObjectIndex, float fov, int scopeLevel))(0xB44080);
		const auto &moduleCamera = Modules::ModuleCamera::Instance();
		return sub_B44080(unitObjectIndex, fov / moduleCamera.VarCameraFov->DefaultValueFloat * moduleCamera.VarCameraFov->ValueFloat, scopeLevel);
	}

	float *GetPreferedCameraFovPointer()
	{
		return &Modules::ModuleCamera::Instance().VarCameraFov->ValueFloat;
	}

	__declspec(naked) void DeadCameraFovHook()
	{
		__asm
		{
			test dword ptr[ecx], 0x800
			jnz NOT_DEAD
			pushad
			call GetPreferedCameraFovPointer
			movss xmm0, [eax]
			popad
			mov eax, 0x006122E0
			jmp eax
			NOT_DEAD:
			mov eax, 0x0061230A
			jmp eax
		}
	}
}
