#include "Forge.hpp"

#include "../Patch.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Blam/Tags/TagBlock.hpp"
#include "../Blam/Math/RealQuaternion.hpp"
#include "../Blam/Math/RealMatrix4x3.hpp"
#include "../Blam/Math/MathUtil.hpp"
#include "../ElDorito.hpp"
#include "Core.hpp"
#include "../Forge/Prefab.hpp"
#include "../Forge/ObjectSet.hpp"
#include "../Forge/ForgeUtil.hpp"
#include "../Modules/ModuleForge.hpp"
#include <cassert>
#include <queue>
#include <bitset>
#include <stack>
#include <fstream>
#include "boost/filesystem.hpp"

using namespace Blam;
using namespace Blam::Objects;

namespace
{
	using namespace ::Forge;
	using RealVector3D = Blam::Math::RealVector3D;
	using RealQuaternion = Blam::Math::RealQuaternion;
	using RealMatrix4x3 = Blam::Math::RealMatrix4x3;

	const auto Objects_GetPosition = (void(*)(uint32_t objectIndex, RealVector3D *position))(0xB2E5A0);

	bool barriersEnabledValid = false;
	bool killBarriersEnabled = true;
	bool pushBarriersEnabled = true;

	void UpdateBarriersEnabled();
	bool CheckKillTriggersHook(int a0, void *a1);
	bool ObjectSafeZoneHook(void *a0);
	void* PushBarriersGetStructureDesignHook(int index);
	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot);
	void SpecialWeaponHUDHook(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objectsInCluster, int16_t objectcount, BYTE* activeSpecialChudTypes);
	void ObjectGrabbedHook(uint32_t playerIndex, uint16_t placementIndex);
	void ObjectDroppedHook(uint16_t placementIndex, float throwForce, int a3);
	void ObjectDeleteHook(uint16_t placementIndex, uint32_t playerIndex);
	void ObjectPropertiesChangeHook(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties);
	void UnitFlyingHook(uint32_t unitObjectIndex, int a2, int a3, int a4, int a5, int a6, int a7);
	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* rightVec, RealVector3D* upVec);

	void FixRespawnZones();

	void SandboxEngineTickHook();
	void __fastcall SandboxEngineObjectDisposeHook(void* thisptr, void* unused, uint32_t objectIndex);

	struct AABB { float MinX, MaxX, MinY, MaxY, MinZ, MaxZ; };
	bool CalculateObjectBoundingBox(DatumIndex objectTagIndex, AABB* outBoundingBox);
	DatumIndex GetObjectIndexUnderCrosshair(DatumIndex playerIndex);
	bool GetObjectUnderCrosshairIntersectNormal(DatumIndex playerIndex, Math::RealVector3D* outNormalVec);
	DatumIndex CloneObject(DatumIndex playerIndex, DatumIndex objectIndex, float depth);
	void CloneSelection(DatumIndex playerIndex);
	void DeletePlacementObject(DatumIndex playerIndex, uint16_t placementIndex);
	void UpdateRotationSnap(DatumIndex playerIndex, DatumIndex objectIndex);

	struct
	{
		uint32_t Flags;
		DatumIndex ObjectIndex;
		uint32_t StartTime;
		float Current;
		float InputXTicks;
		float InputYTicks;
		float InputZTicks;
		RealQuaternion StartRotation;
		RealQuaternion EndRotation;

	} s_RotationSnapState = { 0 };

	std::queue<std::function<void()>> s_SandboxTickCommandQueue;
	Forge::ObjectSet s_SelectedObjects;
	RealVector3D s_GrabOffset;
}

namespace Patches
{
	namespace Forge
	{
		void ApplyAll()
		{
			Pointer(0x0165AB54).Write<uint32_t>((uint32_t)&SandboxEngineTickHook);
			Pointer(0x0165AB94).Write((uint32_t)&SandboxEngineObjectDisposeHook);

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
			Hook(0x19FA69, RotateHeldObjectHook, HookFlags::IsCall).Apply();
			Hook(0x62E760, SpecialWeaponHUDHook, HookFlags::IsCall).Apply();
			Hook(0x7350A, ObjectGrabbedHook, HookFlags::IsCall).Apply();
			Hook(0x7356F, ObjectDroppedHook, HookFlags::IsCall).Apply();
			Hook(0x734FC, ObjectDeleteHook, HookFlags::IsCall).Apply();
			Hook(0x73527, ObjectPropertiesChangeHook, HookFlags::IsCall).Apply();
			Hook(0x7AF758, UnitFlyingHook, HookFlags::IsCall).Apply();
			Hook(0x19CAFC, UpdateHeldObjectTransformHook, HookFlags::IsCall).Apply();

			// enable teleporter volume editing compliments of zedd
			Patch::NopFill(Pointer::Base(0x6E4796), 0x66);

			// set max of 255 and cost of 0 for all forge objects
			Patch::NopFill(Pointer::Base(0x183CA3), 2); // weapons
			Patch::Patch(0x183C38, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();
			Patch::NopFill(Pointer::Base(0x183C23), 2);	// vehicles
			Patch::Patch(0x183CB8, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();
			Patch::NopFill(Pointer::Base(0x183D23), 2); // equipment
			Patch::Patch(0x183D38, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();
			Patch::NopFill(Pointer::Base(0x183DA3), 2); // scenery
			Patch::Patch(0x183DB8, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();
			Patch::NopFill(Pointer::Base(0x183E23), 2); // teleporters
			Patch::Patch(0x183E38, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();
			Patch::NopFill(Pointer::Base(0x183EA3), 2); // spawners
			Patch::Patch(0x183EB8, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();
			Patch::NopFill(Pointer::Base(0x183F23), 2); // goals
			Patch::Patch(0x183F38, { 0x0F, 0x57, 0xC0, 0x90, 0x90 }).Apply();

			Patches::Core::OnGameStart(FixRespawnZones);
		}

		void Tick()
		{
			// Require a rescan for barrier disabler objects each tick
			barriersEnabledValid = false;
		}

		void DeleteAll()
		{
			auto mapv = GetMapVariant();
			if (!mapv)
				return;

			auto playerIndex = Blam::Players::GetLocalPlayer(0);
			auto objectIndex = GetObjectIndexUnderCrosshair(playerIndex);

			auto& objects = ElDorito::GetMainTls(0x448).Read<DataArray<ObjectHeader>*>();
			auto objectDatum = objects->Get(objectIndex);
			if (!objectDatum || !objectDatum->Data)
				return;

			auto tagIndex = *(uint32_t*)objectDatum->Data;

			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!(placement.PlacementFlags & 1))
					continue;

				auto& budget = mapv->Budget[placement.BudgetIndex];
				if (budget.TagIndex != tagIndex)
					continue;

				DeletePlacementObject(playerIndex, i);
			}
		}

		void CanvasMap()
		{
			auto mapv = GetMapVariant();
			if (!mapv)
				return;

			auto playerIndex = Blam::Players::GetLocalPlayer(0);

			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!(placement.PlacementFlags & 1))
					continue;

				auto& budget = mapv->Budget[placement.BudgetIndex];
				if (budget.TagIndex == -1 || budget.Cost == -1)
					continue;

				DeletePlacementObject(playerIndex, i);
			}
		}

		void DeselectAll()
		{
			s_SelectedObjects.Clear();
		}

		void SelectAll()
		{
			auto playerIndex = Blam::Players::GetLocalPlayer(0);
			auto& objectUnderCrosshairPtr = Pointer(Blam::Objects::GetObjects().Get(GetObjectIndexUnderCrosshair(playerIndex)))[0xC];
			if (!objectUnderCrosshairPtr)
				return;

			const auto mapv = GetMapVariant();
			auto tagIndex = objectUnderCrosshairPtr.Read<uint32_t>();

			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (placement.ObjectIndex == -1 || placement.BudgetIndex == -1)
					continue;

				if (mapv->Budget[placement.BudgetIndex].TagIndex == tagIndex)
					s_SelectedObjects.Add(placement.ObjectIndex);
			}
		}

		bool SavePrefab(const std::string& name, const std::string& path)
		{
			return ::Forge::Prefabs::Save(s_SelectedObjects, name, path);
		}

		bool LoadPrefab(const std::string& path)
		{
			s_SandboxTickCommandQueue.push([=]()
			{
				::Forge::Prefabs::Load(s_SelectedObjects, path);
			});

			return true;
		}
	}
}

namespace
{
	using RealVector3D = Blam::Math::RealVector3D;

	void SandboxEngineTickHook()
	{
		static auto SandboxEngine_Tick = (void(*)())(0x0059ED70);
		static auto Forge_GetEditorModeState = (bool(__cdecl *)(uint32_t playerIndex, uint32_t* heldObjectIndex, uint32_t* objectIndexUnderCrosshair))(0x0059A6F0);

		SandboxEngine_Tick();

		while (!s_SandboxTickCommandQueue.empty())
		{
			auto cmd = s_SandboxTickCommandQueue.front();
			cmd();
			s_SandboxTickCommandQueue.pop();
		}

		auto activeScreenCount = Pointer(0x05260F34)[0](0x3c).Read<int16_t>();
		if (activeScreenCount == 0)
		{
			auto playerIndex = Blam::Players::GetLocalPlayer(0);
			if (playerIndex == DatumIndex::Null)
				return;

			uint32_t heldObjectIndex = -1, objectIndexUnderCrosshair = -1;
			if (Forge_GetEditorModeState(playerIndex, &heldObjectIndex, &objectIndexUnderCrosshair))
			{

				UpdateRotationSnap(playerIndex, heldObjectIndex);

				if (heldObjectIndex == -1)
				{
					if (Input::GetActionState(Blam::Input::eGameActionMelee)->Ticks == 1)
					{
						if (objectIndexUnderCrosshair != -1)
						{
							auto& forgeModule = Modules::ModuleForge::Instance();
							auto cloneDepth = forgeModule.VarCloneDepth->ValueFloat;
							auto cloneMultiplier = forgeModule.VarCloneMultiplier->ValueInt;

							auto objectIndexToClone = objectIndexUnderCrosshair;
							for (auto i = 0; i < cloneMultiplier; i++)
							{
								objectIndexToClone = CloneObject(playerIndex, objectIndexToClone, cloneDepth);
								if (objectIndexToClone == -1)
									break;
							}
						}
						else
						{
							if(s_SelectedObjects.Any())
								CloneSelection(playerIndex);
						}
					}

					static bool s_IsPaintSelecting = false;
		
					if (Input::GetActionState(Blam::Input::eGameActionFireRight)->Ticks > 0)
					{
						if (Input::GetActionState(Blam::Input::eGameActionFireRight)->Ticks == 1 && objectIndexUnderCrosshair == -1)
						{
							s_IsPaintSelecting = true;
						}

						if (objectIndexUnderCrosshair != -1 &&
							(Input::GetActionState(Blam::Input::eGameActionFireRight)->Ticks == 1 || s_IsPaintSelecting))
						{
							if (s_SelectedObjects.Contains(objectIndexUnderCrosshair) && !s_IsPaintSelecting)
								s_SelectedObjects.Remove(objectIndexUnderCrosshair);
							else
								s_SelectedObjects.Add(objectIndexUnderCrosshair);
						}
					}
					else
					{
						s_IsPaintSelecting = false;
					}
				}
			}
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


	void DeletePlacementObject(DatumIndex playerIndex, uint16_t placementIndex)
	{
		static auto Forge_DeleteObject = (void(__cdecl*)(int placementIndex, int playerIndex))(0x0059A920);
		Forge_DeleteObject(placementIndex, playerIndex);
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

		auto mapv = GetMapVariant();


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

	DatumIndex CloneObject(DatumIndex playerIndex, DatumIndex objectIndex, float depth)
	{
		using Vector3 = Math::RealVector3D;

		auto& objects = ElDorito::GetMainTls(0x448).Read<DataArray<ObjectHeader>*>();
		auto mapv = GetMapVariant();

		if (!mapv || playerIndex == DatumIndex::Null || objectIndex == DatumIndex::Null)
			return DatumIndex::Null;

		auto objectDatum = objects->Get(objectIndex);
		if (!objectDatum)
			return DatumIndex::Null;

		auto objectPtr = Pointer(objectDatum->Data);
		auto tagIndex = objectPtr.Read<uint32_t>();
		auto placementIndex = objectPtr(0x1c).Read<uint16_t>();
		auto position = objectPtr(0x54).Read<Vector3>();
		auto& rightVec = objectPtr(0x60).Read<Vector3>();
		auto& upVec = objectPtr(0x6c).Read<Vector3>();
		auto& forwardVec = Vector3::Normalize(Vector3::Cross(upVec, rightVec));
		auto& variantProperties = mapv->Placements[placementIndex].Properties;

		if (placementIndex == -1)
			return DatumIndex::Null;

		Vector3 intersectNormal;
		if (!GetObjectUnderCrosshairIntersectNormal(playerIndex, &intersectNormal))
			return DatumIndex::Null;

		static auto Matrix3x3_CreateAxes = (void(__cdecl*)(float *matrix, Vector3 *position, Vector3 *rightVec, Vector3 *upVec))(0x005B20C0);
		static auto Matrix3x3_TransformVec = (void(__cdecl*)(float *matrix, Vector3 *vec, Vector3 *outVector))(0x005B2710);

		float m0[13];
		Matrix3x3_CreateAxes(m0, &position, &rightVec, &upVec);
		Matrix3x3_TransformVec(m0, &intersectNormal, &intersectNormal);

		AABB bounds;
		if (!CalculateObjectBoundingBox(tagIndex, &bounds))
			return DatumIndex::Null;

		Vector3 displacement(0, 0, 0);

		if (abs(intersectNormal.K) > abs(intersectNormal.J) && abs(intersectNormal.K) > abs(intersectNormal.I))
		{
			auto dz = (bounds.MaxZ - bounds.MinZ);
			displacement = intersectNormal.K > 0 ? upVec * dz : upVec * -dz;
		}
		else if (abs(intersectNormal.J) > abs(intersectNormal.K) && abs(intersectNormal.J) > abs(intersectNormal.I))
		{
			auto dy = (bounds.MaxY - bounds.MinY);
			displacement = intersectNormal.J > 0 ? forwardVec * dy : forwardVec * -dy;
		}
		else if (abs(intersectNormal.I) > abs(intersectNormal.J) && abs(intersectNormal.I) > abs(intersectNormal.K))
		{
			auto dx = (bounds.MaxX - bounds.MinX);
			displacement = intersectNormal.I > 0 ? rightVec * dx : rightVec * -dx;
		}

		auto displacedPosition = position + displacement * depth;

		auto SpawnObject = (uint32_t(__thiscall *)(MapVariant* thisptr, uint32_t tagIndex, int a3, int16_t placementIndex, const Vector3 *position,
			const Vector3 *rightVec, const Vector3 *upVec, int16_t scnrPlacementBlockIndex, int objectType,
			MapVariant::VariantProperties* variantProperties, uint16_t placementFlags))(0x00582110);

		return SpawnObject(mapv, tagIndex, 0, -1, &displacedPosition, &rightVec, &upVec, -1, -1, &variantProperties, 0);
	}

	inline DatumIndex GetObjectIndexUnderCrosshair(DatumIndex playerIndex)
	{
		auto gameEngineGlobalsPtr = ElDorito::GetMainTls(0x48);
		if (!gameEngineGlobalsPtr)
			return DatumIndex::Null;

		return gameEngineGlobalsPtr[0](0xe45c + 4 * playerIndex.Index()).Read<DatumIndex>();
	}

	inline bool GetObjectUnderCrosshairIntersectNormal(DatumIndex playerIndex, Math::RealVector3D* outNormalVec)
	{
		auto gameEngineGlobalsPtr = ElDorito::GetMainTls(0x48);
		if (!gameEngineGlobalsPtr)
			return false;

		*outNormalVec = gameEngineGlobalsPtr[0](0xE2DC + 0xC * playerIndex.Index()).Read<Math::RealVector3D>();
		return true;
	}

	bool CalculateObjectBoundingBox(DatumIndex objectTagIndex, AABB* outBoundingBox)
	{
		using namespace Blam::Tags;

		auto objectDefPtr = Pointer(TagInstance(objectTagIndex.Index()).GetDefinition<void>());
		if (!objectDefPtr)
			return false;

		auto hlmtTagIndex = objectDefPtr(0x40).Read<uint32_t>();

		auto hlmtDefPtr = Pointer(TagInstance(hlmtTagIndex).GetDefinition<void>());
		if (!hlmtDefPtr)
			return false;

		auto modeTagIndex = hlmtDefPtr(0xC).Read<uint32_t>();

		auto modeDefPtr = Pointer(TagInstance(modeTagIndex).GetDefinition<void>());
		if (!modeDefPtr)
			return false;

		auto compressionInfoBlock = modeDefPtr(0x74).Read<Blam::Tags::TagBlock<uint8_t>>();
		if (compressionInfoBlock.Count < 1)
			return false;

		*outBoundingBox = Pointer(compressionInfoBlock.Elements)(0x4).Read<AABB>();

		return true;
	}

	RealVector3D GetClosestCardianalAxix(const RealVector3D& v)
	{
		auto xn = abs(v.I);
		auto yn = abs(v.J);
		auto zn = abs(v.K);


		if (xn >= yn && xn >= zn)
			return v.I > 0 ? RealVector3D(1, 0, 0) : RealVector3D(-1, 0, 0);
		else if (yn > xn && yn > zn)
			return v.J > 0 ? RealVector3D(0, 1, 0) : RealVector3D(0, -1, 0);
		else if (zn > xn && zn > yn)
			return v.K > 0 ? RealVector3D(0, 0, 1) : RealVector3D(0, 0, -1);

		return RealVector3D(0, 0, 0);
	}

	void RotateSnapped(uint32_t objectIndex, RealQuaternion rotation)
	{
		static auto Object_GetTransformationMatrix = (void(__cdecl*)(uint32_t objectIndex, RealMatrix4x3* outMatrix))(0x00B2EC60);

		RealMatrix4x3 objectTransform;
		Object_GetTransformationMatrix(objectIndex, &objectTransform);

		s_RotationSnapState.StartTime = Blam::Time::GetGameTicks();
		s_RotationSnapState.StartRotation = RealQuaternion::Normalize(RealQuaternion::CreateFromRotationMatrix(objectTransform));
		s_RotationSnapState.EndRotation = RealQuaternion::Normalize(rotation * s_RotationSnapState.StartRotation);
		s_RotationSnapState.Current = 0;
		s_RotationSnapState.ObjectIndex = objectIndex;
	}

	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();

		const auto snapAngleDegrees = moduleForge.VarRotationSnap->ValueFloat;
		const auto rotationSensitvity = moduleForge.VarRotationSensitivity->ValueFloat;

		xRot *= rotationSensitvity;
		yRot *= rotationSensitvity;
		zRot *= rotationSensitvity;

		if (snapAngleDegrees < 1)
		{
			static auto RotateHeldObject = (void(__stdcall*)(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot))(0x0059DD50);
			RotateHeldObject(playerIndex, objectIndex, xRot, yRot, zRot);
			return;
		}

		if (s_RotationSnapState.Current < 1)
			return;

		const auto snapAngleRadians = snapAngleDegrees / 180.0f * Blam::Math::PI;

		const auto xn = std::abs(xRot);
		const auto yn = std::abs(yRot);
		const auto zn = std::abs(zRot);

		const float DEAD_ZONE = 0.5f;

		if (xn > DEAD_ZONE) s_RotationSnapState.InputXTicks++; else s_RotationSnapState.InputXTicks = 0;
		if (yn > DEAD_ZONE) s_RotationSnapState.InputYTicks++; else s_RotationSnapState.InputYTicks = 0;
		if (zn > DEAD_ZONE) s_RotationSnapState.InputZTicks++; else s_RotationSnapState.InputZTicks = 0;

		auto& players = Blam::Players::GetPlayers();
		auto& objects = Blam::Objects::GetObjects();

		auto player = players.Get(playerIndex);

		auto unitObjectDatum = objects.Get(player->SlaveUnit);
		if (!unitObjectDatum || !unitObjectDatum->Data)
			return;

		auto unitObjectPtr = Pointer(unitObjectDatum->Data);

		const auto& unitRightVec = RealVector3D::Normalize(unitObjectPtr(0x60).Read<RealVector3D>());
		const auto& unitUpVec = RealVector3D::Normalize(unitObjectPtr(0x6C).Read<RealVector3D>());
		const auto& unitForwardVec = RealVector3D::Normalize(RealVector3D::Cross(unitUpVec, unitRightVec));

		if (zn > 0.5f)
		{
			if (s_RotationSnapState.InputZTicks == 1)
			{
				auto theta = snapAngleRadians  * (zRot > 0 ? 1 : -1);
				RotateSnapped(objectIndex, RealQuaternion::CreateFromAxisAngle(GetClosestCardianalAxix(unitRightVec), theta));
			}
		}
		else if (xn > yn)
		{
			if (s_RotationSnapState.InputXTicks == 1)
			{
				auto theta = snapAngleRadians  * (xRot > 0 ? -1 : 1);
				RotateSnapped(objectIndex, RealQuaternion::CreateFromAxisAngle(GetClosestCardianalAxix(unitUpVec), theta));
			}
		}
		else
		{
			if (s_RotationSnapState.InputYTicks == 1)
			{
				auto theta = snapAngleRadians  * (yRot > 0 ? 1 : -1);
				RotateSnapped(objectIndex, RealQuaternion::CreateFromAxisAngle(GetClosestCardianalAxix(unitForwardVec), theta));
			}
		}
	}

	void SetRotationSnapAngle(float angle)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();

		if (angle < 0 || angle >= 360)
			angle = 0;

		wchar_t buff[256];
		if (angle > 0)
			swprintf(buff, L"Rotation Snap: %.2f\n", angle);
		else
			swprintf(buff, L"Rotation Snap: OFF");

		static auto PrintKillFeedText = (void(__cdecl *)(unsigned int hudIndex, wchar_t *text, int a3))(0x00A95920);
		PrintKillFeedText(0, buff, 0);

		moduleForge.VarRotationSnap->ValueFloat = angle;
	}

	void UpdateRotationSnap(DatumIndex playerIndex, DatumIndex objectIndex)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();

		const auto snapAngleDegrees = moduleForge.VarRotationSnap->ValueFloat;
	
		if (objectIndex != DatumIndex::Null)
		{
			auto uiLeftTicks = Input::GetActionState(Blam::Input::eGameActionUiLeft)->Ticks;
			auto uiRightTicks = Input::GetActionState(Blam::Input::eGameActionUiRight)->Ticks;

			if (uiLeftTicks == 1 || uiRightTicks == 1)
			{
				auto snapAngle = snapAngleDegrees + 5 * (uiLeftTicks == 1 ? -1 : 1);
				SetRotationSnapAngle(snapAngle);
			}

			if (snapAngleDegrees > 0 && Blam::Time::TicksToSeconds(uiLeftTicks) > 1)
			{
				SetRotationSnapAngle(0);
			}
		}

		if (snapAngleDegrees < 1)
			return;

		if (!(s_RotationSnapState.Flags & 1) || objectIndex == DatumIndex::Null)
		{
			s_RotationSnapState.StartTime = Blam::Time::GetGameTicks();
			s_RotationSnapState.StartRotation = RealQuaternion();
			s_RotationSnapState.EndRotation = s_RotationSnapState.StartRotation;
			s_RotationSnapState.Current = 1.0f;
			s_RotationSnapState.ObjectIndex = DatumIndex::Null;
			s_RotationSnapState.Flags |= 1;
		}


		if (s_RotationSnapState.ObjectIndex == DatumIndex::Null)
			return;

		const auto ROTATION_DURATION_SECONDS = 0.3f;
		s_RotationSnapState.Current += Blam::Time::GetSecondsPerTick() / ROTATION_DURATION_SECONDS;
		if (s_RotationSnapState.Current > 1) s_RotationSnapState.Current = 1;

		if (s_RotationSnapState.ObjectIndex != objectIndex)
		{
			RealMatrix4x3 objectTransform;
			static auto Object_GetTransformationMatrix = (void(__cdecl*)(uint32_t objectIndex, RealMatrix4x3* outMatrix))(0x00B2EC60);
			Object_GetTransformationMatrix(objectIndex, &objectTransform);

			s_RotationSnapState.ObjectIndex = objectIndex;
			s_RotationSnapState.StartRotation = RealQuaternion::CreateFromRotationMatrix(objectTransform);
			s_RotationSnapState.EndRotation = s_RotationSnapState.StartRotation;
			s_RotationSnapState.Current = 1.0f;
		}

		RealVector3D rightVec(1, 0, 0), upVec(0, 0, 1);

		auto interpolatedRotation = RealQuaternion::Slerp(s_RotationSnapState.StartRotation, s_RotationSnapState.EndRotation, s_RotationSnapState.Current);

		rightVec = RealVector3D::Normalize(RealVector3D::Transform(rightVec, interpolatedRotation));
		upVec = RealVector3D::Normalize(RealVector3D::Transform(upVec, interpolatedRotation));

		static auto Object_Transform = (void(__cdecl*)(bool a1, uint32_t objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);
		Object_Transform(0, objectIndex, nullptr, &rightVec, &upVec);
	}

	void __fastcall SandboxEngineObjectDisposeHook(void* thisptr, void* unused, uint32_t objectIndex)
	{
		s_SelectedObjects.Remove(objectIndex);

		static auto SandboxEngineObjectDispose = (void(__thiscall*)(void* thisptr, uint32_t objectIndex))(0x0059BC70);
		SandboxEngineObjectDispose(thisptr, objectIndex);
	}

	void SpecialWeaponHUDHook(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objectsInCluster, int16_t objectcount, BYTE* activeSpecialChudTypes)
	{
		static auto sub_A2CAA0 = (void(__cdecl*)(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objects, int16_t objectCount, BYTE *result))(0xA2CAA0);

		sub_A2CAA0(a1, unitObjectIndex, a3, objectsInCluster, objectcount, activeSpecialChudTypes);

		static auto sub_686FD0 = (void(__cdecl*)())(0x686FD0);
		static auto sub_A78230 = (void(__cdecl*)())(0xA78230);
		static auto sub_A53160 = (void(__cdecl*)(int a1, int a2, char a3, char a4))(0xA53160);
		static auto sub_A7A510 = (void(__cdecl*)(int a1))(0xA7A510);
		static auto sub_A242E0 = (unsigned __int8(__cdecl*)(int a1, int a2))(0xA242E0);
		static auto sub_A48E40 = (int(__cdecl*)(int a1, int a2, int a3))(0xA48E40);
		static auto sub_A78F00 = (int(__cdecl*)(int a1, int a2))(0xA78F00);
		static auto sub_A781F0 = (int(*)())(0xA781F0);
		static auto sub_686DE0 = (int(*)())(0x686DE0);

		auto mapv = GetMapVariant();
		if (!mapv)
			return;

		if (!s_SelectedObjects.Any())
			return;

		for (auto i = 0; i < mapv->UsedPlacementsCount; i++)
		{
			auto& placement = mapv->Placements[i];

			if (s_SelectedObjects.Contains(placement.ObjectIndex))
			{
				const int specialHudType = 6;
				activeSpecialChudTypes[specialHudType] = 1;

				sub_686FD0();
				sub_A78230();
				sub_A53160(placement.ObjectIndex, a1, 0, 1);
				sub_A7A510(0);
				sub_A242E0(
					18,
					(unsigned __int8)(signed int)floor((float)((float)(signed int)specialHudType * 255.0) * 0.125));
				sub_A48E40(2, 23, -1);
				sub_A78F00(0, 1);
				sub_A48E40(1, 0, 0);
				sub_A48E40(2, 0, 0);
				sub_A781F0();
				sub_686DE0();
			}
		}
	}

	void ApplyGrabOffset(uint32_t playerIndex, uint32_t objectIndex)
	{	
		s_GrabOffset = RealVector3D(0, 0, 0);

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		auto objectPtr = Pointer(Blam::Objects::GetObjects().Get(objectIndex))[0xC];
		auto unitObjectPtr = Blam::Objects::GetObjects().Get(player->SlaveUnit);
		if (!player || !unitObjectPtr || !objectPtr)
			return;

		RealVector3D unitPos;
		Objects_GetPosition(player->SlaveUnit, &unitPos);

		const auto crosshairPoint = ElDorito::GetMainTls(0x48)[0](0xe21c).Read<RealVector3D>();
		auto& heldObjectDistance = ElDorito::GetMainTls(0x48)[0](0xE1DC).Read<float>();

		s_GrabOffset = objectPtr(0x20).Read<RealVector3D>() - crosshairPoint;
		heldObjectDistance = (unitPos - crosshairPoint).Length();
	}

	void __cdecl ObjectGrabbedHook(uint32_t playerIndex, uint16_t placementIndex)
	{
		static auto ObjectGrabbed = (void(__cdecl*)(uint32_t, uint32_t))(0x0059B080);
		ObjectGrabbed(playerIndex, placementIndex);

		static auto FreePlacement = (void(__thiscall *)(MapVariant* mapv, int16_t placementIndex, int a3))(0x585C00);
		static auto ObjectAttach = (void(__cdecl*)(uint32_t parentobjectIndex, uint32_t objectIndex, int a3))(0x00B2A250);
		static auto sub_59A620 = (void(__cdecl *)(int objectIndex, char a2))(0x59A620);

		auto& objects = Blam::Objects::GetObjects();

		auto mapv = GetMapVariant();

		auto objectIndex = mapv->Placements[placementIndex].ObjectIndex;

		if (!s_SelectedObjects.Contains(objectIndex))
			return;

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (!(placement.PlacementFlags & 1) ||
				placement.ObjectIndex == -1 ||
				placement.ObjectIndex == objectIndex)
				continue;

			auto placementObjectIndex = placement.ObjectIndex;

			if (s_SelectedObjects.Contains(placement.ObjectIndex))
			{
				FreePlacement(mapv, i, 2);
				ObjectAttach(objectIndex, placementObjectIndex, 0);
				sub_59A620(placementObjectIndex, 1);
			}
		}

		ApplyGrabOffset(playerIndex, objectIndex);
	}

	void ThrowObject(uint32_t playerIndex, uint32_t objectIndex, float objectThrowForce)
	{
		static auto Update_ObjectTransform = (void(__cdecl *)(bool a1, int objectIndex))(0x0059E340);
		static auto Object_Wake = (void(__cdecl *)(uint32_t objectIndex, bool a2))(0x00B327F0);
		static auto Unit_GetPosition = (void(__cdecl*)(uint32_t unitObjectIndex, RealVector3D *position))(0x00B439D0);
		static auto Object_GetVelocity = (void(__cdecl *)(uint32_t objectIndex, RealVector3D* a2, RealVector3D *a3))(0x00B2EB30);
		static auto sub_B34040 = (void(__cdecl*)(uint32_t objectIndex, RealVector3D* v1, RealVector3D* v2))(0xB34040);
		static auto sub_580AD0 = (void(__cdecl*)(uint32_t unitObjectIndex, int a2))(0x580AD0);
		static auto sub_59EA10 = (void(__cdecl*)(uint32_t objectIndex))(0x59EA10);
		static auto Object_GetRotation = (void(__cdecl*) (uint16_t objectIndex, RealVector3D *right, RealVector3D *up))(0x00B2E490);
		static auto Object_GetPosition = (void(__cdecl*) (uint16_t objectIndex, RealVector3D *position))(0x00B2E5A0);

		RealVector3D right, up, position;
		Object_GetRotation(objectIndex, &right, &up);
		Object_GetPosition(objectIndex, &position);

		auto& players = Blam::Players::GetPlayers();
		auto& objects = Blam::Objects::GetObjects();

		auto player = players.Get(playerIndex);

		Object_Wake(objectIndex, 0);

		if (objectThrowForce > 0.000099999997f)
		{
			auto unitObjectPtr = Pointer(objects.Get(player->SlaveUnit))[0xC];
			if (!unitObjectPtr)
				return;

			RealVector3D unitPosition;
			Unit_GetPosition(player->SlaveUnit, &unitPosition);

			auto vec = position - unitPosition;
			RealVector3D::Normalize(vec);

			vec.I = objectThrowForce * 15.0f * vec.I;
			vec.J = objectThrowForce * 15.0f * vec.J;
			vec.K = objectThrowForce * 15.0f * vec.K;

			RealVector3D v2, v3;
			Object_GetVelocity(objectIndex, &v2, &v3);
			sub_B34040(objectIndex, &v2, &v3);
			sub_580AD0(objectIndex, 4);

		}
		sub_59EA10(objectIndex);
	}

	void __cdecl ObjectDroppedHook(uint16_t placementIndex, float throwForce, int a3)
	{
		static auto GetPlayerHoldingObject = (uint32_t(__cdecl*)(int objectIndex))(0x0059BB90);
		static auto ObjectDropped = (void(__cdecl*)(uint16_t placementIndex, float throwForce, int a3))(0x0059B250);

		auto mapv = GetMapVariant();
		auto& objects = Blam::Objects::GetObjects();

		auto droppedObjectIndex = mapv->Placements[placementIndex].ObjectIndex;
		if (droppedObjectIndex == -1)
			return;

		auto playerIndex = GetPlayerHoldingObject(droppedObjectIndex);

		ObjectDropped(placementIndex, throwForce, a3);

		if (!s_SelectedObjects.Contains(droppedObjectIndex))
			return;

		auto droppedObjectPtr = Pointer(objects.Get(droppedObjectIndex))[0xC];
		if (!droppedObjectPtr)
			return;

		std::stack<uint32_t> detachStack;
		for (auto objectIndex = droppedObjectPtr(0x10).Read<uint32_t>(); objectIndex != -1;)
		{
			auto objectPtr = Pointer(objects.Get(objectIndex))[0xC];
			if (!objectPtr)
				continue;

			if (s_SelectedObjects.Contains(objectIndex))
				detachStack.push(objectIndex);

			objectIndex = objectPtr(0xC).Read<uint32_t>();
		}

		while (!detachStack.empty())
		{
			auto objectIndex = detachStack.top();
			detachStack.pop();

			static auto ObjectDetach = (void(__cdecl*)(uint32_t objectIndex))(0x00B2D180);
			static auto AssignPlacement = (int(__thiscall *)(void *thisptr, uint32_t objectIndex, int16_t placementIndex))(0x5855E0);
			static auto Object_GetRotation = (void(__cdecl*) (uint32_t objectIndex, RealVector3D *right, RealVector3D *up))(0x00B2E490);
			static auto Object_GetPosition = (void(__cdecl*) (uint32_t objectIndex, RealVector3D *position))(0x00B2E5A0);
			static auto Object_Transform = (void(__cdecl *)(bool a1, uint32_t objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);
			static auto Update_ObjectTransform = (void(__cdecl *)(float a1, uint32_t objectIndex))(0x0059E9C0);
			static auto Object_SetVelocity_0 = (void(__cdecl *)(uint32_t objectIndex, RealVector3D *movementVec, RealVector3D *vec))(0x00B34040);
			static auto sub_B313E0 = (void(__cdecl *)(int objectIndex, bool arg_4))(0xB313E0);


			ObjectDetach(objectIndex);
			AssignPlacement(mapv, objectIndex, -1);

			RealVector3D right, up, position;
			Object_GetRotation(objectIndex, &right, &up);
			Object_GetPosition(objectIndex, &position);
			Object_Transform(0, objectIndex, &position, &right, &up);
			Update_ObjectTransform(0, objectIndex);

			sub_B313E0(objectIndex, true);

			ThrowObject(playerIndex, objectIndex, throwForce);
		}
	}

	void __cdecl ObjectDeleteHook(uint16_t placementIndex, uint32_t playerIndex)
	{
		static auto ObjectDelete = (void(__cdecl*)(uint16_t placementIndex, uint32_t playerIndex))(0x0059A920);

		auto mapv = GetMapVariant();
		auto deletedObjectIndex = mapv->Placements[placementIndex].ObjectIndex;

		if (s_SelectedObjects.Contains(deletedObjectIndex))
		{
			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!(placement.PlacementFlags & 1) ||
					placement.ObjectIndex == -1 ||
					placement.ObjectIndex == deletedObjectIndex)
					continue;

				auto placementObjectIndex = placement.ObjectIndex;

				if (s_SelectedObjects.Contains(placement.ObjectIndex))
				{
					ObjectDelete(i, playerIndex);
				}
			}
		}
	
		ObjectDelete(placementIndex, playerIndex);

		Pointer(ElDorito::GetMainTls(0x48))[0](0xE1DC).Write(*(float*)0x018A157C);
	}

	void __cdecl ObjectPropertiesChangeHook(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties)
	{
		static auto ObjectPropertiesChange = (void(__cdecl*)(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties))(0x0059B5F0);
		ObjectPropertiesChange(playerIndex, placementIndex, properties);

		auto mapv = GetMapVariant();
		auto changedObjectIndex = mapv->Placements[placementIndex].ObjectIndex;

		if (s_SelectedObjects.Contains(changedObjectIndex))
		{
			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!(placement.PlacementFlags & 1) ||
					placement.ObjectIndex == -1 ||
					placement.ObjectIndex == changedObjectIndex)
					continue;

				auto placementObjectIndex = placement.ObjectIndex;

				if (s_SelectedObjects.Contains(placement.ObjectIndex))
				{
					ObjectPropertiesChange(playerIndex, i, properties);
				}
			}
		}
	}

	void CloneSelection(DatumIndex playerIndex)
	{
		auto mapv = GetMapVariant();
		auto objects = Blam::Objects::GetObjects();

		std::stack<uint32_t> cloneStack;

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (!(placement.PlacementFlags & 1) ||
				placement.ObjectIndex == -1)
				continue;

			auto placementObjectIndex = placement.ObjectIndex;

			if (s_SelectedObjects.Contains(placement.ObjectIndex))
				cloneStack.push(placement.ObjectIndex);
		}

		if (cloneStack.empty())
			return;

		auto referenceObjectIndex = cloneStack.top();
		auto referenceObjectPtr = Pointer(objects.Get(referenceObjectIndex))[0xC];
		if (!referenceObjectPtr)
			return;

		s_SelectedObjects.Clear();

		const auto& crosshairPoint = Pointer(ElDorito::GetMainTls(0x48))[0](0xE21C).Read<RealVector3D>();
		const auto& referencePoint = referenceObjectPtr(0x54).Read<RealVector3D>();

		auto grabObjectIndex = -1;

		while (!cloneStack.empty())
		{
			auto objectIndex = cloneStack.top();
			cloneStack.pop();

			auto objectPtr = Pointer(objects.Get(objectIndex))[0xC];
			if (!objectPtr)
				continue;

			auto objectTagIndex = objectPtr.Read<uint32_t>();
			auto objectPosition = objectPtr(0x54).Read<RealVector3D>();
			auto objectRightVec = objectPtr(0x60).Read<RealVector3D>();
			auto objectUpVec = objectPtr(0x6c).Read<RealVector3D>();
			auto placementIndex = objectPtr(0x1c).Read<uint16_t>();

			MapVariant::VariantProperties* variantProperties = nullptr;

			if (placementIndex != -1)
			{
				auto& placement = mapv->Placements[placementIndex];
				variantProperties = &placement.Properties;
			}

			static auto SpawnObject = (uint32_t(__thiscall *)(MapVariant* thisptr, uint32_t tagIndex, int a3, int16_t placementIndex, const RealVector3D* position,
				const RealVector3D *rightVec, const RealVector3D *upVec, int16_t scnrPlacementBlockIndex, int objectType,
				MapVariant::VariantProperties* variantProperties, uint16_t placementFlags))(0x00582110);

			auto displacement = crosshairPoint - objectPosition;
			auto pos = objectPosition + displacement - (referencePoint - objectPosition);

			auto newObjectIndex = SpawnObject(mapv, objectTagIndex, 0, -1, &pos, &objectRightVec, &objectUpVec, -1, -1, variantProperties, 0);
			if (newObjectIndex != -1)
			{
				grabObjectIndex = newObjectIndex;
				s_SelectedObjects.Add(newObjectIndex);
			}
		}

		s_SandboxTickCommandQueue.push([=]()
		{
			auto grabObjectPtr = Pointer(objects.Get(grabObjectIndex))[0xC];
			if (!grabObjectPtr)
				return;

			ObjectGrabbedHook(playerIndex, grabObjectPtr(0x1c).Read<int16_t>());
		});
	}


	void UnitFlyingHook(uint32_t unitObjectIndex, int a2, int a3, int a4, int a5, int a6, int a7)
	{
		static auto UnitFlying = (void(__cdecl *)(int a1, int a2, int a3, int a4, int a5, int a6, int a7))(0x7205D0);
		static auto Forge_GetEditorModeState = (bool(__cdecl *)(uint32_t playerIndex, uint32_t* heldObjectIndex, uint32_t* objectIndexUnderCrosshair))(0x59A6F0);

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		auto& players = Blam::Players::GetPlayers();
		auto player = players.Get(playerIndex);

		if (player && player->SlaveUnit == DatumIndex(unitObjectIndex)
			&& Forge_GetEditorModeState(playerIndex, nullptr, nullptr))
		{
			auto& moduleForge = Modules::ModuleForge::Instance();
			auto& monitorSpeed = *(float*)(a2 + 0x150);
			monitorSpeed *= moduleForge.VarMonitorSpeed->ValueFloat;

			UnitFlying(unitObjectIndex, a2, a3, a4, a5, a6, a7);

			auto activeScreenCount = Pointer(0x05260F34)[0](0x3c).Read<int16_t>();
			if (activeScreenCount != 0)
				return;

			auto& objects = Blam::Objects::GetObjects();
			auto unitObjectPtr = Pointer(objects.Get(unitObjectIndex))[0xC];
			if (!unitObjectPtr)
				return;

			auto unitDefPtr = Pointer(Blam::Tags::TagInstance(unitObjectPtr.Read<uint32_t>()).GetDefinition<void>());
			auto maxVelocity = *(float*)unitDefPtr(0x564) * monitorSpeed;
			auto acceleration = *(float*)unitDefPtr(0x56C);
			auto deceleration = *(float*)unitDefPtr(0x570);

			auto direction = 0;
			if (Blam::Input::GetActionState(Blam::Input::eGameActionUiRightBumper)->Ticks > 0)
				direction = 1;
			if (Blam::Input::GetActionState(Blam::Input::eGameActionUiLeftBumper)->Ticks > 0)
				direction = -1;

			static float s_Velocity = 0;

			auto t = Blam::Time::GetSecondsPerTick();
			auto destination = direction * maxVelocity;
			s_Velocity = s_Velocity * (1.0f - t * acceleration) + destination * (t * deceleration);

			auto v = (RealVector3D*)(a2 + 0x134);
			if (direction != 0)
				v->K = s_Velocity;
		}
		else
		{
			UnitFlying(unitObjectIndex, a2, a3, a4, a5, a6, a7);
		}
	}

	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* rightVec, RealVector3D* upVec)
	{
		static auto Object_Transform = (void(*)(bool a1, int objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);

		auto heldObjectPtr = Pointer(Blam::Objects::GetObjects().Get(objectIndex))[0xC];
		if (!heldObjectPtr)
			return;

		if (s_SelectedObjects.Contains(objectIndex))
		{
			const auto& centerPos = heldObjectPtr(0x20).Read<RealVector3D>();
			const auto& objectPos = heldObjectPtr(0x54).Read<RealVector3D>();
			const auto& crosshairPoint = ElDorito::GetMainTls(0x48)[0](0xe21c).Read<RealVector3D>();

			auto offset = centerPos - objectPos - s_GrabOffset;
			auto newPos = crosshairPoint - offset;

			Object_Transform(a1, objectIndex, &newPos, rightVec, upVec);
		}
		else
		{
			Object_Transform(a1, objectIndex, position, rightVec, upVec);
		}
	}
}
