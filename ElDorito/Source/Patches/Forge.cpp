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
#include "../Modules/ModuleForge.hpp"
#include <cassert>
#include <queue>

using namespace Blam;
using namespace Blam::Objects;

namespace
{
	using RealVector3D = Blam::Math::RealVector3D;
	using RealQuaternion = Blam::Math::RealQuaternion;
	using RealMatrix4x3 = Blam::Math::RealMatrix4x3;

	bool shouldDelete = false;

	bool barriersEnabledValid = false;
	bool killBarriersEnabled = true;
	bool pushBarriersEnabled = true;

	void UpdateBarriersEnabled();
	bool CheckKillTriggersHook(int a0, void *a1);
	bool ObjectSafeZoneHook(void *a0);
	void* PushBarriersGetStructureDesignHook(int index);
	void UpdateForgeInputHook();
	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot);

	void FixRespawnZones();

	void SandboxEngineTickHook();

	MapVariant* GetMapVariant();
	struct AABB { float MinX, MaxX, MinY, MaxY, MinZ, MaxZ; };
	bool CalculateObjectBoundingBox(DatumIndex objectTagIndex, AABB* outBoundingBox);
	DatumIndex GetObjectIndexUnderCrosshair(DatumIndex playerIndex);
	bool GetObjectUnderCrosshairIntersectNormal(DatumIndex playerIndex, Math::RealVector3D* outNormalVec);
	DatumIndex CloneObject(DatumIndex playerIndex, DatumIndex objectIndex, float depth);
	void DeletePlacementObject(DatumIndex playerIndex, uint16_t placementIndex);
	void UpdateRotationSnap(DatumIndex playerIndex, DatumIndex objectIndex);

	struct
	{
		using RealQuaternion3D = Blam::Math::RealQuaternion;
		using RealVector3D = Blam::Math::RealVector3D;

		uint32_t Flags;
		DatumIndex ObjectIndex;
		uint32_t StartTime;
		float Current;
		float InputXTicks;
		float InputYTicks;
		float InputZTicks;
		RealQuaternion3D StartRotation;
		RealQuaternion3D EndRotation;

	} s_RotationSnapState = { 0 };
}

namespace Patches
{
	namespace Forge
	{
		void ApplyAll()
		{
			Pointer(0x0165AB54).Write<uint32_t>((uint32_t)&SandboxEngineTickHook);
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
			Hook(0x19FA69, RotateHeldObjectHook, HookFlags::IsCall).Apply();

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

	}
}

namespace
{
	using RealVector3D = Blam::Math::RealVector3D;

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
				mov byte ptr[eax + 0x9E], 1    // Ticks = 1
				and byte ptr[eax + 0x9F], 0xFE // Clear the "handled" flag

											   // Call the original function
				push esi
				mov eax, 0x59F0E0
				call eax

				// Make sure nothing else gets the fake press
				mov eax, 0x244D1F0          // Controller data
				or byte ptr[eax + 0x9F], 1 // Set the "handled" flag
				retn 4
		}
	}

	void SandboxEngineTickHook()
	{
		static auto SandboxEngine_Tick = (void(*)())(0x0059ED70);
		static auto Forge_GetEditorModeState = (bool(__cdecl *)(uint32_t playerIndex, uint32_t* heldObjectIndex, uint32_t* objectIndexUnderCrosshair))(0x0059A6F0);

		SandboxEngine_Tick();

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == DatumIndex::Null)
			return;

		uint32_t heldObjectIndex = -1, objectIndexUnderCrosshair = -1;
		if (Forge_GetEditorModeState(playerIndex, &heldObjectIndex, &objectIndexUnderCrosshair))
		{

			UpdateRotationSnap(playerIndex, heldObjectIndex);

			if (heldObjectIndex == -1)
			{
				if (Input::GetActionState(Blam::Input::eGameActionMelee)->Ticks == 1 && objectIndexUnderCrosshair != -1)
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

	inline MapVariant* GetMapVariant()
	{
		static auto GetMapVariant = (Blam::MapVariant* (__cdecl*)())(0x00583230);
		return GetMapVariant();
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
}
