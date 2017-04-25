#include "Mouse.hpp"

#include "../ElDorito.hpp"
#include "../Patch.hpp"

namespace
{
	void AimAssistHook();
	uint32_t DualAimAssistHook(uint32_t unitObject, short weaponIndex);
}

namespace Patches
{
	namespace Mouse
	{
		void ApplyAll()
		{
			Hook(0x18AA17, AimAssistHook).Apply();
			Hook(0x18ABAB, DualAimAssistHook, HookFlags::IsCall).Apply();
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

	uint32_t DualAimAssistHook(uint32_t unitObject, short weaponIndex)
	{
		// If using a mouse, report that there's no weapon being dual wielded
		if (!Pointer::Base(0x204DE98).Read<uint32_t>())
			return 0xFFFFFFFF;

		// Otherwise, get the weapon datum index normally
		typedef uint32_t(*UnitGetWeaponPtr)(uint32_t unitObject, short weaponIndex);
		auto UnitGetWeapon = reinterpret_cast<UnitGetWeaponPtr>(0xB454D0);
		return UnitGetWeapon(unitObject, weaponIndex);
	}
}