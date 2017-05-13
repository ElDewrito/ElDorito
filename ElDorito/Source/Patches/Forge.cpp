#include "Forge.hpp"

#include "../Patch.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../ElDorito.hpp"
#include "Core.hpp"

using namespace Blam;
using namespace Blam::Objects;

namespace
{
	bool shouldDelete = false;

	bool barriersEnabledValid = false;
	bool killBarriersEnabled = true;
	bool pushBarriersEnabled = true;

	void UpdateForgeInputHook();
	void UpdateBarriersEnabled();
	bool CheckKillTriggersHook(int a0, void *a1);
	bool ObjectSafeZoneHook(void *a0);
	void* PushBarriersGetStructureDesignHook(int index);
	void FixRespawnZones();
}

namespace Patches
{
	namespace Forge
	{
		void ApplyAll()
		{
			Hook(0x19D482, UpdateForgeInputHook, HookFlags::IsCall).Apply();
			Hook(0x771C7D, CheckKillTriggersHook, HookFlags::IsCall).Apply();
			Hook(0x7B4C32, CheckKillTriggersHook, HookFlags::IsCall).Apply();
			Hook(0x19EBA1, ObjectSafeZoneHook, HookFlags::IsCall).Apply();
			Hook(0x19FDBE, ObjectSafeZoneHook, HookFlags::IsCall).Apply();
			Hook(0x19FEEC, ObjectSafeZoneHook, HookFlags::IsCall).Apply();
			Hook(0x32663D, ObjectSafeZoneHook, HookFlags::IsCall).Apply();
			Hook(0x2749D1, PushBarriersGetStructureDesignHook, HookFlags::IsCall).Apply();
			Hook(0x274DBA, PushBarriersGetStructureDesignHook, HookFlags::IsCall).Apply();
			Hook(0x2750F8, PushBarriersGetStructureDesignHook, HookFlags::IsCall).Apply();
			Hook(0x275655, PushBarriersGetStructureDesignHook, HookFlags::IsCall).Apply();

			// enable teleporter volume editing compliments of zedd
			Patch::NopFill(Pointer::Base(0x6E4796), 0x66);

			Patches::Core::OnGameStart(FixRespawnZones);
		}

		void Tick()
		{
			// Require a rescan for barrier disabler objects each tick
			barriersEnabledValid = false;
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

	void UpdateBarriersEnabled()
	{
		if (barriersEnabledValid)
			return; // Don't scan multiple times per tick

		// Scan the object table to check if the barrier disablers are spawned
		auto objectHeadersPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset);
		auto objectHeaders = objectHeadersPtr.Read<const DataArray<ObjectHeader>*>();
		if (!objectHeaders)
			return;
		killBarriersEnabled = true;
		pushBarriersEnabled = true;
		for (auto &&header : *objectHeaders)
		{
			// The objects are identified by tag index.
			// scen 0x5728 disables kill barriers
			// scen 0x5729 disables push barriers
			if (header.Type != eObjectTypeScenery)
				continue;
			auto tagIndex = header.GetTagIndex().Index();
			if (tagIndex == 0x5728)
				killBarriersEnabled = false;
			else if (tagIndex == 0x5729)
				pushBarriersEnabled = false;
			if (!killBarriersEnabled && !pushBarriersEnabled)
				break;
		}
		barriersEnabledValid = true;
	}

	bool CheckKillTriggersHook(int a0, void *a1)
	{
		UpdateBarriersEnabled();
		if (!killBarriersEnabled)
			return false;

		typedef bool(*CheckKillTriggersPtr)(int a0, void *a1);
		auto CheckKillTriggers = reinterpret_cast<CheckKillTriggersPtr>(0x68C410);
		return CheckKillTriggers(a0, a1);
	}

	bool ObjectSafeZoneHook(void *a0)
	{
		UpdateBarriersEnabled();
		if (!killBarriersEnabled)
			return true;

		typedef bool(*CheckSafeZonesPtr)(void *a0);
		auto CheckSafeZones = reinterpret_cast<CheckSafeZonesPtr>(0x4EB130);
		return CheckSafeZones(a0);
	}

	void* PushBarriersGetStructureDesignHook(int index)
	{
		UpdateBarriersEnabled();
		if (!pushBarriersEnabled)
			return nullptr; // Return a null sddt if push barriers are disabled

		typedef void*(*GetStructureDesignPtr)(int index);
		auto GetStructureDesign = reinterpret_cast<GetStructureDesignPtr>(0x4E97D0);
		return GetStructureDesign(index);
	}

	void FixRespawnZones()
	{
		using Vector3 = Blam::Math::RealVector3D;

		// TODO: Move somewhere more appropriate for re-use
		struct ZoneShape
		{
			int32_t Shape;
			float Width;
			float Depth;
			float Top;
			float Bottom;
			float MatrixScale4x3[13];
			float BoundingRadius;
			uint32_t Unknown4C;
		};

		static_assert(sizeof(ZoneShape) == 0x50, "Invalid ZoneShape size");


		static auto GetObjectZoneShape = (void(__cdecl *)(uint32_t objectIndex, ZoneShape* zoneShape, int a3))(0xBA0AD0);
		static auto ZoneShapeIntersect = (bool(__cdecl*)(Vector3* point, ZoneShape* zone))(0x00BA11F0);

		auto objectsPtr = (Blam::DataArray<Blam::Objects::ObjectHeader>**)ElDorito::GetMainTls(0x448);
		if (!objectsPtr)
			return;

		auto& objects = *objectsPtr;

		static auto GetMapVariant = (Blam::MapVariant* (__cdecl*)())(0x00583230);
		auto mapv = GetMapVariant();
		if (!mapv)
			return;


		// loop throught mapv placements
		for (auto i = 0; i < mapv->UsedPlacementsCount; i++)
		{
			auto placement = mapv->Placements[i];
			if (!(placement.PlacementFlags & 1) || placement.ObjectIndex == -1)
				continue;

			auto& properties = placement.Properties;

			// Player Respawn Zone
			if (properties.ObjectType != 0xD)
				continue;

			auto zoneObjectDatum = objects->Get(placement.ObjectIndex);
			if (!zoneObjectDatum || !zoneObjectDatum->Data)
				continue;

			auto zoneObjectPtr = Pointer(zoneObjectDatum->Data);
			auto zoneTagIndex = zoneObjectPtr.Read<uint32_t>();


			ZoneShape zoneShape = { 0 };
			GetObjectZoneShape(placement.ObjectIndex, &zoneShape, 0);

			for (auto j = 0; j < mapv->UsedPlacementsCount; j++)
			{
				auto foundObjectIndex = mapv->Placements[j].ObjectIndex;

				if (foundObjectIndex == -1 || foundObjectIndex == placement.ObjectIndex)
					continue;

				auto foundObjectDatum = objects->Get(foundObjectIndex);
				if (!foundObjectDatum || !foundObjectDatum->Data)
					continue;

				auto foundObjectPtr = Pointer(foundObjectDatum->Data);

				Vector3* centerPoint = foundObjectPtr(0x20);
				auto tagIndex = foundObjectPtr.Read<uint32_t>();

				auto mpPropertiesOffset = foundObjectPtr(0x16c).Read<uint16_t>();
				if (mpPropertiesOffset == -1)
					continue;

				auto mpPropertiesPtr = foundObjectPtr(mpPropertiesOffset);

				// check if the object's center is inside the zone
				if (!ZoneShapeIntersect(centerPoint, &zoneShape))
					continue;

				auto mpObjectType = mpPropertiesPtr(0x2).Read<uint8_t>();
				switch (mpObjectType)
				{
				case 0xC:  // PlayerSpawnLocation:
				{
					// ignore invisible spawns
					if (tagIndex == 0x00002EA6)
						continue;

					auto flags = mpPropertiesPtr.Read<uint16_t>();

					// ignore intial spawns
					if (flags & (1 << 1))
						continue;
				}
				case 0x0E: // HoldSpawnObjective:
				case 0x0F: // CaptureSpawnObjective:
				case 0x10: // HoldDestinationObjective:
				case 0x11: // CaptureDestinationObjective:
				case 0x12: // HillObjective:
				case 0x13: // InfectionHavenObjective:
				case 0x14: // TerritoryObjective:
				case 0x15: // VIPBoundaryOeebjective:
				case 0x16: // VIPDestinationObjective:
				case 0x17: // JuggernautDestinationObjective:
					break;
				default:
					continue;
				}

				// set the team index to match the zone
				auto zoneTeamIndex = placement.Properties.ObjectFlags >> 8;
				mpPropertiesPtr(0xA).Write<uint8_t>(zoneTeamIndex);
			}
		}
	}
}