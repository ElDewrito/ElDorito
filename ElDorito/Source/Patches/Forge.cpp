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
#include "../Forge/RotationSnap.hpp"
#include "../Forge/Selection.hpp"
#include "../Forge/SelectionRenderer.hpp"
#include "../Forge/Magnets.hpp"
#include "../Modules/ModuleForge.hpp"
#include <cassert>
#include <queue>
#include <stack>

using namespace Forge;
using namespace Blam;
using namespace Blam::Objects;
using namespace Blam::Math;

namespace
{
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
	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* forwardVec, RealVector3D* upVec);
	void SandboxEngineInitHook();
	void SandboxEngineShutdownHook();
	void SandboxEngineTickHook();
	void __fastcall SandboxEngineObjectDisposeHook(void* thisptr, void* unused, uint32_t objectIndex);

	void FixRespawnZones();
	void GrabSelection(uint32_t playerIndex);
	void DoClone(uint32_t playerIndex, uint32_t objectIndexUnderCrosshair);

	std::queue<std::function<void()>> s_SandboxTickCommandQueue;
	RealVector3D s_GrabOffset;

	bool shown_legal(char unused);
}

namespace Patches
{
	namespace Forge
	{
		void ApplyAll()
		{
			Pointer(0x0165AB54).Write<uint32_t>((uint32_t)&SandboxEngineTickHook);
			Pointer(0x0165AB94).Write((uint32_t)&SandboxEngineObjectDisposeHook);
			Pointer(0x0165AB04).Write((uint32_t)&SandboxEngineInitHook);
			Pointer(0x0165AB08).Write((uint32_t)&SandboxEngineShutdownHook);

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
			Hook(0x7350A, ObjectGrabbedHook, HookFlags::IsCall).Apply();
			Hook(0x7356F, ObjectDroppedHook, HookFlags::IsCall).Apply();
			Hook(0x734FC, ObjectDeleteHook, HookFlags::IsCall).Apply();
			Hook(0x73527, ObjectPropertiesChangeHook, HookFlags::IsCall).Apply();
			Hook(0x7AF758, UnitFlyingHook, HookFlags::IsCall).Apply();
			Hook(0x19CAFC, UpdateHeldObjectTransformHook, HookFlags::IsCall).Apply();

			// enable teleporter volume editing compliments of zedd
			Patch::NopFill(Pointer::Base(0x6E4796), 0x66);

			Hook(0x70E33B, shown_legal, HookFlags::IsCall).Apply();
			Hook(0x6EAAD3, shown_legal, HookFlags::IsCall).Apply();
			Hook(0x6EAA2D, shown_legal, HookFlags::IsCall).Apply();

			Patches::Core::OnGameStart(FixRespawnZones);
		}

		void Tick()
		{
			// Require a rescan for barrier disabler objects each tick
			barriersEnabledValid = false;
		}

		bool SavePrefab(const std::string& name, const std::string& path)
		{
			return ::Forge::Prefabs::Save(name, path);
		}

		bool LoadPrefab(const std::string& path)
		{
			s_SandboxTickCommandQueue.push([=]()
			{
				::Forge::Prefabs::Load(path);
				GrabSelection(Blam::Players::GetLocalPlayer(0));
			});

			return true;
		}
	}
}

namespace
{
	void SandboxEngineInitHook()
	{
		static auto SandboxEngineInit = (void(*)())(0x0059C0D0);
		SandboxEngineInit();

		Forge::Magnets::Initialize();
		Forge::SelectionRenderer::Initialize();	
	}

	void SandboxEngineShutdownHook()
	{
		static auto SandboxEngineShutdown = (void(*)())(0x0059A600);
		SandboxEngineShutdown();

		Forge::Magnets::Shutdown();

		Forge::Selection::Clear();
	}

	void SandboxEngineTickHook()
	{
		static auto SandboxEngine_Tick = (void(*)())(0x0059ED70);

		SandboxEngine_Tick();
		Forge::SelectionRenderer::Update();
		Forge::Magnets::Update();
		
		while (!s_SandboxTickCommandQueue.empty())
		{
			auto cmd = s_SandboxTickCommandQueue.front();
			cmd();
			s_SandboxTickCommandQueue.pop();
		}

		auto activeScreenCount = Pointer(0x05260F34)[0](0x3c).Read<int16_t>();
		if (activeScreenCount > 0)
			return;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == DatumIndex::Null)
			return;

		uint32_t heldObjectIndex = -1, objectIndexUnderCrosshair = -1;

		if (GetEditorModeState(playerIndex, &heldObjectIndex, &objectIndexUnderCrosshair))
		{
			const auto& moduleForge = Modules::ModuleForge::Instance();
			// only show selection when we're in monitor mode
			Forge::SelectionRenderer::SetEnabled(true);
			auto rendererType = Forge::SelectionRenderer::RendererImplementationType(moduleForge.VarSelectionRenderer->ValueInt);
			Forge::SelectionRenderer::SetRendererType(rendererType);
			Forge::Selection::Update();

			Forge::RotationSnap::Update(playerIndex, heldObjectIndex);

			if (heldObjectIndex == -1)
			{
				if (GetActionState(Blam::Input::eGameActionMelee)->Ticks == 1)
					DoClone(playerIndex, objectIndexUnderCrosshair);
			}
		}
		else
		{
			Forge::SelectionRenderer::SetEnabled(false);
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

		auto& players = Blam::Players::GetPlayers();
		auto& objects = Blam::Objects::GetObjects();

		auto player = players.Get(playerIndex);
		auto unitObject = Blam::Objects::Get(player->SlaveUnit);
		if (!unitObject)
			return;

		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(player->SlaveUnit, &objectTransform);

		const auto xn = std::abs(xRot);
		const auto yn = std::abs(yRot);
		const auto zn = std::abs(zRot);

		const float DEAD_ZONE = 0.5f;
		static float xticks = 0, yticks = 0, zticks = 0;

		if (xn > DEAD_ZONE) xticks++; else xticks = 0;
		if (yn > DEAD_ZONE) yticks++; else yticks = 0;
		if (zn > DEAD_ZONE) zticks++; else zticks = 0;

		if (zn > DEAD_ZONE && zticks == 1)
			Forge::RotationSnap::RotateSnapped(GetClosestCardianalAxix(objectTransform.Forward) * (zRot > 0 ? 1.0f : -1.0f));
		else if (xn > yn && xticks == 1)
			Forge::RotationSnap::RotateSnapped(GetClosestCardianalAxix(objectTransform.Up) * (xRot > 0 ? -1.0f : 1.0f));
		else if (yticks == 1)
			Forge::RotationSnap::RotateSnapped(GetClosestCardianalAxix(objectTransform.Left) *  (yRot > 0 ? 1.0f : -1.0f));
	}

	void __fastcall SandboxEngineObjectDisposeHook(void* thisptr, void* unused, uint32_t objectIndex)
	{
		Forge::Selection::GetSelection().Remove(objectIndex);

		static auto SandboxEngineObjectDispose = (void(__thiscall*)(void* thisptr, uint32_t objectIndex))(0x0059BC70);
		SandboxEngineObjectDispose(thisptr, objectIndex);
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
		GetObjectPosition(player->SlaveUnit, &unitPos);

		auto& sandboxGlobals = GetSandboxGlobals();

		const auto crosshairPoint = sandboxGlobals.CrosshairPoints[playerIndex & 0xFFFF];
		auto& heldObjectDistance = sandboxGlobals.HeldObjectDistances[playerIndex & 0xFFFF];

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

		const auto& selection = Forge::Selection::GetSelection();
		if (!selection.Contains(objectIndex))
			return;

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (!(placement.PlacementFlags & 1) || placement.ObjectIndex == objectIndex)
				continue;

			auto placementObjectIndex = placement.ObjectIndex;

			if (selection.Contains(placement.ObjectIndex))
			{
				FreePlacement(mapv, i, 2);
				ObjectAttach(objectIndex, placementObjectIndex, 0);
				sub_59A620(placementObjectIndex, 1);
			}
		}

		ApplyGrabOffset(playerIndex, objectIndex);
	}

	void __cdecl ObjectDroppedHook(uint16_t placementIndex, float throwForce, int a3)
	{
		static auto GetPlayerHoldingObject = (uint32_t(__cdecl*)(int objectIndex))(0x0059BB90);
		static auto ObjectDropped = (void(__cdecl*)(uint16_t placementIndex, float throwForce, int a3))(0x0059B250);
		static auto Object_Transform = (void(*)(bool a1, int objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);
		static auto Object_GetPosition = (void(*)(uint32_t objectIndex, RealVector3D *position))(0x00B2E5A0);

		auto mapv = GetMapVariant();
		auto& objects = Blam::Objects::GetObjects();

		auto droppedObjectIndex = mapv->Placements[placementIndex].ObjectIndex;
		if (droppedObjectIndex == -1)
			return;

		auto playerIndex = GetPlayerHoldingObject(droppedObjectIndex);

		ObjectDropped(placementIndex, throwForce, a3);

		
		auto droppedObject = Blam::Objects::Get(droppedObjectIndex);
		if (!droppedObject)
			return;

		auto& magnetPair = Magnets::GetMagnetPair();
		if (magnetPair.IsValid)
		{
			RealVector3D heldObjectPosition;
			Object_GetPosition(droppedObjectIndex, &heldObjectPosition);
			heldObjectPosition = magnetPair.Dest->Position - (magnetPair.Source->Position - heldObjectPosition);
			Object_Transform(0, droppedObjectIndex, &heldObjectPosition, nullptr, nullptr);
		}

		auto& selection = Forge::Selection::GetSelection();
		if (!selection.Contains(droppedObjectIndex))
			return;


		std::stack<uint32_t> detachStack;
		for (auto objectIndex = droppedObject->FirstChild; objectIndex != DatumIndex::Null;)
		{
			auto object = Blam::Objects::Get(objectIndex);
			if (!object)
				continue;

			if (selection.Contains(objectIndex))
				detachStack.push(objectIndex);

			objectIndex = object->NextSibling;
		}

		while (!detachStack.empty())
		{
			auto objectIndex = detachStack.top();
			detachStack.pop();

			static auto ObjectDetach = (void(__cdecl*)(uint32_t objectIndex))(0x00B2D180);
			static auto AssignPlacement = (int(__thiscall *)(void *thisptr, uint32_t objectIndex, int16_t placementIndex))(0x5855E0);
			static auto Object_Transform = (void(__cdecl *)(bool a1, uint32_t objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);
			static auto Update_ObjectTransform = (void(__cdecl *)(float a1, uint32_t objectIndex))(0x0059E9C0);
			static auto sub_B313E0 = (void(__cdecl *)(int objectIndex, bool arg_4))(0xB313E0);

			ObjectDetach(objectIndex);
			AssignPlacement(mapv, objectIndex, -1);

			RealMatrix4x3 objectTransform;
			GetObjectTransformationMatrix(objectIndex, &objectTransform);
			Object_Transform(0, objectIndex, &objectTransform.Position, &objectTransform.Forward, &objectTransform.Up);
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

		const auto& selection = Forge::Selection::GetSelection();

		if (selection.Contains(deletedObjectIndex))
		{
			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!placement.InUse() || placement.ObjectIndex == -1 || placement.ObjectIndex == deletedObjectIndex)
					continue;

				const auto& selection = Forge::Selection::GetSelection();
				if (selection.Contains(placement.ObjectIndex))
					ObjectDelete(i, playerIndex);
			}
		}

		ObjectDelete(placementIndex, playerIndex);

		Forge::GetSandboxGlobals().HeldObjectDistances[playerIndex & 0xFFFF] = *(float*)0x018A157C;
	}

	void __cdecl ObjectPropertiesChangeHook(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties)
	{
		static auto ObjectPropertiesChange = (void(__cdecl*)(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties))(0x0059B5F0);
		ObjectPropertiesChange(playerIndex, placementIndex, properties);

		auto mapv = GetMapVariant();
		auto changedObjectIndex = mapv->Placements[placementIndex].ObjectIndex;

		const auto& selection = Forge::Selection::GetSelection();

		if (selection.Contains(changedObjectIndex))
		{
			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!placement.InUse() || placement.ObjectIndex == -1 || placement.ObjectIndex == changedObjectIndex)
					continue;

				const auto& selection = Forge::Selection::GetSelection();
				if (selection.Contains(placement.ObjectIndex))
					ObjectPropertiesChange(playerIndex, i, properties);
			}
		}
	}

	void UnitFlyingHook(uint32_t unitObjectIndex, int a2, int a3, int a4, int a5, int a6, int a7)
	{
		static auto UnitFlying = (void(__cdecl *)(int a1, int a2, int a3, int a4, int a5, int a6, int a7))(0x7205D0);

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		auto& players = Blam::Players::GetPlayers();
		auto player = players.Get(playerIndex);

		if (player && player->SlaveUnit == DatumIndex(unitObjectIndex) && GetEditorModeState(playerIndex, nullptr, nullptr))
		{
			auto& moduleForge = Modules::ModuleForge::Instance();
			auto& monitorSpeed = *(float*)(a2 + 0x150);
			monitorSpeed *= moduleForge.VarMonitorSpeed->ValueFloat;

			UnitFlying(unitObjectIndex, a2, a3, a4, a5, a6, a7);

			auto activeScreenCount = Pointer(0x05260F34)[0](0x3c).Read<int16_t>();
			if (activeScreenCount != 0)
				return;

			auto unitObject = Blam::Objects::Get(unitObjectIndex);
			if (!unitObject)
				return;

			auto unitDefPtr = Pointer(Blam::Tags::TagInstance(unitObject->TagIndex).GetDefinition<void>());
			auto maxVelocity = *(float*)unitDefPtr(0x564) * monitorSpeed;
			auto acceleration = *(float*)unitDefPtr(0x56C);
			auto deceleration = *(float*)unitDefPtr(0x570);

			auto uiRightBumper = Blam::Input::GetActionState(Blam::Input::eGameActionUiRightBumper);
			auto uiLeftBumper = Blam::Input::GetActionState(Blam::Input::eGameActionUiLeftBumper);

			auto direction = 0;
			if (!(uiRightBumper->Flags & 1) && uiRightBumper->Ticks > 0)
			{
				uiRightBumper->Flags &= Blam::Input::eActionStateFlagsHandled;
				direction = 1;
			}

			if (!(uiLeftBumper->Flags & 1) && uiLeftBumper->Ticks > 0)
			{
				uiLeftBumper->Flags &= Blam::Input::eActionStateFlagsHandled;
				direction = -1;
			}

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

	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* forwardVec, RealVector3D* upVec)
	{
		static auto Object_Transform = (void(*)(bool a1, int objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);

		auto heldObject = Blam::Objects::Get(objectIndex);
		if (!heldObject)
			return;

		DatumIndex playerIndex = GetPlayerHoldingObject(objectIndex);

		const auto& selection = Forge::Selection::GetSelection();
		if (selection.Contains(objectIndex))
		{
			auto offset = heldObject->Center - heldObject->Position - s_GrabOffset;
			auto newPos = GetSandboxGlobals().CrosshairPoints[playerIndex.Index()] - offset;
			Object_Transform(a1, objectIndex, &newPos, forwardVec, upVec);
		}
		else
		{
			Object_Transform(a1, objectIndex, position, forwardVec, upVec);
		}
	}

	void FixRespawnZones()
	{
		const auto& objects = Blam::Objects::GetObjects();
		auto mapv = GetMapVariant();

		// loop throught mapv placements
		for (auto i = 0; i < mapv->UsedPlacementsCount; i++)
		{
			const auto& placement = mapv->Placements[i];
			if (!placement.InUse() || placement.ObjectIndex == -1)
				continue;
			// Player Respawn Zone
			if (placement.Properties.ObjectType != 0xD)
				continue;
			auto zoneObject = Blam::Objects::Get(placement.ObjectIndex);
			if (!zoneObject)
				continue;

			ZoneShape zoneShape;
			GetObjectZoneShape(placement.ObjectIndex, &zoneShape, 0);

			for (auto j = 0; j < mapv->UsedPlacementsCount; j++)
			{
				auto foundObjectIndex = mapv->Placements[j].ObjectIndex;

				auto foundObject = Blam::Objects::Get(foundObjectIndex);
				if (!foundObject || foundObjectIndex == placement.ObjectIndex)
					continue;

				auto mpPropertiesPtr = Pointer(foundObject->GetMultiplayerProperties());
				if (!mpPropertiesPtr)
					continue;
				auto mpObjectType = mpPropertiesPtr(0x2).Read<uint8_t>();
				auto flags = mpPropertiesPtr.Read<uint16_t>();

				// check if the object's center is inside the zone
				if (!PointIntersectsZone(&foundObject->Center, &zoneShape))
					continue;
				// PlayerSpawnLocation
				if (mpObjectType != 0xC)
					continue;
				// ignore invisible spawns and initial spawns
				if (foundObject->TagIndex == 0x00002EA6 || flags & (1 << 1))
					continue;

				// set the team index to match the zone
				auto zoneTeamIndex = placement.Properties.ObjectFlags >> 8;
				mpPropertiesPtr(0xA).Write<uint8_t>(zoneTeamIndex);
			}
		}
	}

	void GrabSelection(uint32_t playerIndex)
	{
		const auto mapv = GetMapVariant();

		const auto& selection = Forge::Selection::GetSelection();
		for (auto i = 0; i < 640; i++)
		{
			if (selection.Contains(mapv->Placements[i].ObjectIndex))
			{
				ObjectGrabbedHook(playerIndex, i);
				break;
			}
		}
	}

	void DoClone(uint32_t playerIndex, uint32_t objectIndexUnderCrosshair)
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
			if (Forge::Selection::GetSelection().Any())
			{
				if (Forge::Selection::Clone())
					GrabSelection(playerIndex);
			}
		}
	}
	bool shown_legal(char unused)
	{
		return true;
	}
}
