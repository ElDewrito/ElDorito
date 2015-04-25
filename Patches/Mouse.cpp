#include "Mouse.h"

#include "../ElDorito.h"
#include "../Patch.h"

namespace
{
	void AimAssistHook();
}

namespace Patches
{
	namespace Mouse
	{
		void ApplyAll()
		{
			// Aim assist hook
			Hook(0x18AA17, AimAssistHook).Apply();
		}
	}
}

namespace
{
	__declspec(naked) void AimAssistHook()
	{
		__asm
		{
			// Check if the player is using a mouse
			mov edx, 0x244DE98
			mov edx, [edx]
			test edx, edx
			jnz controller

			// Set magnetism angle to 0
			xor edx, edx
			mov[edi + 0x14], edx

			// Skip past magnetism angle code
			mov edx, 0x58AA23
			jmp edx

		controller:
			// Load magnetism angle normally
			movss xmm0, dword ptr[ebx + 0x388]
			mov edx, 0x58AA1F
			jmp edx
		}
	}
}