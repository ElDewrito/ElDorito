#include "Forge.hpp"

#include "../Patch.hpp"

namespace
{
	bool shouldDelete = false;
	void UpdateForgeInputHook();
}

namespace Patches
{
	namespace Forge
	{
		void ApplyAll()
		{
			Hook(0x19D482, UpdateForgeInputHook, HookFlags::IsCall).Apply();
		}

		void SignalDelete()
		{
			shouldDelete = true;
		}
	}
}

namespace
{
	__declspec(naked) void UpdateForgeInputHook()
	{
		__asm
		{
			mov al, shouldDelete
			test al, al
			jnz del

			// Not deleting - just call the original function
			push esi
			mov eax, 0x59F0E0
			call eax
			retn 4

		del:
			mov shouldDelete, 0

			// Simulate a Y button press
			mov eax, 0x244D1F0              // Controller data
			mov byte ptr [eax + 0x9E], 1    // Ticks = 1
			and byte ptr [eax + 0x9F], 0xFE // Clear the "handled" flag

			// Call the original function
			push esi
			mov eax, 0x59F0E0
			call eax

			// Make sure nothing else gets the fake press
			mov eax, 0x244D1F0          // Controller data
			or byte ptr [eax + 0x9F], 1 // Set the "handled" flag
			retn 4
		}
	}
}