#include "BottomlessClip.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"

namespace
{
	bool player_has_infnite_ammo_hook(uint32_t playerIndex);
	void weapon_round_subtract_hook();
	void weapon_heat_generation_hook();
	void __fastcall chud_weapon_render_state_hook(void *thisptr, void *unused, uint32_t weaponObjectIndex, int a3, int a4);

	bool forced = false;
}

namespace Patches::BottomlessClip
{
	void ApplyAll()
	{
		Hook(0x75FACD, weapon_round_subtract_hook).Apply();
		Hook(0x7601DA, weapon_heat_generation_hook).Apply();
		Hook(0x151E50, player_has_infnite_ammo_hook).Apply();
		Hook(0x686239, chud_weapon_render_state_hook, HookFlags::IsCall).Apply();
		Hook(0x6862A8, chud_weapon_render_state_hook, HookFlags::IsCall).Apply();
	}

	// TODO: remove once the tags are done
	void Toggle(bool enabled)
	{
		forced = enabled;
	}
}

namespace
{
	inline bool player_has_bottomless_clip(uint32_t playerIndex)
	{
		if (forced)
			return true;

		const auto game_engine_get = (void*(*)())(0x005CE150);
		if (playerIndex == -1 || !game_engine_get())
			return false;

		const auto player = Blam::Players::GetPlayers().Get(playerIndex);
		return player && *(uint8_t*)((uint8_t*)player + 0x2DBA) == 3;
	}

	inline bool unit_has_bottomless_clip(uint32_t unitObjectIndex)
	{
		const auto game_engine_get_current = (void*(*)())(0x005CE150);
		if (!game_engine_get_current())
			return false;

		auto unitObject = (uint8_t*)Blam::Objects::Get(unitObjectIndex);
		return unitObject && player_has_bottomless_clip(*(uint32_t*)(unitObject + 0x198));
	}

	bool player_has_infnite_ammo_hook(uint32_t playerIndex)
	{
		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		auto infiniteAmmo = *(uint8_t*)((uint8_t*)player + 0x2DBA);
		return player && (infiniteAmmo == 2 || infiniteAmmo == 3 || forced);
	}

	void __fastcall chud_weapon_render_state_hook(void *thisptr, void *unused, uint32_t weaponObjectIndex, int a3, int a4)
	{
		const auto chud_build_weapon_hud_render_data = (void(__thiscall*)(void *thisptr, uint32_t weaponObjectIndex, int a3, int a4))(0x00A8AED0);

		chud_build_weapon_hud_render_data(thisptr, weaponObjectIndex, a3, a4);

		auto weaponObject = (uint8_t*)Blam::Objects::Get(weaponObjectIndex);
		if (!weaponObject)
			return;

		Blam::Objects::ObjectBase *unitObject{ nullptr };
		auto unitObjectIndex = *(uint32_t*)(weaponObject + 0x184);
		if (unitObjectIndex == -1 || !(unitObject = Blam::Objects::Get(unitObjectIndex)))
			return;

		Blam::Players::PlayerDatum *player{ nullptr };
		auto  playerIndex = *(uint32_t*)((uint8_t*)unitObject + 0x198);

		if (playerIndex != -1 && player_has_bottomless_clip(playerIndex))
		{
			auto offset = 0x5C * a3;
			*(float*)((uint8_t*)thisptr + offset + 0x34) = FLT_MAX;
			*(float*)((uint8_t*)thisptr + offset + 0x48) = FLT_MAX;
			*(float*)((uint8_t*)thisptr + offset + 0x50) = FLT_MAX;
		}
	}

	__declspec(naked) void weapon_round_subtract_hook()
	{
		__asm
		{
			pushad
			push[esi + 0x184]
			call unit_has_bottomless_clip
			add esp, 4
			test al, al
			popad
			jnz RETURN
			sub[ecx + esi + 0x28E], dx
			RETURN :
			push 0x00B5FAD5
			retn
		}
	}

	__declspec(naked) void weapon_heat_generation_hook()
	{
		__asm
		{
			pushad
			push[esi + 0x184]
			call unit_has_bottomless_clip
			add esp, 4
			test al, al
			popad
			jnz RETURN
			movss[esi + 0x1A4], xmm0
			RETURN :
			push 0x00B601E2
			retn
		}
	}
}
