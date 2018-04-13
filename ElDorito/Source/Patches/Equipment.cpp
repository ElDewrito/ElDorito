#include "Equipment.hpp"
#include "../Pointer.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamData.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Tags/TagBlock.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Items/Item.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/BlamTime.hpp"
#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include <cstdint>
#include <cassert>
#include <unordered_map>

namespace
{
	void EquipmentPickupHook();
	void EquipmentActionStateHook();
	char __cdecl Unit_EquipmentDetachHook(uint32_t unitObjectIndex, uint32_t equipmentObjectIndex, int a3);
	void* __cdecl Player_GetArmorAbilitiesCHUDHook(Blam::Players::PlayerDatum* playerDatum);
	bool __cdecl UnitUpdateHook(uint32_t unitObjectIndex);
	void __cdecl EquipmentUseHook(int unitObjectIndex, int slotIndex, unsigned int isClient);
	void __cdecl UnitDeathHook(int unitObjectIndex, int a2, int a3);
	void DespawnEquipmentHook();
	void OvershieldDecayHook();
	void VisionEndHook();
	void VisionEndHook2();
}

namespace Patches::Equipment
{
	void ApplyAll()
	{
		// implements the missing functionality in order to pickup equipment
		Hook(0x139888, EquipmentPickupHook, HookFlags::IsJmpIfNotEqual).Apply();

		// so the unit drops their equipment on death and when swapping
		Hook(0x0073FC9B, Unit_EquipmentDetachHook, HookFlags::IsCall).Apply();

		// this gets called by the chud renderer, hud state builder in order to display the equipment
		// we're hijacking it and returning our own state to match the unit's equipment at 0x2F0
		Hook(0x006858FE, Player_GetArmorAbilitiesCHUDHook, HookFlags::IsCall).Apply();

		// for use duration and max uses
		Hook(0x00749F23, UnitUpdateHook, HookFlags::IsCall).Apply();
		Hook(0x0013DC4B, EquipmentUseHook, HookFlags::IsCall).Apply(); // host
		Hook(0x000BA06A, EquipmentUseHook, HookFlags::IsCall).Apply(); // client

																		// disable armor ability slot checks preventing use 
		Patch::NopFill(Pointer::Base(0x786CFF), 6);
		Patch::NopFill(Pointer::Base(0x74CFF4), 6);
		Patch::NopFill(Pointer::Base(0x137F4D), 2);

		// remove initial equipment use delay
		Patch::NopFill(Pointer::Base(0x13DC0A), 2);
		Patch(0x137F20, { 0xB0, 0x01, 0xC3 }).Apply();

		// stop new equipment from destroying the old one
		Patch(0x78A179, { 0xEB }).Apply();
		Patch(0x78950D, { 0xEB }).Apply();

		// mid-function jump table hook for equipment.
		// this gets executed for each equipment object returned from a cluster search radius-
		// and is used in order to populate the control globals action structure with the information needed for swapping
		Pointer(0x00539E08 + 3 * 4).Write(uint32_t(&EquipmentActionStateHook));

		// drop grenades on unit death
		Hook(0x73F562, UnitDeathHook, HookFlags::IsCall).Apply();
		Hook(0x14EE29, DespawnEquipmentHook, HookFlags::IsJmpIfNotEqual).Apply();

		// prevent movement depowering active camo
		Patch(0x74A0FB, { 0xEB }).Apply();
		// fix overshield decay
		Pointer(0x00B5620F).Write(uint32_t(OvershieldDecayHook) - 0x00B5620F - 4);

		// reimplmented so that unrelated screen effects aren't deleted
		Hook(0x789462, VisionEndHook, HookFlags::IsCall).Apply();
		Hook(0x788703, VisionEndHook2, HookFlags::IsCall).Apply();
	}
}

namespace
{
	const auto equipment_get_max_use_duration = (float(*)(uint32_t equipmentObjectIndex))(0x00B889F0);
	const auto equipment_get_remaining_uses = (int(*)(uint32_t equipmentObjectIndex))(0x00B89190);
	const auto sub_B87DA0 = (float(*)(int equipmentObjectIndex))(0xB87DA0);
	const auto IsClient = (bool(*)())(0x00531D70);

	using namespace Blam::Math;

	void __stdcall DoPickup(uint32_t playerIndex, uint32_t objectIndex)
	{
		struct PickupData
		{
			uint32_t Type;
			uint32_t ObjectIndex;
			uint32_t Unknown08;
			uint8_t Unknown0C[0x3C];
		};

		const auto Equipment_PlayPickupSound = (int(__cdecl *)(uint16_t playerIndex, uint32_t eqipTagIndex))(0xB887B0);
		const auto sub_4B31C0 = (void(_cdecl *)(uint32_t unitObjectIndex, uint32_t eqipTagIndex))(0x4B31C0);
		const auto Unit_Pickup = (char(__cdecl *)(uint32_t unitIndex, const PickupData *pickupData))(0xB69C50);

		struct HUDIterator
		{
			uint32_t PlayerIndex;
			uint32_t HudIndex;

			HUDIterator(uint32_t playerIndex) : PlayerIndex(playerIndex), HudIndex(-1) {}
			bool Advance() { return ((bool(__thiscall*)(HUDIterator* iterator))(0x00589770))(this); }
		};

		PickupData pickupdata = { 0 };
		pickupdata.Type = 0x3D;
		pickupdata.ObjectIndex = objectIndex;

		if (playerIndex == -1)
			return;
		auto playerDatum = Blam::Players::GetPlayers().Get(playerIndex);
		if (!playerDatum)
			return;
		auto unitObject = Blam::Objects::Get(playerDatum->SlaveUnit);
		if (!unitObject)
			return;

		auto equipmentObject = Blam::Objects::Get(objectIndex);
		if (!equipmentObject)
			return;

		// return if the unit already holds equipment
		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();
		if (primaryEquipmentObjectIndex != -1)
			return;

		if (!IsClient())
		{
			HUDIterator hudIterator(playerIndex);
			while (hudIterator.Advance())
			{
				static auto DisplayPickupMessage = (int(__cdecl*)(uint32_t hudIndex, uint32_t eqipTagIndex))(0xA95850);
				DisplayPickupMessage(hudIterator.HudIndex, equipmentObject->TagIndex);
			}


			Equipment_PlayPickupSound(playerIndex, equipmentObject->TagIndex);
			sub_4B31C0(playerDatum->SlaveUnit, equipmentObject->TagIndex);
			Unit_Pickup(playerDatum->SlaveUnit, &pickupdata);
		}
	}

	void __stdcall CleanupEquipment(uint32_t unitObjectIndex, int equipmentSlotIndex)
	{
		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return;

		auto equipmentObjectIndex = Pointer(unitObject)(0x2F0 + 4 * equipmentSlotIndex).Read<uint32_t>();

		if (equipmentObjectIndex == -1)
			return;

		// essentially a copy of sub_B3FC50 with a few tweaks in order to dispose of the equipment correctly
		static auto sub_B40A70 = (int(__cdecl *)(int a1, int a2, int a3))(0xB40A70);
		auto sub_4B1E70 = (int(__thiscall *)(void* thisptr, int unitIndex, int a3))(0x4B1E70);
		auto sub_4B3010 = (int(__cdecl *)(int objectIndex, void* a2))(0x4B3010);

		sub_B40A70(unitObjectIndex, equipmentObjectIndex, 1);
		Pointer(unitObject)(0x2F0 + 4 * equipmentSlotIndex).WriteFast<int32_t>(-1);
		Pointer(unitObject)(0x310).WriteFast<int32_t>(-1);

		uint8_t unknown[8] = { 0 };
		sub_4B1E70(unknown, unitObjectIndex, 0x1Cu);
		sub_4B3010(unitObjectIndex, unknown);
	}

	bool __cdecl UnitUpdateHook(uint32_t unitObjectIndex)
	{
		const auto UnitUpdate = (bool(__cdecl*)(uint32_t unitObjectIndex))(0xB4C6D0);

		auto ret = UnitUpdate(unitObjectIndex);

		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return ret;

		for (auto i = 0; i < 4; i++)
		{
			auto equipmentObjectIndex = Pointer(unitObject)(0x2F0 + 4 * i).Read<uint32_t>();
			if (equipmentObjectIndex == -1)
				return ret;

			if (equipment_get_remaining_uses(equipmentObjectIndex) == 0 && sub_B87DA0(equipmentObjectIndex) == 0.0f)
				CleanupEquipment(unitObjectIndex, i);
		}

		return ret;
	}

	void __cdecl EquipmentUseHook(int unitObjectIndex, int slotIndex, unsigned int isClient)
	{
		const auto EquipmentUse = (void(__cdecl*)(int unitObjectIndex, int slotIndex, unsigned int isClient))(0x00B4CE90);

		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return;

		auto equipmentObjectIndex = Pointer(unitObject)(0x2F0 + 4 * slotIndex).Read<uint32_t>();
		if (equipmentObjectIndex == -1)
			return;

		if (equipment_get_remaining_uses(equipmentObjectIndex) == 0 || sub_B87DA0(equipmentObjectIndex) > 0.0f)
			return;

		EquipmentUse(unitObjectIndex, slotIndex, isClient);
	}

	void __cdecl UpdateEquipmentActionState(uint32_t playerIndex, uint32_t itemObjectIndex, uint32_t objectType, void* pControlGlobalActionState)
	{

		auto itemObject = Blam::Objects::Get(itemObjectIndex);
		if (!itemObject)
			return;
		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return;

		// ignore grenades and powerups
		static auto Equipment_GetEquipmentType = (signed int(__cdecl*) (unsigned __int32 eqipTagIndex, int a2))(0x00BA0260);
		auto equipType = Equipment_GetEquipmentType(itemObject->TagIndex, 0);
		if (equipType <= 1)
			return;

		auto unitObjectIndex = Pointer(player)(0x30).Read<uint32_t>();
		if (unitObjectIndex == -1)
			return;

		auto unitObject = Blam::Objects::Get(unitObjectIndex);

		auto unitRadius = Pointer(unitObject)(0x2c).Read<float>();
		auto testRadius = unitRadius + 0.1f;

		auto& itemPosition = Pointer(itemObject)(0x20).Read<RealVector3D>();
		auto& unitPosition = Pointer(unitObject)(0x20).Read<RealVector3D>();

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();

		if (primaryEquipmentObjectIndex == -1)
			return;

		auto primaryEquipmentObject = Blam::Objects::Get(primaryEquipmentObjectIndex);
		if (!primaryEquipmentObject)
			return;

		if (sub_B87DA0(primaryEquipmentObjectIndex) > 0)
			return;

		// if the equipment under us is not the same tag as the equipment we're holding
		if (primaryEquipmentObject->TagIndex == itemObject->TagIndex)
			return;

		auto itemDef = Blam::Tags::TagInstance(itemObject->TagIndex).GetDefinition<Blam::Tags::Items::Item>();
		if (itemDef->SwapMessage == -1)
			return;

		auto dx = itemPosition.I - unitPosition.I;
		auto dy = itemPosition.J - unitPosition.J;
		auto dz = itemPosition.K - unitPosition.K;

		if ((dx*dx + dy*dy + dz*dz) <= testRadius*testRadius)
		{
			// sets up the swap action
			Pointer p(pControlGlobalActionState);
			p.WriteFast<uint8_t>(2);
			p(0x4).WriteFast<uint32_t>(itemObjectIndex);
		}
	}

	char __cdecl Unit_EquipmentDetachHook(uint32_t unitObjectIndex, uint32_t equipmentObjectIndex, int a3)
	{
		auto sub_B40A70 = (char(__cdecl*)(unsigned __int32 a1, unsigned __int32 a2, signed int a3))(0xB40A70);

		// we only want to do this if we're the host. Otherwise, it'll create a duplicate object on the client
		if (!IsClient())
		{
			// passing -1 as the last argument tells the engine to drop the equipment into the world rather than dispose of it
			// this is needed in order for the unit to drop equipment on death and when swapped	
			if (equipment_get_remaining_uses(equipmentObjectIndex) == 0)
				return sub_B40A70(unitObjectIndex, equipmentObjectIndex, 1);
			else
				return sub_B40A70(unitObjectIndex, equipmentObjectIndex, -1);
		}

		return sub_B40A70(unitObjectIndex, equipmentObjectIndex, a3);
	}

	int GetMPGlobalsEquipmentBlockIndex(uint32_t tagIndex)
	{
		static std::unordered_map<uint32_t, int> s_CachedLookup;

		auto it = s_CachedLookup.find(tagIndex);
		if (it != s_CachedLookup.end())
			return it->second;

		auto matchGlobalsDefinition = *(uint8_t**)0x022AAEB8;

		auto mulgTagIndex = *(uint32_t*)(matchGlobalsDefinition + 0x1BC);
		if (mulgTagIndex == -1)
			return 0;

		auto mulgDefinition = Blam::Tags::TagInstance(mulgTagIndex).GetDefinition<Blam::Tags::Game::MultiplayerGlobals>();
		if (!mulgDefinition)
			return 0;

		if (mulgDefinition->Universal.Count == 0 || mulgDefinition->Universal->Equipment.Count == 0)
			return 0;


		for (auto i = 1; i < mulgDefinition->Universal->Equipment.Count; i++)
		{
			const auto& equipmentElem = mulgDefinition->Universal->Equipment[i];
			const auto& equipmentRef = equipmentElem.Equipment;

			if (equipmentRef.TagIndex == tagIndex)
			{
				s_CachedLookup[tagIndex] = i;
				return i;
			}
		}

		return 1;
	}


	void* __cdecl Player_GetArmorAbilitiesCHUDHook(Blam::Players::PlayerDatum* playerDatum)
	{
		static uint8_t data[8];
		memset(data, 0, sizeof(data));

		auto unitObject = Blam::Objects::Get(playerDatum->SlaveUnit);
		if (!unitObject)
			return data;

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();

		if (primaryEquipmentObjectIndex == -1)
			return data;

		auto itemObject = Blam::Objects::Get(primaryEquipmentObjectIndex);
		if (!itemObject)
			return data;

		// set the first armor ability slot to the correct equipment
		data[3] = GetMPGlobalsEquipmentBlockIndex(itemObject->TagIndex);

		return data;
	}

	__declspec(naked) void EquipmentActionStateHook()
	{
		__asm
		{
			push[ebp + 0ch]
			push[ebp - 04h]
			push[ebp - 0ch]
			push[ebp + 08h]
			call UpdateEquipmentActionState
			add esp, 10h
			mov edx, [ebp - 04h]
			mov ebx, 00539DB7h
			jmp  ebx
		}
	}

	__declspec(naked) void EquipmentPickupHook()
	{
		__asm
		{
			cmp eax, 2
			jb POWERUPS
			push edi // item object index
			push[ebp + 8] // player index
			call DoPickup
			mov edx, 0x005397D5
			jmp edx
			POWERUPS :
			mov edx, 0x0053988E
				jmp edx
		}
	}

	void __stdcall DespawnEquipment(uint32_t objectIndex, void* object)
	{
		auto tagIndex = *(uint32_t*)object;

		static auto Equipment_GetEquipmentType = (signed int(__cdecl*) (uint32_t eqipTagIndex, int a2))(0x00BA0260);
		static auto Object_Wake = (void(__cdecl *)(uint32_t objectIndex, bool a2))(0x00B327F0);
		static auto Object_Despawn = (void(__cdecl *)(uint32_t objectIndex))(0x00B2CD10);

		auto equipType = Equipment_GetEquipmentType(tagIndex, 0);

		// only despawn grenades
		if (equipType > 1)
			return;

		Object_Wake(objectIndex, 0);
		Object_Despawn(objectIndex);
	}

	__declspec(naked) void DespawnEquipmentHook()
	{
		__asm
		{
			lea eax, [ebp - 0x40]
			push[eax] // object
			push[eax + 0x14] // object index
			call DespawnEquipment
			mov eax, 0x54EF97
			jmp eax
		}
	}

	void __cdecl UnitDeathHook(int unitObjectIndex, int a2, int a3)
	{
		using namespace Blam::Tags;

		struct GrenadeBlock
		{
			int16_t MaximumCount;
			uint16_t Unknown02;
			TagReference ThrowingEffect;
			uint32_t Unknown14;
			uint32_t Unknown18;
			uint32_t Unknown1C;
			uint32_t Unknown20;
			TagReference Equipment;
			TagReference Projectile;
		};

		const auto UnitDeath = (void(__cdecl *)(int unitObjectIndex, char a2, char a3))(0xB40410);
		UnitDeath(unitObjectIndex, a2, a3);

		// only continue if we're host
		static auto IsClient = (bool(*)())(0x00531D70);
		if (IsClient())
			return;

		auto objects = Blam::Objects::GetObjects();
		auto unitObjectDatum = objects.Get(unitObjectIndex);
		if (!unitObjectDatum || !unitObjectDatum->Data || unitObjectDatum->Type != Blam::Objects::eObjectTypeBiped)
			return;

		auto unitObjectPtr = Pointer(unitObjectDatum->Data);
		auto& unitPosition = unitObjectPtr(0x20).Read<RealVector3D>();

		auto matchGlobalsPtr = Pointer(0x022AAEB8)[0];
		if (!matchGlobalsPtr)
			return;

		auto grenadeBlock = matchGlobalsPtr(0x12c).Read<TagBlock<GrenadeBlock>>();

		for (auto i = 0; i < 4; i++)
		{
			auto& blockElem = grenadeBlock.Elements[i];
			if (blockElem.Equipment.TagIndex == -1)
				continue;

			auto nadeCount = unitObjectPtr(0x320 + i).Read<uint8_t>();

			uint8_t objectData[0x18Cu];

			// spawn a new grenade object for each grenade the unit had
			for (auto j = 0; j < nadeCount; j++)
			{
				static auto Objects_InitializeNewObject = (void(__cdecl *)(void* objectData, int tagIndex, int objectIndex, int a4))(0x00B31590);
				static auto Objects_SpawnObject = (uint32_t(__cdecl*)(void* objectData))(0x00B30440);
				static auto Simulation_SpawnObject = (char(__cdecl *)(unsigned __int32 unitObjectIndex))(0x4B2CD0);
				static auto ItemDrop = (void(__cdecl*)(uint32_t unitObjectIndex, uint32_t itemObjectIndex, int a3, float a4, int a5))(0xB49580);

				Objects_InitializeNewObject(objectData, blockElem.Equipment.TagIndex, unitObjectIndex, 0);

				Pointer(objectData)(0x1c).WriteFast(unitPosition);

				auto grenadeObjectIndex = Objects_SpawnObject(objectData);
				uint8_t *grenadeObject = (uint8_t*)Blam::Objects::Get(grenadeObjectIndex);
				if (grenadeObject)
				{
					*(uint32_t*)(grenadeObject + 0x180) = Blam::Time::GetGameTicks();
					*(uint8_t*)(grenadeObject + 0x178) &= 0xFDu;
					*(uint8_t*)(grenadeObject + 0x179) = 0;
					*(uint32_t*)(grenadeObject + 0x184) = -1;
				}

				ItemDrop(unitObjectIndex, grenadeObjectIndex, 0, 1.0f, 1);

				// notify clients that an object has been spawned
				Simulation_SpawnObject(grenadeObjectIndex);
			}
		}
	}

	void OvershieldDecay(uint32_t unitObjectIndex)
	{
		const auto sub_4B1E10 = (void(__thiscall *)(void *thisptr, int objectIndex, unsigned int a3))(0x4B1E10);
		const auto sub_4B3010 = (void(*)(int unitObjectIndex, void *eventData))(0x4B3010);

		const auto DECAY_RATE = 0.025f;

		auto unitObject = (uint8_t*)Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return;

		*(float*)(unitObject + 0x100) -= DECAY_RATE * Blam::Time::GetSecondsPerTick();

		// simulation
		uint8_t unknown[0x8] = { 0 };
		sub_4B1E10(unknown, unitObjectIndex, 7u);
		sub_4B3010(unitObjectIndex, unknown);
	}

	__declspec(naked) void OvershieldDecayHook()
	{
		__asm
		{
			push ebx // unit object index
			call OvershieldDecay
			add esp, 4
			push 0x00B56462
			retn
		}
	}

	void VisionEnd(uint32_t unitObjectIndex, uint32_t visionScreenEffectTagIndex)
	{
		struct s_rasterizer_screen_effect : Blam::DatumBase
		{
			uint16_t field_2;
			uint32_t tag_index;
			float seconds_active;
			RealVector3D position;
			uint32_t object_index;
			uint32_t field_1c;
			RealVector3D field_20;
			RealVector3D field_2c;
			uint32_t field_38;
		};
		static_assert(sizeof(s_rasterizer_screen_effect) == 0x3C, "s_rasterizer_screen_effect invalid");

		auto screenEffects = ElDorito::GetMainTls(0x338).Read<Blam::DataArray<s_rasterizer_screen_effect>*>();
		for (auto it = screenEffects->begin(); it != screenEffects->end(); ++it)
		{
			if (it->object_index == unitObjectIndex && it->tag_index == visionScreenEffectTagIndex)
				screenEffects->Delete(it.CurrentDatumIndex);
		}
	}

	__declspec(naked) void VisionEndHook()
	{
		__asm
		{
			push ebp
			mov ebp, esp
			pushad
			push [ecx + 0xC]
			push esi
			call VisionEnd
			add esp, 8
			mov esp, ebp
			pop ebp
			retn
		}
	}

	__declspec(naked) void VisionEndHook2()
	{
		__asm
		{
			push ebp
			mov ebp, esp
			pushad
			push [esi + 0xC]
			push edi
			call VisionEnd
			add esp, 8
			mov esp, ebp
			pop ebp
			retn
		}
	}
}
