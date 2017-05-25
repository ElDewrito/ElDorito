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
	void EquipmentActivationHook();
	void EquipmentActionStateHook();
	char __cdecl Unit_EquipmentDetachHook(uint32_t unitObjectIndex, uint32_t equipmentObjectIndex, int a3);
	void* __cdecl Player_GetArmorAbilitiesCHUDHook(Blam::Players::PlayerDatum* playerDatum);
	bool __cdecl UnitUpdateHook(uint32_t unitObjectIndex);
	void __cdecl EquipmentUseHook(int unitObjectIndex, int slotIndex, unsigned int isClient);
	void __cdecl UnitDeathDropHook(int unitObjectIndex, int eventType);
	void DespawnEquipmentHook();
}

namespace Patches
{
	namespace Equipment
	{
		void ApplyAll()
		{
			// implements the missing functionality in order to pickup equipment
			Hook(0x139888, EquipmentPickupHook, HookFlags::IsJmpIfNotEqual).Apply();

			// this is needed for object creationional equipment, as we cannot free the equipment slot until-
			// after the initial animation, or it will cut off abruptly.
			Hook(0x7A37F9, EquipmentActivationHook, HookFlags::None).Apply();

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

			// stop new equipment from destroying the old one
			Patch(0x78A179, { 0xEB }).Apply();
			Patch(0x78950D, { 0xEB }).Apply();

			// mid-function jump table hook for equipment.
			// this gets executed for each equipment object returned from a cluster search radius-
			// and is used in order to populate the control globals action structure with the information needed for swapping
			DWORD oldProtect, tmp;
			auto pEquipmentJumpTableCase = LPVOID(0x00539E08 + 3 * 4);
			VirtualProtect(pEquipmentJumpTableCase, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
			*(uintptr_t*)pEquipmentJumpTableCase = (uintptr_t)&EquipmentActionStateHook;
			VirtualProtect(pEquipmentJumpTableCase, 4, oldProtect, &tmp);

			// drop grenades on unit death
			Hook(0x74C746, UnitDeathDropHook, HookFlags::IsCall).Apply();
			Hook(0x14EE29, DespawnEquipmentHook, HookFlags::IsJmpIfNotEqual).Apply();
		}
	}
}

namespace
{
	using ObjectArray = Blam::DataArray<Blam::Objects::ObjectHeader>;
	using PlayerArray = Blam::DataArray<Blam::Players::PlayerDatum>;
	using Vector3D = Blam::Math::RealVector3D;

	inline ObjectArray* GetObjects()
	{
		auto objectsPtr = (ObjectArray**)ElDorito::GetMainTls(0x448);
		return objectsPtr ? *objectsPtr : nullptr;
	}

	inline PlayerArray* GetPlayers()
	{
		auto playersPtr = (PlayerArray**)ElDorito::GetMainTls(0x40);
		return playersPtr ? *playersPtr : nullptr;
	}

	void __stdcall DoPickup(uint16_t playerIndex, uint32_t objectIndex)
	{
		struct PickupData
		{
			uint32_t Type;
			uint32_t ObjectIndex;
			uint32_t Unknown08;
			uint8_t Unknown0C[0x3C];
		};

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
		//pickupdata.Unknown08 = 3;

		auto playerArray = GetPlayers();
		auto objectArray = GetObjects();
		assert(objectArray);
		assert(playerArray);

		auto playerDatum = (Blam::Players::PlayerDatum*)Pointer(playerArray->Data)(playerIndex * playerArray->DatumSize);
		if (!playerDatum)
			return;

		auto playerUnitObjectIndex = playerDatum->SlaveUnit;
		auto unitObjectDatum = objectArray->Get(playerUnitObjectIndex);

		if (playerUnitObjectIndex == Blam::DatumIndex::Null || !unitObjectDatum || !unitObjectDatum->Data)
			return;

		auto unitObject = unitObjectDatum->Data;

		auto equipmentObjectDatum = objectArray->Get(objectIndex);
		if (!equipmentObjectDatum || !unitObjectDatum->Data)
			return;

		auto equipmentTagIndex = Pointer(equipmentObjectDatum->Data).Read<uint32_t>();

		// return if the unit already holds equipment
		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();
		if (primaryEquipmentObjectIndex != -1)
			return;


		static auto IsClient = (bool(*)())(0x00531D70);

		if (!IsClient())
		{
			HUDIterator hudIterator(playerIndex);
			while (hudIterator.Advance())
			{
				static auto DisplayPickupMessage = (int(__cdecl*)(uint32_t hudIndex, uint32_t eqipTagIndex))(0xA95850);
				DisplayPickupMessage(hudIterator.HudIndex, equipmentTagIndex);
			}

			static auto Equipment_PlayPickupSound = (int(__cdecl *)(uint16_t playerIndex, uint32_t eqipTagIndex))(0xB887B0);
			Equipment_PlayPickupSound(playerIndex, equipmentTagIndex);

			static auto sub_4B31C0 = (void(_cdecl *)(uint32_t unitObjectIndex, uint32_t eqipTagIndex))(0x4B31C0);
			sub_4B31C0(playerUnitObjectIndex, equipmentTagIndex);
		}

		if (!IsClient())
		{
			static auto Unit_Pickup = (char(__cdecl *)(uint32_t unitIndex, const PickupData *pickupData))(0xB69C50);
			Unit_Pickup(playerUnitObjectIndex, &pickupdata);
		}
	}

	void __stdcall CleanupEquipment(uint32_t unitObjectIndex)
	{
		auto objects = GetObjects();
		assert(objects);

		auto unitObjectDatum = objects->Get(unitObjectIndex);
		assert(unitObjectDatum);

		auto unitObject = Pointer(unitObjectDatum)[0xC];
		assert(unitObject);

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();

		if (primaryEquipmentObjectIndex != -1)
		{
			// essentially a copy of sub_B40A70 with a few tweaks in order to dispose of the equipment correctly
			static auto sub_B40A70 = (int(__cdecl *)(int a1, int a2, int a3))(0xB40A70);
			auto sub_4B1E70 = (int(__thiscall *)(void* thisptr, int unitIndex, int a3))(0x4B1E70);
			auto sub_4B3010 = (int(__cdecl *)(int objectIndex, void* a2))(0x4B3010);

			sub_B40A70(unitObjectIndex, primaryEquipmentObjectIndex, 1);
			unitObject(0x2F0).Write<uint32_t>(-1);
			unitObject(0x310).Write<uint32_t>(-1);

			uint8_t unknown[8] = { 0 };
			sub_4B1E70(unknown, unitObjectIndex, 0x1Cu);
			sub_4B3010(unitObjectIndex, unknown);
		}
	}

	bool __cdecl UnitUpdateHook(uint32_t unitObjectIndex)
	{
		static auto UnitUpdate = (bool(__cdecl*)(uint32_t unitObjectIndex))(0xB4C6D0);

		auto ret = UnitUpdate(unitObjectIndex);

		auto objects = GetObjects();
		if (!objects)
			return ret;

		auto unitObjectDatum = objects->Get(unitObjectIndex);
		assert(unitObjectDatum);

		auto unitObject = unitObjectDatum->Data;

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();
		if (primaryEquipmentObjectIndex == -1)
			return ret;

		auto equipmentObjectDatum = objects->Get(primaryEquipmentObjectIndex);
		if (!equipmentObjectDatum)
			return ret;

		auto equipmentObjectPtr = Pointer(equipmentObjectDatum)[0xC];
		auto equipmentDefinitionPtr = Pointer(Blam::Tags::TagInstance(equipmentObjectPtr.Read<uint32_t>()).GetDefinition<void>());


		auto maxUseDuration = equipmentDefinitionPtr(0x1D4).Read<float>();
		auto maxNumberOfUses = equipmentDefinitionPtr(0x1DC).Read<int16_t>();

		auto lastUsedTicks = equipmentObjectPtr(0x194).Read<int32_t>();
		auto numberOfUses = equipmentObjectPtr(0x198).Read<int32_t>();

		auto deltaTime = Blam::Time::TicksToSeconds(static_cast<float>(Blam::Time::GetGameTicks() - lastUsedTicks));
		if (maxUseDuration > 0 && lastUsedTicks > 0 && deltaTime >= maxUseDuration)
		{
			if (maxNumberOfUses <= 1 || numberOfUses >= maxNumberOfUses)
			{
				CleanupEquipment(unitObjectIndex);
			}
		}

		return ret;
	}

	void __cdecl EquipmentUseHook(int unitObjectIndex, int slotIndex, unsigned int isClient)
	{
		auto EquipmentUse = (void(__cdecl*)(int unitObjectIndex, int slotIndex, unsigned int isClient))(0x00B4CE90);

		auto objects = GetObjects();
		assert(objects);
		auto unitObjectDatum = objects->Get(unitObjectIndex);
		assert(unitObjectDatum);

		auto unitObject = unitObjectDatum->Data;

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();
		if (primaryEquipmentObjectIndex == -1)
			return;

		auto equipmentObjectDatum = objects->Get(primaryEquipmentObjectIndex);
		if (!equipmentObjectDatum)
			return;

		auto equipmentObjectPtr = Pointer(equipmentObjectDatum)[0xC];
		auto equipmentDefinitionPtr = Pointer(Blam::Tags::TagInstance(equipmentObjectPtr.Read<uint32_t>()).GetDefinition<void>());

		auto maxUseDuration = equipmentDefinitionPtr(0x1D4).Read<float>();
		auto maxNumberOfUses = equipmentDefinitionPtr(0x1DC).Read<int16_t>();

		auto lastUsedTicks = equipmentObjectPtr(0x194).Read<int32_t>();
		auto numberOfUses = equipmentObjectPtr(0x198).Read<int32_t>();

		auto deltaTime = Blam::Time::TicksToSeconds(static_cast<float>(Blam::Time::GetGameTicks() - lastUsedTicks));
		if (maxUseDuration > 0 && lastUsedTicks > 0 && deltaTime < maxUseDuration)
		{
			if (maxNumberOfUses <= 1 || numberOfUses >= maxNumberOfUses)
			{
				// prevent use
				return;
			}
		}

		EquipmentUse(unitObjectIndex, slotIndex, isClient);
	}

	void __cdecl UpdateEquipmentActionState(uint32_t playerIndex, uint32_t objectIndex, uint32_t objectType, void* pControlGlobalActionState)
	{
		auto objects = GetObjects();

		auto itemObjectDatum = objects->Get(objectIndex);
		auto itemObject = itemObjectDatum->Data;
		if (!itemObjectDatum || !itemObject)
			return;

		auto players = GetPlayers();
		auto player = players->Get(playerIndex);
		if (!player)
			return;

		auto itemTagIndex = Pointer(itemObject).Read<uint32_t>();

		// ignore grenades and powerups
		static auto Equipment_GetEquipmentType = (signed int(__cdecl*) (unsigned __int32 eqipTagIndex, int a2))(0x00BA0260);
		auto equipType = Equipment_GetEquipmentType(itemTagIndex, 0);
		if (equipType <= 1)
			return;

		auto unitObjectIndex = Pointer(player)(0x30).Read<uint32_t>();
		if (unitObjectIndex == -1)
			return;

		auto unitObjectDatum = objects->Get(unitObjectIndex);
		auto unitObject = unitObjectDatum->Data;

		auto unitRadius = Pointer(unitObject)(0x2c).Read<float>();
		auto testRadius = unitRadius + 0.1f;

		auto& itemPosition = Pointer(itemObject)(0x20).Read<Vector3D>();
		auto& unitPosition = Pointer(unitObject)(0x20).Read<Vector3D>();

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();

		if (primaryEquipmentObjectIndex == -1)
			return;

		auto primaryEquipmentObjectDatum = objects->Get(primaryEquipmentObjectIndex);
		if (!primaryEquipmentObjectDatum || !primaryEquipmentObjectDatum->Data)
			return;

		auto primaryEquipmentTagIndex = Pointer(primaryEquipmentObjectDatum->Data).Read<uint32_t>();

		// if the equipment under us is not the same tag as the equipment we're holding
		if (primaryEquipmentTagIndex == itemTagIndex)
			return;

		auto dx = itemPosition.I - unitPosition.I;
		auto dy = itemPosition.J - unitPosition.J;
		auto dz = itemPosition.K - unitPosition.K;

		if ((dx*dx + dy*dy + dz*dz) <= testRadius*testRadius)
		{
			// sets up the swap action
			Pointer p(pControlGlobalActionState);
			p.Write<uint8_t>(2);
			p(0x4).Write<uint32_t>(objectIndex);
		}
	}

	char __cdecl Unit_EquipmentDetachHook(uint32_t unitObjectIndex, uint32_t equipmentObjectIndex, int a3)
	{
		auto sub_B40A70 = (char(__cdecl*)(unsigned __int32 a1, unsigned __int32 a2, signed int a3))(0xB40A70);

		static auto IsClient = (bool(*)())(0x00531D70);

		// we only want to do this if we're the host. Otherwise, it'll create a duplicate object on the client
		if (!IsClient())
		{
			// passing -1 as the last argument tells the engine to drop the equipment into the world rather than dispose of it
			// this is needed in order for the unit to drop equipment on death and when swapped	
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

		auto objects = GetObjects();
		if (!objects || playerDatum->SlaveUnit == Blam::DatumIndex::Null)
			return data;

		auto unitObjectDatum = objects->Get(playerDatum->SlaveUnit);
		auto unitObject = unitObjectDatum->Data;

		if (!unitObjectDatum || !unitObject)
			return data;

		auto primaryEquipmentObjectIndex = Pointer(unitObject)(0x2F0).Read<uint32_t>();

		if (primaryEquipmentObjectIndex == -1)
			return data;

		auto itemObjectDatum = objects->Get(primaryEquipmentObjectIndex);
		if (!itemObjectDatum)
			return data;

		auto itemTagIndex = Pointer(itemObjectDatum->Data).Read<uint32_t>();

		// set the first armor ability slot to the correct equipment
		data[3] = GetMPGlobalsEquipmentBlockIndex(itemTagIndex);

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

	__declspec(naked) void EquipmentActivationHook()
	{
		__asm
		{
			push[ebp + 8] // unit object
			call CleanupEquipment
			mov eax, 0x00B2CD10
			call eax
			mov eax, 0x00BA37FE
			jmp eax
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

	void DespawnEquipment(uint32_t objectIndex, void* object)
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
			lea eax, [ebp-0x40]
			push [eax] // object
			push [eax+0x14] // object index
			call DespawnEquipment
			add esp, 8
			mov eax, 0x0054EF6D
			jmp eax
		}
	}

	void __cdecl UnitDeathDropHook(int unitObjectIndex, int eventType)
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

		static auto UnitDeathDrop = (void(__cdecl*)(uint32_t unitObjectIndex, int a2))(0x00B69CF0);
		UnitDeathDrop(unitObjectIndex, eventType);
	
		// only continue if we're host
		static auto IsClient = (bool(*)())(0x00531D70);
		if (IsClient())
			return;

		auto objects = GetObjects();
		assert(objects);

		auto unitObjectDatum = objects->Get(unitObjectIndex);
		if (!unitObjectDatum || !unitObjectDatum->Data || unitObjectDatum->Type != Blam::Objects::eObjectTypeBiped)
			return;

		auto unitObjectPtr = Pointer(unitObjectDatum->Data);
		auto& unitPosition = unitObjectPtr(0x20).Read<Vector3D>();

		auto matchGlobalsPtr = Pointer(0x022AAEB8)[0];
		assert(matchGlobalsPtr);

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
				static auto Objects_InitializeNewObject = (void (__cdecl *)(void* objectData, int tagIndex, int objectIndex, int a4))(0x00B31590);
				static auto Objects_SpawnObject = (uint32_t(__cdecl*)(void* objectData))(0x00B30440);
				static auto Simulation_SpawnObject = (char(__cdecl *)(unsigned __int32 unitObjectIndex))(0x4B2CD0);

				Objects_InitializeNewObject(objectData, blockElem.Equipment.TagIndex, unitObjectIndex, 0);

				// TODO: also take into account the unit's velocity
				Pointer(objectData)(0x1c).Write(unitPosition);

				auto grenadeObjectIndex = Objects_SpawnObject(objectData);
				auto grenadeObjectDatum = objects->Get(grenadeObjectIndex);
				if (grenadeObjectDatum && grenadeObjectDatum->Data)
				{
					auto grenadeObject = (uint8_t*)grenadeObjectDatum->Data;
					*(uint32_t*)(grenadeObject + 0x180) = Blam::Time::GetGameTicks();
					*(uint8_t*)(grenadeObject + 0x178) &= 0xFDu;
					*(uint8_t*)(grenadeObject + 0x179) = 0;
					*(uint32_t*)(grenadeObject + 0x184) = -1;
				}

				// notify clients that an object has been spawned
				Simulation_SpawnObject(grenadeObjectIndex);
			}
		}
	}
}
