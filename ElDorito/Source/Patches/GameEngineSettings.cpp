#include "GameEngineSettings.hpp"
#include "../Patch.hpp"
#include "../Blam/BitStream.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamTime.hpp"

namespace
{
	const float kPlayerScales[] = { 0.25f, 0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 2.0f, 3.0f };
	const auto kPlayerScaleCount = sizeof(kPlayerScales) / sizeof(kPlayerScales[0]);
	const float kPlayerScaleAcceleration = 0.85f;

	struct c_player_appearance_traits
	{
		uint8_t ActiveCamo;
		uint8_t Waypoint;
		uint8_t Aura;
		uint8_t ForcedColor;
	};

	void __fastcall c_player_appearance_traits__serialize_hook(c_player_appearance_traits *traits, void *unused, Blam::BitStream *stream);
	void __fastcall c_player_appearance_traits__deserialize_hook(c_player_appearance_traits *traits, void *unused, Blam::BitStream *stream);
	float __fastcall c_player_health_traits__get_damage_resistance_hook(uint8_t* thisptr, void *unused);
	void PlayerAuraHook();
	bool UnitCanRagdoll(uint32_t unitObjectIndex);
	bool RagdollHook();
}

namespace Patches::GameEngineSettings
{
	void ApplyAll()
	{
		// bottomless clip
		Patch(0x1CB9A1 + 2, { 3 }).Apply();
		Patch(0x1CBF32 + 2, { 3 }).Apply();
		Patch(0x1CE01A + 2, { 3 }).Apply();
		Patch(0x1CD586 + 2, { 3 }).Apply();
		Patch(0x1CCBED + 2, { 3 }).Apply();
		Patch(0x1CAE69 + 2, { 3 }).Apply();

		// waypoint
		Patch(0x1CB59A + 2, { 4 }).Apply();
		Patch(0x1CDEF6 + 2, { 4 }).Apply();
		Patch(0x1CBA15 + 2, { 4 }).Apply();
		Patch(0x1CC699 + 2, { 4 }).Apply();
		Patch(0x1CCE53 + 2, { 4 }).Apply();
		Hook(0x1CAF50, c_player_appearance_traits__serialize_hook).Apply();
		Hook(0x1CAC10, c_player_appearance_traits__deserialize_hook).Apply();

		// immune to assasinations
		Patch(0x1CB8CE + 2, { 13 }).Apply();
		Patch(0x1CDB74 + 2, { 13 }).Apply();
		Patch(0x1CD2C6 + 2, { 13 }).Apply();
		Patch(0x1CC94D + 2, { 13 }).Apply();
		Patch(0x1CAD38 + 2, { 13 }).Apply();
		Hook(0x1CC110, c_player_health_traits__get_damage_resistance_hook).Apply();

		// player size
		auto numPlayerSizes = uint8_t(kPlayerScaleCount + 2);
		Patch(0x1CB5B9 + 2, { numPlayerSizes }).Apply();
		Patch(0x1CBA25 + 2, { numPlayerSizes }).Apply();
		Patch(0x1CCEB6 + 2, { numPlayerSizes }).Apply();
		Patch(0x1CC6B7 + 2, { numPlayerSizes }).Apply();
		Patch(0x1CCE66 + 2, { numPlayerSizes }).Apply();
		
		// TODO: move this somewhere else
		Hook(0x14F3C0, PlayerAuraHook).Apply();
		Patch(0x14F3B6, 0x90, 6).Apply();
		Patch(0x14F3BD, 0x90, 2).Apply();
		Hook(0x7D847A, RagdollHook, HookFlags::IsCall).Apply();

		// disable appearance trait caching
		Patch(0x349781, { 0x90, 0x90 }).Apply();
		Patch(0x34978E, { 0xEB }).Apply();

	
	}
}

namespace
{
	void __fastcall c_player_appearance_traits__serialize_hook(c_player_appearance_traits *traits, void *unused, Blam::BitStream *stream)
	{
		const auto c_bitstream__flush = (void(__thiscall*)(Blam::BitStream *stream, unsigned long *pSize))(0x005580D0);
		stream->WriteUnsigned(traits->ActiveCamo, 3);
		stream->WriteUnsigned(traits->Waypoint, 3);
		stream->WriteUnsigned(traits->Aura, 5);
		stream->WriteUnsigned(traits->ForcedColor, 4);
	}

	void __fastcall c_player_appearance_traits__deserialize_hook(c_player_appearance_traits *traits, void *unused, Blam::BitStream *stream)
	{
		traits->ActiveCamo = stream->ReadUnsigned<uint8_t>(3);
		traits->Waypoint = stream->ReadUnsigned<uint8_t>(3);
		traits->Aura = stream->ReadUnsigned<uint8_t>(5);
		traits->ForcedColor = stream->ReadUnsigned<uint8_t>(4);
	}

	float __fastcall c_player_health_traits__get_damage_resistance_hook(uint8_t* thisptr, void *unused)
	{
		switch (*(uint8_t*)thisptr)
		{
		case 1: return 0.1f;
		case 2: return 0.5f;
		case 3: return 0.89999998f;
		case 5: return 1.1f;
		case 6: return 1.5f;
		case 7: return 2.0f;
		case 8: return 3.0f;
		case 9: return 5.0f;
		case 0xA: return 10.0f;
		case 0xB: return 20.0f;
		case 0xC:
		case 0xD: // invincible
			return  1000.0;
		}
		return 1.0f;
	}

	float LinearAccelerate(float current, float target, float dt)
	{
		float t = target - current;
		if (t > dt)
			return current + dt;
		if (t < -dt)
			return current - dt;
		return target;
	}

	void UpdatePlayerSize(uint32_t playerIndex)
	{
		const auto objects_scripting_set_scale = (void(*)(uint32_t objectIndex, float scale, float a3))(0x00B965C0);

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player || player->SlaveUnit == Blam::DatumHandle::Null)
			return;
		auto unitObject = Blam::Objects::Get(player->SlaveUnit);
		if (!unitObject)
			return;
		int playerScaleIndex = *((uint8_t*)player + 0x2DC2) - 2;
		if (playerScaleIndex < 0 || playerScaleIndex >= kPlayerScaleCount)
			return;

		auto currentScale = unitObject->Scale;
		auto desiredScale = kPlayerScales[playerScaleIndex];
		auto scale = LinearAccelerate(currentScale, desiredScale, Blam::Time::GetSecondsPerTick() * kPlayerScaleAcceleration);
		objects_scripting_set_scale(player->SlaveUnit, scale, 1.0f);	
	}

	_declspec(naked) void PlayerAuraHook()
	{
		__asm
		{
			pushad
			push[ebp + 8]
			call UpdatePlayerSize
			add esp, 4
			popad
			mov eax, [ebp + 0x18]
			pop edi
			pop esi
			mov[eax], cl
			pop ebx
			mov esp, ebp
			pop ebp
			retn
		}
	}

	bool UnitCanRagdoll(uint32_t unitObjectIndex)
	{
		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return false;

		// disable ragdolling when scale < 1.0 to avoid stretching issues
		return unitObject->Scale >= 1.0f;
	}

	bool RagdollHook()
	{
		const auto sub_765A30 = (bool(*)())(0x765A30);
		uint32_t unitObjectIndex;
		__asm { mov unitObjectIndex, edi }
		return sub_765A30() && UnitCanRagdoll(unitObjectIndex);
	}
}
