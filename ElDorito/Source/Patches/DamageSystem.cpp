#include "DamageSystem.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Patch.hpp"

namespace
{
	uint32_t MeleeDamageHook(uint8_t *damageData, uint32_t unitObjectIndex, int a3);
	void UnsuspectingLethalDamageHook();
}

namespace Patches::DamageSystem
{
	void ApplyAll()
	{
		// fix melee damage acceleration
		Hook(0x73D10A, MeleeDamageHook, HookFlags::IsCall).Apply();
		// allow for immunity from assassinations
		Hook(0x753AB6, UnsuspectingLethalDamageHook).Apply();
	}
}

namespace
{
	uint32_t MeleeDamageHook(uint8_t *damageData, uint32_t unitObjectIndex, int a3)
	{
		const auto sub_B542A0 = (uint32_t(*)(uint8_t *damageData, uint32_t unitObjectIndex, int a3))(0xB542A0);
		*(float*)(damageData + 0x54) = 1.0f; // acceleration scale, scale
		return sub_B542A0(damageData, unitObjectIndex, a3);
	}


	bool UnitHasInvincibleTrait(uint32_t unitObjectIndex)
	{
		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return false;
		auto playerIndex = *(uint32_t*)((char*)unitObject + 0x198);
		if (playerIndex == -1)
			return false;
		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return false;

		auto damageResistance = *((uint8_t*)player + 0x2DAC);
		return damageResistance == 15;
	}

	__declspec(naked) void UnsuspectingLethalDamageHook()
	{
		__asm
		{
			cmp word ptr[eax], 02
			jne not_lethal
			push[edi + 0xC]
			call UnitHasInvincibleTrait
			add esp, 4
			test al, al
			jnz not_lethal
			push 0x00B53ABC
			retn
			not_lethal :
			push 0x00B53B37
			retn
		}
	}
}
