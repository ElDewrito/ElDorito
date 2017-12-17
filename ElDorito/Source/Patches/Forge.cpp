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
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Blam/Math/RealColorRGB.hpp"
#include "../Blam/Tags/Camera/AreaScreenEffect.hpp"
#include "../Blam/Tags/Items/DefinitionWeapon.hpp"
#include "../Blam/Tags/Models/Model.hpp"
#include "../ElDorito.hpp"
#include "Core.hpp"
#include "../Forge/Prefab.hpp"
#include "../Forge/ObjectSet.hpp"
#include "../Forge/ForgeUtil.hpp"
#include "../Forge/RotationSnap.hpp"
#include "../Forge/Selection.hpp"
#include "../Forge/SelectionRenderer.hpp"
#include "../Forge/Magnets.hpp"
#include "../Forge/ForgeVolumes.hpp"
#include "../Forge/PrematchCamera.hpp"
#include "../Modules/ModuleForge.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Web/Ui/WebForge.hpp"
#include <cassert>
#include <queue>
#include <stack>
#include "../CommandMap.hpp"

using namespace Forge;
using namespace Blam;
using namespace Blam::Objects;
using namespace Blam::Math;

namespace
{
	const auto HELDOBJECT_DISTANCE_MIN = 0.1f;
	const auto HELDOBJECT_DISTANCE_CHANGE_MULTIPLIER = 0.1f;
	const auto HELDOBJECT_ROTATION_SENSITIVTY_BASE = 0.5f;
	const auto REFORGE_DEFAULT_SHADER = 0x3ab0;
	const auto INVISIBLE_MATERIAL_INDEX = 121;

	const auto UI_PlaySound = (void(*)(int index, uint32_t uiSoundTagIndex))(0x00AA5CD0);

	bool barriersEnabledValid = false;
	bool killBarriersEnabled = true;
	bool pushBarriersEnabled = true;

	void UpdateMapModifier();
	bool CheckKillTriggersHook(int a0, void *a1);
	bool ObjectSafeZoneHook(void *a0);
	void* PushBarriersGetStructureDesignHook(int index);
	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot);
	void SpecialWeaponHUDHook(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objectsInCluster, int16_t objectcount, BYTE* activeSpecialChudTypes);
	void ObjectGrabbedHook(uint32_t playerIndex, uint16_t placementIndex);
	void ObjectDroppedHook(uint16_t placementIndex, float throwForce, int a3);
	void ObjectDeleteHook(uint16_t placementIndex, uint32_t playerIndex);
	void ObjectSpawnedHook(uint32_t tagIndex, uint32_t playerIndex, const RealVector3D* position);
	void ObjectPropertiesChangeHook(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties);
	void UnitFlyingHook(uint32_t unitObjectIndex, int a2, int a3, int a4, int a5, int a6, int a7);
	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* forwardVec, RealVector3D* upVec);
	bool Forge_UpdatePlayerEditorGlobalsHook(int16_t playerIndex, uint32_t* pObjectIndex);
	bool Forge_SpawnItemCheckHook(uint32_t tagIndex, RealVector3D *position, uint32_t unitObjectIndex);
	void SandboxEngineInitHook();
	void SandboxEngineShutdownHook();
	void SandboxEngineTickHook();
	void __fastcall SandboxEngineObjectDisposeHook(void* thisptr, void* unused, uint32_t objectIndex);
	void* ObjectPropertiesUIAllocateHook(int size);
	void* ObjectCreationUIAllocateHook(int size);
	void RenderMeshPartHook(void* data, int a2);
	void UpdateObjectCachedColorPermutationRenderStateHook(uint32_t objectIndex, bool invalidated);
	int CreateOrGetBudgetForItem(Blam::MapVariant *thisptr, int tagIndex);

	int LightmapHook(RealVector3D *origin, RealVector3D *direction, int a3, int objectIndex, char a5, char a6, void *a7);
	bool __cdecl sub_980E40_hook(int a1, uint32_t objectIndex);

	void __fastcall MapVariant_SyncObjectPropertiesHook(Blam::MapVariant* thisptr, void* unused,
		Blam::MapVariant::VariantProperties *placementProps, uint32_t objectIndex);
	void __cdecl MapVariant_SyncVariantPropertiesHook(Blam::MapVariant::VariantPlacement *placement);
	void __fastcall sub_584CF0_hook(MapVariant *thisptr, void *unused);
	void __fastcall MapVariant_SyncObjectProperties_Object_Properties_Hook(Blam::MapVariant* thisptr, void* unused,
		Blam::MapVariant::VariantProperties *placementProps, uint32_t objectIndex);

	void __fastcall c_map_variant_initialize_from_scenario_hook(Blam::MapVariant *thisptr, void* unused);
	void __fastcall c_map_variant_update_item_budget_hook(Blam::MapVariant *thisptr, void* unused, int budgetIndex, char arg_4);
	void __fastcall c_map_variant_copy_budget_hook(Blam::MapVariant *thisptr, void *unused, Blam::MapVariant *other);

	void MapVariant_SpawnObjectHook();

	void __fastcall c_game_engine_object_runtime_manager__on_object_spawned_hook(void *thisptr, void *unused, int16_t placementIndex, uint32_t objectIndex);

	void UpdateLightHook(uint32_t lightDatumIndex, int a2, float intensity, int a4);
	uint32_t __fastcall SpawnItemHook(MapVariant *thisptr, void *unused, uint32_t tagIndex, int a3, int placementIndex,
		RealVector3D *position, RealVector3D *forward, RealVector3D *up,
		int scenarioPlacementIndex, int objectType, uint8_t *placementProps, uint16_t placementFlags);

	void sandbox_zone_shape_render_hook(ZoneShape *zone, float *color, uint32_t objectIndex);

	struct ScreenEffectData;
	void ScreenEffectsHook(RealVector3D *a1, RealVector3D *a2, ScreenEffectData *renderData, void *a4, int localPlayerIndex);
	void GameEngineTickHook();

	bool __fastcall sub_724890_hook(void *thisptr, void *unused, int a2, int a3, float *matrix);

	void __fastcall CameraFxHook(void *thisptr, void *unused, void *a2);
	void __cdecl ShieldImpactBloomHook(int id, int count, float *data);

	void GrabSelection(uint32_t playerIndex);
	void DoClone(uint32_t playerIndex, uint32_t objectIndexUnderCrosshair);
	void HandleMovementSpeed();
	void HandleSpawnItem();

	std::queue<std::function<void()>> s_SandboxTickCommandQueue;
	RealVector3D s_GrabOffset;

	const float MONITOR_MOVEMENT_SPEEDS[] = { 0.001f, 0.05f, 0.25f, 1.0f, 2.0f, 4.0f };


	std::vector<Patches::Forge::ItemSpawnedCallback> s_ItemSpawnedCallbacks;
	uint32_t s_SpawnItemTagIndex = -1;
	uint32_t s_SpawnItemPlayerIndex = -1;

	struct SandboxPaletteItem
	{
		Blam::Tags::TagReference Object;
		uint32_t Name;
		int32_t MaxAllowed;
		float Cost;
		uint8_t Unknown1C[0x14];
	};
	static_assert(sizeof(SandboxPaletteItem) == 0x30, "Invalid SandboxPaletteItem size");

	struct {
		float Exposure;
		float LightIntensity;
		float BloomIntensity;
	} s_CameraFxSettings;
}

namespace Patches::Forge
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
		Hook(0x734EE, ObjectSpawnedHook, HookFlags::IsCall).Apply();
		Hook(0x7AF758, UnitFlyingHook, HookFlags::IsCall).Apply();
		Hook(0x19CAFC, UpdateHeldObjectTransformHook, HookFlags::IsCall).Apply();
		Hook(0x19F4CA, Forge_UpdatePlayerEditorGlobalsHook, HookFlags::IsCall).Apply();

		Hook(0xAD4DD, MapVariant_SyncObjectPropertiesHook, HookFlags::IsCall).Apply();
		Hook(0x180E66, MapVariant_SyncObjectProperties_Object_Properties_Hook, HookFlags::IsCall).Apply();
		Hook(0x181326, MapVariant_SyncObjectPropertiesHook, HookFlags::IsCall).Apply();
		Hook(0x1824EA, MapVariant_SyncObjectPropertiesHook, HookFlags::IsCall).Apply();
		Hook(0x185BBD, MapVariant_SyncObjectPropertiesHook, HookFlags::IsCall).Apply();

		// hook the object properties menu to show the cef one
		Hook(0x6E25A0, ObjectPropertiesUIAllocateHook, HookFlags::IsCall).Apply();
		Hook(0x6E26C1, ObjectPropertiesUIAllocateHook, HookFlags::IsCall).Apply();
		// hook the object creation menu to show the cef one
		Hook(0x6E2729, ObjectCreationUIAllocateHook, HookFlags::IsCall).Apply();
		// disable legal screen
		Patch(0x6EAADD, { 0xEB }).Apply();
		Patch(0x6EAA37, { 0xEB }).Apply();

		// facilitate swapping materials based on shared storage
		Hook(0x678B9E, RenderMeshPartHook, HookFlags::IsCall).Apply();
		Hook(0x679136, RenderMeshPartHook, HookFlags::IsCall).Apply(); // transparent
		// disable the lightmap for reforge objects
		Hook(0x7BEEEE, LightmapHook, HookFlags::IsCall).Apply();

		// prevent the object from lurching forward when being rotated
		Pointer(0x0059FA95 + 4).Write((float*)&HELDOBJECT_DISTANCE_MIN);
		// slow down in/out movement
		Pointer(0x0059FA7A + 4).Write((float*)&HELDOBJECT_DISTANCE_CHANGE_MULTIPLIER);

		// enable teleporter volume editing compliments of zedd
		Patch::NopFill(Pointer::Base(0x6E4796), 0x66);

		// increase forge item limit
		Hook(0x151506, c_map_variant_initialize_from_scenario_hook, HookFlags::IsCall).Apply();
		Hook(0x185DC1, c_map_variant_update_item_budget_hook, HookFlags::IsCall).Apply();
		Hook(0x182110, MapVariant_SpawnObjectHook).Apply();
		Hook(0x19AEE6, SpawnItemHook, HookFlags::IsCall).Apply();
		// also removes bounding radius check
		Hook(0x19AEBA, Forge_SpawnItemCheckHook, HookFlags::IsCall).Apply();
		Pointer(0xA669E4 + 1).Write(uint32_t(&UpdateLightHook));
		// fix incorrect runtime when reloading
		Hook(0x181CC0, c_map_variant_copy_budget_hook).Apply();

		Hook(0x639986, ScreenEffectsHook, HookFlags::IsCall).Apply();

		Hook(0x645160, UpdateObjectCachedColorPermutationRenderStateHook).Apply();

		// allow arbitrary data to be stored in variant properties
		Patch(0x18676E, { 0x90,0x90 }).Apply();

		// prevent serializing/deserialzing the map variant on save to keep precision
		Hook(0x184CF0, sub_584CF0_hook).Apply();

		// allow all object types to be forge
		Patch(0x181C90, { 0xB0, 0x01, 0xC3 }).Apply();
		Patch::NopFill(Pointer::Base(0x185655), 6);

		Hook(0x1337F1, GameEngineTickHook, HookFlags::IsCall).Apply();

		// disble projectile collisions on invisible material
		Hook(0x2D8F82, sub_980E40_hook, HookFlags::IsCall).Apply();

		Hook(0x19F0B8, sandbox_zone_shape_render_hook, HookFlags::IsCall).Apply();

		// fix jump canceling on forged objects
		Hook(0x3245FD, sub_724890_hook, HookFlags::IsCall).Apply();

		Hook(0x19004E, c_game_engine_object_runtime_manager__on_object_spawned_hook, HookFlags::IsCall).Apply();

		Hook(0x00639A68, CameraFxHook, HookFlags::IsCall).Apply();
		Hook(0x00653D77, ShieldImpactBloomHook, HookFlags::IsCall).Apply();
	}

	void Tick()
	{
		UpdateMapModifier();
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

	void SpawnItem(uint32_t tagIndex)
	{
		s_SpawnItemTagIndex = tagIndex;
	}

	void OnItemSpawned(ItemSpawnedCallback callback)
	{
		s_ItemSpawnedCallbacks.push_back(callback);
	}

	void SetPrematchCamera()
	{
		s_SandboxTickCommandQueue.push([=]()
		{
			PrematchCamera::PlaceCameraObject();
		});
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
		Forge::SelectionRenderer::SetEnabled(false);
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

		HandleSpawnItem();

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
			HandleMovementSpeed();

			if (heldObjectIndex == -1)
			{
				auto cloneAction = GetActionState(Blam::Input::eGameActionUiB);
				if ((!(cloneAction->Flags & Blam::Input::eActionStateFlagsHandled) && cloneAction->Ticks == 1))
				{
					cloneAction->Flags |= Blam::Input::eActionStateFlagsHandled;

					if (Blam::Network::GetActiveSession()->IsHost())
						DoClone(playerIndex, objectIndexUnderCrosshair);
					else
					{
						PrintKillFeedText(0, L"Must be host to clone objects", 0);
						UI_PlaySound(0, -1); // error
					}
				}
			}

			if (Blam::Input::GetKeyTicks(Blam::Input::eKeyCodeM, Blam::Input::eInputTypeGame) == 1
				|| (!(Blam::Input::GetActionState(Blam::Input::eGameActionUiDown)->Flags & Blam::Input::eActionStateFlagsHandled)
					&& Blam::Input::GetActionState(Blam::Input::eGameActionUiDown)->Ticks == 1))
			{
				auto prevValue = moduleForge.VarMagnetsEnabled->ValueInt;
				std::string previousValueStr;

				auto& commandMap = Modules::CommandMap::Instance();
				commandMap.SetVariable(moduleForge.VarMagnetsEnabled, std::to_string(prevValue ? 0 : 1), previousValueStr);
				if (!prevValue)
					PrintKillFeedText(0, L"Magnets Enabled", 0);
				else
					PrintKillFeedText(0, L"Magnets Disabled", 0);
			}
		}
		else
		{
			Forge::SelectionRenderer::SetEnabled(false);
		}
	}

	void UpdateMapModifier()
	{
		// Scan the object table to check if map modifier is spawned
		for (auto &&header : Blam::Objects::GetObjects())
		{
			if (header.Type != eObjectTypeScenery)
				continue;
			auto tagIndex = header.GetTagIndex().Index();
			if (tagIndex == 0x5728)
			{
				auto mpProperties = header.Data->GetMultiplayerProperties();
				auto mapModifierProperties = (Forge::ForgeMapModifierProperties*)&mpProperties->RadiusWidth;

				killBarriersEnabled = !(mapModifierProperties->Flags & Forge::ForgeMapModifierProperties::eMapModifierFlags_DisableDeathBarrier);
				pushBarriersEnabled = !(mapModifierProperties->Flags & Forge::ForgeMapModifierProperties::eMapModifierFlags_DisablePushBarrier);

				s_CameraFxSettings.Exposure = std::pow((float)mapModifierProperties->CameraFxExposure * 0.05f, 2.2f) * 0.01f;
				auto expScale = s_CameraFxSettings.Exposure;
				if (std::abs(s_CameraFxSettings.Exposure) < 0.0001f)
					expScale = 1.0f;
				s_CameraFxSettings.LightIntensity = ((float)mapModifierProperties->CameraFxLightIntensity / expScale) * 0.07f;
				if (s_CameraFxSettings.LightIntensity > 20000.0f)
					s_CameraFxSettings.LightIntensity = 20000.0f;
				s_CameraFxSettings.BloomIntensity = ((float)mapModifierProperties->CameraFxBloom / expScale) * 0.07f;
				if (s_CameraFxSettings.BloomIntensity > 20000.0f)
					s_CameraFxSettings.BloomIntensity = 20000.0f;

				return;
			}
		}

		// not found
		killBarriersEnabled = true;
		pushBarriersEnabled = true;

		s_CameraFxSettings.Exposure = 0;
		s_CameraFxSettings.LightIntensity = 0;
		s_CameraFxSettings.BloomIntensity = 0;
	}

	bool CheckKillTriggersHook(int a0, void *a1)
	{
		if (!killBarriersEnabled)
			return false;

		typedef bool(*CheckKillTriggersPtr)(int a0, void *a1);
		auto CheckKillTriggers = reinterpret_cast<CheckKillTriggersPtr>(0x68C410);
		return CheckKillTriggers(a0, a1);
	}

	bool ObjectSafeZoneHook(void *a0)
	{
		if (!killBarriersEnabled)
			return true;

		typedef bool(*CheckSafeZonesPtr)(void *a0);
		auto CheckSafeZones = reinterpret_cast<CheckSafeZonesPtr>(0x4EB130);
		return CheckSafeZones(a0);
	}

	void* PushBarriersGetStructureDesignHook(int index)
	{
		if (!pushBarriersEnabled)
			return nullptr; // Return a null sddt if push barriers are disabled

		typedef void*(*GetStructureDesignPtr)(int index);
		auto GetStructureDesign = reinterpret_cast<GetStructureDesignPtr>(0x4E97D0);
		return GetStructureDesign(index);
	}

	void HandleSpawnItem()
	{
		if (s_SpawnItemTagIndex == -1)
			return;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == DatumIndex::Null || !GetEditorModeState(playerIndex, nullptr, nullptr))
			return;

		Blam::Tags::TagInstance instance(s_SpawnItemTagIndex);
		auto def = instance.GetDefinition<void>();
		auto groupTag = def ? instance.GetGroupTag() : 0;

		s_SpawnItemTagIndex = -1;

		if (!def || !(groupTag == 'weap' || groupTag == 'vehi' || groupTag == 'eqip'
			|| groupTag == 'scen' || groupTag == 'bloc' || groupTag == 'crea'
			|| groupTag == 'efsc' || groupTag == 'proj' || groupTag == 'bipd'
			|| groupTag == 'mach' || groupTag == 'ctrl' || groupTag == 'gint'
			|| groupTag == 'ssce'))
		{
			wchar_t buff[256];
			swprintf_s(buff, L"Tag not loaded or not a valid forge object [0x%X4]", instance.Index);
			PrintKillFeedText(0, buff, 0);
			return;
		}

		ForgeMessage msg = { 0 };
		msg.Type = 0;
		msg.PlayerIndex = playerIndex;
		msg.TagIndex = instance.Index;
		msg.CrosshairPoint = GetSandboxGlobals().CrosshairPoints[playerIndex.Index()];

		static auto Forge_SendMessage = (void(*)(ForgeMessage*))(0x004735D0);
		Forge_SendMessage(&msg);
	}

	void HandleRotationReset()
	{
		using namespace Blam::Input;

		auto uiUpAction = GetActionState(eGameActionUiUp);
		if ((!(uiUpAction->Flags & eActionStateFlagsHandled) && uiUpAction->Ticks == 1)
			|| GetMouseButtonTicks(eMouseButtonMiddle, eInputTypeGame) == 1)
		{
			uiUpAction->Flags |= Blam::Input::eActionStateFlagsHandled;
			Forge::RotationSnap::RotateToScripted(RealQuaternion());
		}
	}

	void HandleMovementSpeed()
	{
		using namespace Blam::Input;

		static auto Object_SetVelocity = (void(__cdecl *)(uint32_t objectIndex, RealVector3D* a2, RealVector3D *a3))(0x00B34040);
		static auto& moduleForge = Modules::ModuleForge::Instance();

		auto player = Blam::Players::GetPlayers().Get(Blam::Players::GetLocalPlayer(0));
		if (!player)
			return;

		auto movementSpeedAction = GetActionState(eGameActionUiLeftStick);
		if ((!(movementSpeedAction->Flags & eActionStateFlagsHandled) && movementSpeedAction->Ticks == 1) ||
			GetKeyTicks(eKeyCodeF, eInputTypeGame) == 1)
		{
			movementSpeedAction->Flags |= Blam::Input::eActionStateFlagsHandled;

			auto currentSpeed = moduleForge.VarMonitorSpeed->ValueInt;

			currentSpeed = (currentSpeed + 1) %
				(sizeof(MONITOR_MOVEMENT_SPEEDS) / sizeof(MONITOR_MOVEMENT_SPEEDS[0]));

			wchar_t buff[256];
			switch (currentSpeed)
			{
			case 0:
				swprintf_s(buff, 256, L"Movement Speed: 0.001 (Z-Fight Fixer)");
				break;
			case 1:
				swprintf_s(buff, 256, L"Movement Speed: Slower");
				break;
			case 2:
				swprintf_s(buff, 256, L"Movement Speed: Slow");
				break;
			case 3:
				swprintf_s(buff, 256, L"Movement Speed: Normal");
				break;
			case 4:
				swprintf_s(buff, 256, L"Movement Speed: Fast");
				break;
			case 5:
				swprintf_s(buff, 256, L"Movement Speed: Faster");
				break;
			}

			std::string prev;
			Modules::CommandMap::Instance().SetVariable(moduleForge.VarMonitorSpeed, std::to_string(currentSpeed), prev);

			RealVector3D v1 = {}, v2 = {};
			Object_SetVelocity(player->SlaveUnit, &v1, &v2);

			PrintKillFeedText(0, buff, 0);
		}
	}

	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot)
	{
		static auto RotateHeldObject = (void(__stdcall*)(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot))(0x0059DD50);

		static auto& moduleForge = Modules::ModuleForge::Instance();
		const auto currentSnap = moduleForge.VarRotationSnap->ValueInt;
		const auto rotationSensitvity = moduleForge.VarRotationSensitivity->ValueFloat;

		if (DatumIndex(playerIndex) != Blam::Players::GetLocalPlayer(0))
		{
			RotateHeldObject(playerIndex, objectIndex, xRot, yRot, zRot);
			return;
		}

		xRot *= rotationSensitvity * HELDOBJECT_ROTATION_SENSITIVTY_BASE;
		yRot *= rotationSensitvity * HELDOBJECT_ROTATION_SENSITIVTY_BASE;
		zRot *= rotationSensitvity * HELDOBJECT_ROTATION_SENSITIVTY_BASE;

		HandleRotationReset();

		if (currentSnap < 1)
		{
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

		const float DEAD_ZONE = 0.5f * rotationSensitvity * HELDOBJECT_ROTATION_SENSITIVTY_BASE;
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
		if (selection.Contains(objectIndex))
		{
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
		}

		if (ObjectIsPhased(objectIndex))
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

			auto object = Blam::Objects::Get(objectIndex);
			if (!object)
				continue;
			if (CreateOrGetBudgetForItem(mapv, object->TagIndex) == -1)
				continue;

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

		// prevent from deleting objects behind the held object
		uint32_t heldObjectIndex, crosshairObjectIndex;
		auto editorState = GetEditorModeState(playerIndex, &heldObjectIndex, &crosshairObjectIndex);
		if (heldObjectIndex != -1 && crosshairObjectIndex != -1)
		{
			auto heldObject = Blam::Objects::Get(heldObjectIndex);
			if (placementIndex != heldObject->PlacementIndex)
				placementIndex = heldObject->PlacementIndex;
		}

		auto mapv = GetMapVariant();
		const auto& selection = Forge::Selection::GetSelection();
		auto deletedObjectIndex = mapv->Placements[placementIndex].ObjectIndex;

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

	void ObjectSpawnedHook(uint32_t tagIndex, uint32_t playerIndex, const RealVector3D* position)
	{
		static auto ObjectSpawned = (void(*)(uint32_t tagIndex, uint32_t playerIndex, const RealVector3D* position))(0x0059AE50);

		auto& sandboxGlobals = GetSandboxGlobals();

		if (playerIndex = Blam::Players::GetLocalPlayer(0))
			s_GrabOffset = RealVector3D(0, 0, 0);

		ObjectSpawned(tagIndex, playerIndex, position);
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
			monitorSpeed *= MONITOR_MOVEMENT_SPEEDS[moduleForge.VarMonitorSpeed->ValueInt];

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
			if (!(uiRightBumper->Flags & Blam::Input::eActionStateFlagsHandled) && uiRightBumper->Ticks > 0)
				direction = 1;
			else if (!(uiLeftBumper->Flags & Blam::Input::eActionStateFlagsHandled) && uiLeftBumper->Ticks > 0)
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

	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* forwardVec, RealVector3D* upVec)
	{
		static auto Object_Transform = (void(*)(bool a1, int objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);

		auto heldObject = Blam::Objects::Get(objectIndex);
		if (!heldObject)
			return;

		DatumIndex playerIndex = GetPlayerHoldingObject(objectIndex);

		const auto& selection = Forge::Selection::GetSelection();

		if (ObjectIsPhased(objectIndex))
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

			auto sandboxGlobals = Forge::GetSandboxGlobals();
			const RealVector3D& intersectNormal = sandboxGlobals.CrosshairIntersectNormals[playerIndex & 0xF];

			auto objectIndexToClone = objectIndexUnderCrosshair;
			for (auto i = 0; i < cloneMultiplier; i++)
			{
				objectIndexToClone = CloneObject(playerIndex, objectIndexToClone, cloneDepth, intersectNormal);
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

	bool CanThemeObject(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;

		auto objectDef = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<uint8_t>();
		if (!objectDef)
			return false;
		auto hlmtDef = Blam::Tags::TagInstance(*(uint32_t*)(objectDef + 0x40)).GetDefinition<uint8_t>();
		if (!hlmtDef)
			return false;
		auto modeTagIndex = *(uint32_t*)(hlmtDef + 0xC);

		const auto modeDefinitionPtr = Pointer(Blam::Tags::TagInstance(modeTagIndex).GetDefinition<uint8_t>());
		if (!modeDefinitionPtr)
			return false;

		const auto materialCount = modeDefinitionPtr(0x48).Read<int32_t>();
		const auto& firstMaterialShaderTagRef = modeDefinitionPtr(0x4c)[0].Read<Blam::Tags::TagReference>();
		if (!materialCount || firstMaterialShaderTagRef.TagIndex != REFORGE_DEFAULT_SHADER)
			return false;

		return true;
	}

	bool GetObjectMaterial(void* renderData, int16_t* pMaterialIndex)
	{
		auto modeTagIndex = Pointer(renderData)(0x4).Read<uint32_t>();
		auto objectIndex = Pointer(renderData)(0x6c).Read<uint32_t>();

		const auto modeDefinitionPtr = Pointer(Blam::Tags::TagInstance(modeTagIndex).GetDefinition<uint8_t>());
		if (!modeDefinitionPtr)
			return false;

		const auto materialCount = modeDefinitionPtr(0x48).Read<int32_t>();
		const auto& firstMaterialShaderTagRef = modeDefinitionPtr(0x4c)[0].Read<Blam::Tags::TagReference>();
		if (!materialCount || firstMaterialShaderTagRef.TagIndex != REFORGE_DEFAULT_SHADER)
			return false;

		const auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;

		auto mpProps = object->GetMultiplayerProperties();
		if (!mpProps)
			return false;

		auto sharedStorage = Pointer(mpProps)(0x5).Read<uint8_t>();

		auto materialIndex = int16_t(sharedStorage);

		if (int32_t(materialIndex) >= materialCount)
			return false;

		*pMaterialIndex = materialIndex;

		return true;
	}

	void RenderMeshPartHook(void* data, int a2)
	{
		const auto is_forge = (bool(*)())(0x0059A780);

		static auto RenderMeshPart = (void(*)(void* data, int a2))(0xA78940);

		auto renderData = Pointer(data)[4];

		auto objectIndex = Pointer(renderData)(0x6c).Read<uint32_t>();
		auto object = Blam::Objects::Get(objectIndex);
		if (object)
		{
			switch (object->TagIndex)
			{
			case Forge::Volumes::KILL_VOLUME_TAG_INDEX:
			{
				auto properties = object->GetMultiplayerProperties();
				if (properties && ((Forge::ForgeKillVolumeProperties*)&properties->TeleporterChannel)->Flags
					& Forge::ForgeKillVolumeProperties::eKillVolumeFlags_AlwaysVisible)
					break;
			}
			case Forge::PrematchCamera::CAMERA_OBJECT_TAG_INDEX:
			case Forge::Volumes::GARBAGE_VOLUME_TAG_INDEX:
				if (!is_forge())
					return;
			}

			int16_t materialIndex;
			if (GetObjectMaterial(renderData, &materialIndex))
			{
				if (materialIndex == int16_t(INVISIBLE_MATERIAL_INDEX))
				{
					if (!Modules::ModuleForge::Instance().VarShowInvisibles->ValueInt
						&& !Forge::GetEditorModeState(Blam::Players::GetLocalPlayer(0), nullptr, nullptr))
						return;
				}

				auto modeTagIndex = Pointer(renderData)(0x4).Read<uint32_t>();
				const auto modeDefinitionPtr = Pointer(Blam::Tags::TagInstance(modeTagIndex).GetDefinition<uint8_t>());
				const auto meshPart = modeDefinitionPtr(0x6C)[0](0x4)[0];
				const auto materialIndexPtr = (uint16_t*)modeDefinitionPtr(0x6C)[0](4)[0];

				auto oldMaterialIndex = *materialIndexPtr;
				*materialIndexPtr = materialIndex;
				RenderMeshPart(data, a2);
				*materialIndexPtr = oldMaterialIndex;

				return;
			}
		}
		RenderMeshPart(data, a2);
	}

	int LightmapHook(RealVector3D *origin, RealVector3D *direction, int a3, int objectIndex, char a5, char a6, void *a7)
	{
		static auto LightmapFunc = (int(*)(RealVector3D *origin, RealVector3D *direction, int a3, int objectIndex, char a5, char a6, void *a7))(0x752B10);

		if (CanThemeObject(objectIndex))
		{
			memcpy(a7, (uint8_t*)0x18F3000, 0x1F8u);
			*((uint8_t*)a7 + 0x1F0) = 0;
			return 1;
		}

		return LightmapFunc(origin, direction, a3, objectIndex, a5, a6, a7);
	}

	void* ObjectPropertiesUIAllocateHook(int size)
	{
		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		uint32_t objectIndexUnderCrosshair, heldObjectIndex;
		if (Forge::GetEditorModeState(playerIndex, &heldObjectIndex, &objectIndexUnderCrosshair))
		{
			auto currentObjectIndex = heldObjectIndex != -1 ? heldObjectIndex : objectIndexUnderCrosshair;
			auto currentObject = Blam::Objects::Get(currentObjectIndex);
			if (currentObject && currentObject->PlacementIndex != -1)
			{
				auto mapv = Forge::GetMapVariant();
				auto placement = mapv->Placements[currentObject->PlacementIndex];

				Web::Ui::WebForge::ShowObjectProperties(currentObjectIndex);
			}
		}
		return nullptr;
	}

	void* ObjectCreationUIAllocateHook(int size)
	{
		Web::Ui::WebForge::ShowObjectCreation();
		return nullptr;
	}

	bool Forge_UpdatePlayerEditorGlobalsHook(int16_t playerIndex, uint32_t* pObjectIndex)
	{
		static auto Forge_UpdatePlayerEditorGlobals = (bool(*)(int16_t playerIndex, uint32_t* pObjectIndex))(0x0059E720);

		if (Blam::Players::GetLocalPlayer(0).Index() == playerIndex)
		{
			auto& editorGlobals = Forge::GetSandboxGlobals();

			if (editorGlobals.HeldObjects[playerIndex] == -1)
			{
				auto oldDistance = editorGlobals.HeldObjectDistances[playerIndex];
				editorGlobals.HeldObjectDistances[playerIndex] = Modules::ModuleForge::Instance().VarMaxGrabDistance->ValueFloat;
				auto ret = Forge_UpdatePlayerEditorGlobals(playerIndex, pObjectIndex);
				editorGlobals.HeldObjectDistances[playerIndex] = oldDistance;
				return ret;
			}
		}

		return Forge_UpdatePlayerEditorGlobals(playerIndex, pObjectIndex);
	}

	void __fastcall MapVariant_SyncObjectPropertiesHook(Blam::MapVariant* thisptr, void* unused,
		Blam::MapVariant::VariantProperties *properties, uint32_t objectIndex)
	{
		const auto MapVariant_SyncObjectProperties = (void(__thiscall *)(Blam::MapVariant* thisptr,
			Blam::MapVariant::VariantProperties *properties, uint32_t objectIndex))(0x00580E80);
		const auto sub_59A620 = (void(__cdecl *)(int objectIndex, char a2))(0x59A620);

		MapVariant_SyncObjectProperties(thisptr, properties, objectIndex);

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return;

		auto mpProperties = object->GetMultiplayerProperties();
		if (properties->EngineFlags)
			mpProperties->EngineFlags = properties->EngineFlags;


		object->HavokFlags |= 0x200u; // at rest
		object->HavokFlags &= ~0x100000u;

		// physics
		if (properties->ZoneShape == 4)
		{
			object->HavokFlags &= ~0x100000u;
			object->HavokFlags |= 0x2C8;
			sub_59A620(objectIndex, 1);
		}
	}

	void __fastcall MapVariant_SyncObjectProperties_Object_Properties_Hook(Blam::MapVariant* thisptr, void* unused,
		Blam::MapVariant::VariantProperties *properties, uint32_t objectIndex)
	{
		const auto MapVariant_SyncObjectProperties = (void(__thiscall *)(Blam::MapVariant* thisptr,
			Blam::MapVariant::VariantProperties *properties, uint32_t objectIndex))(0x00580E80);
		const auto sub_59A620 = (void(__cdecl *)(int objectIndex, char a2))(0x59A620);

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return;

		auto mpProperties = object->GetMultiplayerProperties();
		auto oldZoneShape = mpProperties->Shape;

		if(properties->EngineFlags)
			mpProperties->EngineFlags = properties->EngineFlags;

		MapVariant_SyncObjectProperties(thisptr, properties, objectIndex);

		if (oldZoneShape != properties->ZoneShape)
		{
			// physics
			if (properties->ZoneShape == 4)
			{
				object->HavokFlags &= ~0x100000u;
				object->HavokFlags |= 0x2C8;
				sub_59A620(objectIndex, 1);
			}
			else if (object->HavokFlags & 0x2C8 && oldZoneShape == 4)
			{
				object->HavokFlags &= ~0x2C8;
				object->HavokFlags &= ~0x100000u;
				object->HavokFlags |= 0x80;
				sub_59A620(objectIndex, 1);
			}
		}
	}


	SandboxPaletteItem* FindSandboxPaletteItem(uint32_t tagIndex)
	{
		using namespace Blam::Tags;
		auto scenarioDef = Blam::Tags::Scenario::GetCurrentScenario();

		for (auto i = 0; i < 7; i++)
		{
			auto palette = ((TagBlock<SandboxPaletteItem>*)&scenarioDef->SandboxVehicles) + i;
			for (auto j = 0; j < palette->Count; j++)
			{
				if (palette->Elements[j].Object.TagIndex == tagIndex)
					return &palette->Elements[j];
			}
		}

		return nullptr;
	}

	void __fastcall c_map_variant_initialize_from_scenario_hook(Blam::MapVariant *thisptr, void* unused)
	{
		struct s_scenario_palette_item
		{
			Blam::Tags::TagReference Object;
			uint32_t Unknown10;
			uint32_t Unknown14;
			uint32_t Unknown18;
			uint32_t Unknown1c;
			uint32_t Unknown20;
			uint32_t Unknown24;
			uint32_t Unknown28;
			uint32_t Unknown2c;
		};

		const auto c_map_variant_initialize = (void(__thiscall *)(MapVariant *thisptr, int mapId))(0x00581F70);
		const auto crc32 = (uint32_t(*)(uint32_t def, uint8_t *data, long size))(0x0052CD20);
		const auto sub_B734F0 = (void*(*)(int objectType))(0x00B734F0);
		const auto scenario_get_placement_block = (void*(*)(void *scenario, int objectType, long *pSize))(0x00B5A880);
		const auto scenario_get_palette_block = (Blam::Tags::TagBlock<s_scenario_palette_item>*(*)(void *scenario, int objectType))(0x00B5A8B0);

		auto scenario = *(Blam::Tags::Scenario::Scenario**)0x022AAEB4;
		auto mapid = scenario->MapId;

		c_map_variant_initialize(thisptr, mapid);

		auto cacheFileHash = (uint8_t*)0x22AB34C;
		thisptr->Unknown12C = crc32(-1, cacheFileHash, 256);

		thisptr->ScnrPlacementsCount = 0;
		thisptr->BudgetEntryCount = 0;

		auto totalObjectCount = 0;
		auto scnrPlacementOffset = 0;
		for (auto objectType = 0; objectType < 15; objectType++)
		{
			if (objectType == Blam::Objects::ObjectType::eObjectTypeScenery
				|| objectType == Blam::Objects::ObjectType::eObjectTypeCrate
				|| objectType == Blam::Objects::ObjectType::eObjectTypeVehicle
				|| objectType == Blam::Objects::ObjectType::eObjectTypeWeapon
				|| objectType == Blam::Objects::ObjectType::eObjectTypeEquipment)
			{
				auto v6 = (uint8_t*)sub_B734F0(objectType);
				if (-1 != *(int16_t *)(v6 + 0xA) && -1 != *(int16_t *)(v6 + 0xC))
				{
					auto placementBlock = scenario_get_placement_block(scenario, objectType, nullptr);

					auto placementCount = *(int32_t*)placementBlock;
					thisptr->ScnrIndices[objectType] = scnrPlacementOffset;
					thisptr->UsedPlacementsCount += placementCount;
					scnrPlacementOffset += placementCount;
				}

				auto palette = scenario_get_palette_block(scenario, objectType);
				for (auto i = 0; i < palette->Count; i++)
				{
					auto& paletteEntry = palette->Elements[i];
					if (paletteEntry.Object.TagIndex == -1)
						continue;

					auto& budgetEntry = thisptr->Budget[thisptr->BudgetEntryCount];

					// inefficient, but only done once
					auto sandboxPaletteItem = FindSandboxPaletteItem(paletteEntry.Object.TagIndex);
					if (!sandboxPaletteItem)
						continue;

					budgetEntry.TagIndex = paletteEntry.Object.TagIndex;
					if (sandboxPaletteItem->MaxAllowed)
						budgetEntry.DesignTimeMax = sandboxPaletteItem->MaxAllowed;
					else
						budgetEntry.DesignTimeMax = -1;
					budgetEntry.CountOnMap = 0;
					budgetEntry.RuntimeMin = 0;
					budgetEntry.RuntimeMax = 0;
					budgetEntry.Cost = sandboxPaletteItem->Cost;

					thisptr->BudgetEntryCount++;
				}
			}
		}

		thisptr->ScnrPlacementsCount = thisptr->UsedPlacementsCount;
		thisptr->MaxBudget = scenario->SandboxBudget;
	}

	int CreateOrGetBudgetForItem(Blam::MapVariant *thisptr, int tagIndex)
	{
		static auto c_map_variant_get_budget_index_for_item = (int(__thiscall*)(Blam::MapVariant *thisptr, int tagIndex))(0x005831C0);
		const auto sub_4B2570 = (void(*)(void *a1))(0x4B2570);
		const auto  game_simulation_is_client = (bool(*)())(0x005319C0);

		auto index = c_map_variant_get_budget_index_for_item(thisptr, tagIndex);
		if (index != -1)
			return index;

		if (thisptr->BudgetEntryCount >= 256)
		{
			for (auto i = 0; i < thisptr->BudgetEntryCount; i++)
			{
				const auto& budget = thisptr->Budget[i];
				if (budget.TagIndex == -1)
				{
					index = i;
					break;
				}
			}
		}
		else
		{
			index = thisptr->BudgetEntryCount;
			thisptr->BudgetEntryCount++;
		}

		if (index != -1)
		{
			auto& budget = thisptr->Budget[index];

			auto sandboxPaletteItem = FindSandboxPaletteItem(tagIndex);
			if (!sandboxPaletteItem)
				return -1;

			budget.TagIndex = tagIndex;
			if (sandboxPaletteItem->MaxAllowed)
				budget.DesignTimeMax = sandboxPaletteItem->MaxAllowed;
			else
				budget.DesignTimeMax = -1;
			budget.CountOnMap = 0;
			budget.RuntimeMin = 0;
			budget.RuntimeMax = 0;
			budget.Cost = sandboxPaletteItem->Cost;

		}

		if (index == -1)
		{
			wchar_t buff[256];
			_swprintf(buff, L"Unable to create budget for 0x%x", tagIndex);
			PrintKillFeedText(0, buff, 0);
		}

		return index;
	}

	void __fastcall c_map_variant_update_item_budget_hook(Blam::MapVariant *thisptr, void* unused, int budgetIndex, char arg_4)
	{
		static auto c_map_variant_update_item_budget = (void(__thiscall *)(Blam::MapVariant *thisptr, int budgetIndex, char arg_4))(0x00584E10);
		c_map_variant_update_item_budget(thisptr, budgetIndex, arg_4);
		const auto IsForge = (bool(*)())(0x0059A780);

		auto& budget = thisptr->Budget[budgetIndex];
	
		if (IsForge())
		{
			auto objectDef = Blam::Tags::TagInstance(budget.TagIndex).GetDefinition<Blam::Tags::Objects::Object>();
			if (objectDef && objectDef->MultiplayerProperties.Count)
			{
				auto engineFlags = objectDef->MultiplayerProperties.Elements[0].EngineFlags;
				if (uint32_t(engineFlags))
					return;
			}
		}
	
		if (!budget.CountOnMap)
		{
			budget.TagIndex = -1;
			budget.Cost = -1;
			budget.CountOnMap = 0;
			budget.DesignTimeMax = -1;
			budget.RuntimeMin = -1;
			budget.RuntimeMax = -1;
		}
	}

	void __fastcall c_map_variant_copy_budget_hook(Blam::MapVariant *thisptr, void *unused, Blam::MapVariant *other)
	{
		for (auto i = 0; i < other->BudgetEntryCount; i++)
		{
			auto &budget = other->Budget[i];
			if (budget.TagIndex != -1)
			{
				auto newBudgetIndex = CreateOrGetBudgetForItem(thisptr, budget.TagIndex);
				if (newBudgetIndex != -1)
				{
					auto &newBudget = thisptr->Budget[newBudgetIndex];
					newBudget = budget;
				}
			}
		}
	}

	__declspec(naked) void MapVariant_SpawnObjectHook()
	{
		__asm
		{
			push ebp
			mov ebp, esp
			sub esp, 0x194
			pushad
			push[ebp + 0x8]
			push ecx
			call CreateOrGetBudgetForItem
			add esp, 8
			popad
			mov eax, 0x00582119
			jmp eax
		}
	}

	uint32_t __fastcall SpawnItemHook(MapVariant *thisptr, void *unused, uint32_t tagIndex, int a3, int placementIndex,
		RealVector3D *position, RealVector3D *forward, RealVector3D *up,
		int scenarioPlacementIndex, int objectType, uint8_t *placementProps, uint16_t placementFlags)
	{
		const auto c_map_variant_spawn_object = (uint32_t(__thiscall *)(MapVariant *thisptr, uint32_t tagIndex, int a3, int placementIndex,
			RealVector3D *position, RealVector3D *forward, RealVector3D *up,
			int scenarioPlacementIndex, int objectType, uint8_t *placementProps, uint16_t placementFlags))(0x00582110);

		CreateOrGetBudgetForItem(thisptr, tagIndex);

		auto objectIndex = c_map_variant_spawn_object(thisptr, tagIndex, a3, placementIndex,
			position, forward, up, scenarioPlacementIndex, objectType, placementProps, placementFlags);

		for (auto &cb : s_ItemSpawnedCallbacks)
			cb(objectIndex);

		Web::Ui::WebForge::OnItemSpawned(objectIndex);

		return objectIndex;
	}

	bool Forge_SpawnItemCheckHook(uint32_t tagIndex, RealVector3D *position, uint32_t unitObjectIndex)
	{
		const auto sub_715E90 = (char(__cdecl *)(int a1, float boundingRadius, int a3, int a4,
			int a5, const RealVector3D *a6, int a7, int a8, RealVector3D *newPosition, char a10))(0x715E90);

		auto object = Blam::Tags::TagInstance(tagIndex).GetDefinition<Blam::Tags::Objects::Object>();
		if (!object)
			return false;

		auto boundingRadius = object->BoundingRadius;
		if (object->Model.TagIndex != -1)
		{
			auto hlmtDef = Blam::Tags::TagInstance(object->Model.TagIndex).GetDefinition<Blam::Tags::Models::Model>();
			if (hlmtDef && hlmtDef->ModelObjectData.Count)
			{
				const auto &modelobjectData = hlmtDef->ModelObjectData.Elements[0];
				if (modelobjectData.Radius > boundingRadius)
					boundingRadius = modelobjectData.Radius;
			}
		}

		if (boundingRadius < 0.000099999997f)
			boundingRadius = 1;

		if (boundingRadius < 2)
			sub_715E90(boundingRadius, boundingRadius, 1, 4, unitObjectIndex, position, 0, -1, position, 0);

		return CreateOrGetBudgetForItem(GetMapVariant(), tagIndex) != -1;
	}

	void UpdateLightHook(uint32_t lightDatumIndex, int a2, float intensity, int a4)
	{
		struct LightDatum : public Blam::DatumBase
		{
			__int16 Flags;
			int TagIndex;
			int field_8;
			int field_C;
			int UnknownTicks;
			BYTE field_14[48];
			int ObjectIndex;
			uint32_t field_48;
			__int16 field_4C;
			__int16 ColorPermutation;
			int field_50;
			BYTE field_54[39];
			char field_7B;
			RealVector3D Center;
			BYTE field_88[28];
			RealVector3D field_A4;
			BYTE field_B0[28];
			RealColorRGB Color;
			int field_D8;
			int field_DC;
			int field_E0;
		};

		static_assert(sizeof(LightDatum) == 0xE4, "invalid light datum size");

		const auto sub_A66900 = (void(__thiscall *)(int thisptr, RealVector3D center, RealColorRGB color, float arg_18, float range,
			RealVector3D a6, float spread, float arg_30, float spreadb))(0xA66900);
		const auto sub_A667F0 = (void(*)(uint32_t lightDatumIndex, int a2, float colorIntensity, int a4))(0xA667F0);

		auto& lightArray = ElDorito::GetMainTls(0x498)[0].Read<Blam::DataArray<LightDatum>>();

		auto light = lightArray.Get(lightDatumIndex);
		if (!light)
			return;

		auto lightDef = Blam::Tags::TagInstance(light->TagIndex).GetDefinition<uint8_t>();
		auto lightFlags = *(uint32_t*)lightDef;

		auto parentObject = Blam::Objects::Get(light->ObjectIndex);
		if (lightFlags & (1 << 31) && parentObject)
		{
			auto props = parentObject->GetMultiplayerProperties();
			auto lightProperties = reinterpret_cast<const Forge::ForgeLightProperties*>((uint8_t*)props + 0x14);

			auto lightDefPtr = Pointer(lightDef);

			auto v2 = 0.0f;
			auto lightType = lightDefPtr(0x4).Read<uint16_t>();
			if (lightType == 0) // point light
				v2 = 1.0f;
			else // spot light
				v2 = lightDefPtr(0x78).Read<float>();

			auto v23 = lightDefPtr(0x74).Read<float>();
			auto v22 = lightDefPtr(0x14).Read<float>();

			intensity = ((lightProperties->Intensity / 255.0f)) * 250.0f * intensity;
			RealColorRGB color(
				intensity * (lightProperties->ColorR / 255.0f),
				intensity * (lightProperties->ColorG / 255.0f),
				intensity * (lightProperties->ColorB / 255.0f)
			);

			sub_A66900(a4, light->Center, color, 6.0f, lightProperties->Range, light->field_A4, v2, v23, v22);

			return;
		}

		sub_A667F0(lightDatumIndex, a2, intensity, a4);
	}

	struct ScreenEffectDatum : Blam::DatumBase
	{
		uint8_t Unknown02;
		uint8_t Unknown03;
		uint32_t TagIndex;
		float SecondsAlive;
		RealVector3D Position;
		uint32_t ObjectIndex;
		uint32_t Unknown1C;
		uint32_t Unknown20;
		uint32_t Unknown24;
		uint32_t Unknown28;
		RealVector3D Unknown2C;
		uint32_t Unknown38;
	};

	struct ScreenEffectData
	{
		float LightIntensity;
		float PrimaryHue;
		float SecondaryHue;
		float Saturation;
		float Desaturation;
		float GammaIncrease;
		float GammaDecrease;
		float ShadowBrightness;
		RealColorRGB ColorFilter;
		RealColorRGB ColorFloor;
		float Tracing;
		float Turbulance;
	};

	void FillScreenEffectRenderData(Blam::Tags::Camera::AreaScreenEffect::ScreenEffect& screenEffectDef, float t, ScreenEffectData *data)
	{
		auto v29 = data->LightIntensity;
		if (data->LightIntensity <= (t * screenEffectDef.LightIntensity))
			v29 = t * screenEffectDef.LightIntensity;
		data->LightIntensity = v29;
		auto v30 = data->PrimaryHue;
		if (v30 <= (screenEffectDef.PrimaryHue * t))
			v30 = screenEffectDef.PrimaryHue * t;
		data->PrimaryHue = v30;
		auto v31 = data->SecondaryHue;
		if (v31 <= screenEffectDef.SecondaryHue * t)
			v31 = screenEffectDef.SecondaryHue * t;
		data->SecondaryHue = v31;
		auto v32 = data->Saturation;
		if (v32 <= (screenEffectDef.Saturation * t))
			v32 = screenEffectDef.Saturation * t;
		data->Saturation = v32;
		auto v33 = data->Desaturation;
		if (v33 <= (screenEffectDef.Desaturation * t))
			v33 = screenEffectDef.Desaturation * t;
		data->Desaturation = v33;
		auto v34 = data->GammaIncrease;
		if (v34 <= (screenEffectDef.GammaIncrease * t))
			v34 = screenEffectDef.GammaIncrease * t;
		data->GammaIncrease = v34;
		auto v35 = data->GammaDecrease;
		if (v35 <= (screenEffectDef.GammaDecrease * t))
			v35 = screenEffectDef.GammaDecrease * t;
		data->GammaDecrease = v35;
		auto v36 = data->ShadowBrightness;
		if (v36 <= (screenEffectDef.ShadowBrightness * t))
			v36 = screenEffectDef.ShadowBrightness * t;
		auto v37 = data->ColorFilter.Red;
		data->ShadowBrightness = v36;
		auto v38 = 1.0f - t;
		auto v39 = (screenEffectDef.ColorFilter.Red * t) + (1.0f - t);
		if (v37 <= v39)
			v39 = v37;
		auto v40 = data->ColorFilter.Green;
		data->ColorFilter.Red = v39;
		auto v41 = (screenEffectDef.ColorFilter.Green * t) + v38;
		if (v40 <= v41)
			v41 = v40;
		auto v42 = data->ColorFilter.Blue;
		data->ColorFilter.Green = v41;
		auto v43 = (screenEffectDef.ColorFilter.Blue * t) + v38;
		if (v42 <= v43)
			v43 = v42;
		auto v44 = data->ColorFloor.Red;
		data->ColorFilter.Blue = v43;
		if (v44 <= (screenEffectDef.ColorFloor.Red * t))
			v44 = screenEffectDef.ColorFloor.Red * t;
		data->ColorFloor.Red = v44;
		auto v45 = data->ColorFloor.Green;
		if (v45 <= (screenEffectDef.ColorFloor.Green * t))
			v45 = screenEffectDef.ColorFloor.Green * t;
		data->ColorFloor.Green = v45;
		auto v46 = data->ColorFloor.Blue;
		if (v46 <= (screenEffectDef.ColorFloor.Blue * t))
			v46 = screenEffectDef.ColorFloor.Blue * t;
		data->ColorFloor.Blue = v46;
		auto v47 = data->Tracing;
		if (v47 <= (screenEffectDef.Tracing * t))
			v47 = screenEffectDef.Tracing * t;
		data->Tracing = v47;
		auto v48 = data->Turbulance;
		if (v48 <= (screenEffectDef.Turbulance * t))
			v48 = screenEffectDef.Turbulance * t;
		data->Turbulance = v48;
	}

	void ScreenEffectsHook(RealVector3D *a1, RealVector3D *a2, ScreenEffectData *renderData, void *a4, int localPlayerIndex)
	{
		const auto sub_683190 = (void(*)(RealVector3D *a1, RealVector3D *a2, ScreenEffectData *data, int a4, int a5))(0x683190);
		const auto sub_682C10 = (float(__thiscall *)(Blam::Tags::Camera::AreaScreenEffect::ScreenEffect *screenEffect, float distance, float a3, float secondsAlive, float *a5))(0x682C10);
		const auto sub_682A90 = (unsigned __int32(__thiscall *)(void *thisptr, void *a2, int a3, float a4, int a5, void * a6))(0x682A90);
		const auto sub_A44FD0 = (bool(*)(int localPlayerIndex, uint32_t objectIndex))(0xA44FD0);
		const auto sub_4EEC40 = (float(*)(RealVector3D *position, RealVector3D *forward))(0x4EEC40);
		const auto sub_5F0C20 = (void(__thiscall *)(void *thisptr))(0x005F0C20);
		const auto sub_6D67E0 = (char(*)(unsigned int flags, int a2, char a3, RealVector3D *a4, RealVector3D *a5, int a6, int a7, int a8, void *a9))(0x6D67E0);
		const auto GetLocalPlayerUnitObjectIndex = (uint32_t(*)(int localPlayerIndex))(0x00589CC0);

		renderData->LightIntensity = 0;
		renderData->PrimaryHue = 0;
		renderData->SecondaryHue = 0;
		renderData->Saturation = 0;
		renderData->Desaturation = 0;
		renderData->GammaIncrease = 0;
		renderData->GammaDecrease = 0;
		renderData->ShadowBrightness = 0;
		renderData->ColorFilter = { 1.0, 1.0, 1.0 };
		renderData->ColorFloor = { 0.0, 0.0, 0.0 };
		renderData->Tracing = 0;
		renderData->Turbulance = 0;

		if (!a4)
			return;

		*(DWORD *)((uint8_t*)a4 + 0x120) = 0;

		auto &screenEffectDatumArray = ElDorito::GetMainTls(0x338)[0].Read<Blam::DataArray<ScreenEffectDatum>>();
		for (auto &screenEffectDatum : screenEffectDatumArray)
		{
			auto sefc = Blam::Tags::TagInstance(screenEffectDatum.TagIndex).GetDefinition<Blam::Tags::Camera::AreaScreenEffect>();
			if (!sefc || sefc->ScreenEffects.Count <= 0)
				continue;
			auto screenEffectDef = sefc->ScreenEffects.Elements[0];
			if (screenEffectDef.Duration > 0 && screenEffectDatum.SecondsAlive > screenEffectDef.Duration)
				continue;

			if (screenEffectDef.Flags & 0xC && screenEffectDatum.Unknown02 == 1)
			{
				if (!(screenEffectDef.Flags & 0x20) || screenEffectDatum.ObjectIndex == GetLocalPlayerUnitObjectIndex(localPlayerIndex))
				{
					auto v25 = sub_A44FD0(localPlayerIndex, screenEffectDatum.ObjectIndex);
					if ((!(screenEffectDef.Flags & 4) || v25) && (!(screenEffectDef.Flags & 8) || !v25))
						continue;
				}
			}

			uint8_t a9[0x70];

			if (!(screenEffectDef.Flags & 0x10) || (sub_5F0C20(&a9[24]), *(WORD *)&a9[32] = -1,
				!sub_6D67E0(*(uint32_t*)0x0471A8D0, *(uint32_t*)0x471A8D4, 0, a1, &screenEffectDatum.Position, -1, -1, -1, a9)))
			{
				auto object = Blam::Objects::Get(screenEffectDatum.ObjectIndex);
				if (object && object->PlacementIndex != -1)
				{
					auto props = object->GetMultiplayerProperties();
					auto properties = (ForgeScreenFxProperties*)((uint8_t*)props + 0x14);
					if (properties)
					{
						auto playerIndex = Blam::Players::GetLocalPlayer(localPlayerIndex);
						Blam::Players::PlayerDatum *player;
						if ((playerIndex != DatumIndex::Null && (player = Blam::Players::GetPlayers().Get(playerIndex))) &&
							player->Properties.TeamIndex != props->TeamIndex && props->TeamIndex != 8)
						{
							screenEffectDef.MaximumDistance = 0;
						}
						else
						{
							screenEffectDef.MaximumDistance = properties->MaximumDistance / 255.0f * 255.0f;
							screenEffectDef.LightIntensity = properties->LightIntensity / 255.0f * 5.0f;
							screenEffectDef.Saturation = properties->Saturation / 255.0f;
							screenEffectDef.Desaturation = properties->Desaturation / 255.0f;
							screenEffectDef.PrimaryHue = properties->Hue / 255.0f * 360;
							screenEffectDef.GammaIncrease = properties->GammaIncrease / 255.0f;
							screenEffectDef.GammaDecrease = properties->GammaDecrease / 255.0f;
							screenEffectDef.ColorFilter.Red = properties->ColorFilterR / 255.0f;
							screenEffectDef.ColorFilter.Green = properties->ColorFilterG / 255.0f;
							screenEffectDef.ColorFilter.Blue = properties->ColorFilterB / 255.0f;
							screenEffectDef.ColorFloor.Red = properties->ColorFloorR / 255.0f;
							screenEffectDef.ColorFloor.Green = properties->ColorFloorG / 255.0f;
							screenEffectDef.ColorFloor.Blue = properties->ColorFloorB / 255.0f;
							screenEffectDef.Tracing = properties->Tracing / 100.0f;
						}
					}
				}

				auto d = screenEffectDatum.Position - *a1;
				auto v60 = sub_4EEC40(&d, a2);

				auto distance = d.Length();

				float f = 0;
				auto t = sub_682C10(&screenEffectDef, distance, v60, screenEffectDatum.SecondsAlive, &f);
				if (t != 0.0f)
				{
					if (t > 1.0f)
						t = 1.0f;

					FillScreenEffectRenderData(screenEffectDef, t, renderData);
					sub_682A90(a4, &screenEffectDef.ScreenShader, t, v60, -1, &screenEffectDatum.Unknown2C);
				}
			}
		}
	}

	void __cdecl MapVariant_SyncVariantPropertiesHook(Blam::MapVariant::VariantPlacement *placement)
	{
		const auto MapVariant_UpdateObjectVariantProperties = (void(*)(Blam::MapVariant::VariantPlacement *a1))(0x00586680);
		MapVariant_UpdateObjectVariantProperties(placement);

		auto object = Blam::Objects::Get(placement->ObjectIndex);
		if (!object)
			return;

		auto mpProperties = object->GetMultiplayerProperties();
		if (mpProperties)
		{
			placement->Properties.ZoneShape = mpProperties->Shape;
			placement->Properties.ZoneTop = mpProperties->Top;
			placement->Properties.ZoneBottom = mpProperties->Bottom;
			placement->Properties.ZoneDepth = mpProperties->Length;
			placement->Properties.ZoneRadiusWidth = mpProperties->RadiusWidth;
		}
	}

	void __fastcall sub_584CF0_hook(MapVariant *thisptr, void *unused)
	{
		// do nothing
	}

	void GameEngineTickHook()
	{
		const auto is_client = (bool(*)())(0x00531D70);

		if (!is_client())
			::Forge::Volumes::Update();
	}

	bool ShouldOverrideColorPermutation(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;

		auto mpProperties = object->GetMultiplayerProperties();
		if (mpProperties)
		{
			if (CanThemeObject(objectIndex))
				return true;

			if (mpProperties->ObjectType == 0xC)
				return true;
		}

		return false;
	}

	void UpdateObjectCachedColorPermutationRenderStateHook(uint32_t objectIndex, bool invalidated)
	{
		using ObjectDefinition = Blam::Tags::Objects::Object;

		const auto sub_5A03E0 = (bool(*)(int a1))(0x5A03E0);
		const auto sub_B31B80 = (void(*)(uint32_t objectIndex, int a2, bool invalidated))(0xB31B80);
		const auto object_get_render_data = (void*(*)(uint32_t objectIndex))(0x00B2E800);
		const auto object_get_color_permutation = (bool(*)(uint32_t objectIndex, int index, Blam::Math::RealColorRGB *a3))(0x00B2DE50);

		if (sub_5A03E0(2))
		{
			auto object = Blam::Objects::Get(objectIndex);
			if (!object)
				return;

			auto objectDef = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<ObjectDefinition>();
			auto cachedRenderStateIndex = *(uint32_t*)object_get_render_data(objectIndex);

			
			bool shouldOverride = false;

			if (cachedRenderStateIndex != -1 
				&& (invalidated || (*((uint8_t*)objectDef + 0x1C) & 1) || (shouldOverride = ShouldOverrideColorPermutation(objectIndex)))) // player respawn location
			{
				auto object = Blam::Objects::Get(objectIndex);
				auto mpProperties = object->GetMultiplayerProperties();

				uint8_t *cachedRenderStates = nullptr;
				__asm
				{
					mov eax, dword ptr fs : [0x02C]
					mov eax, dword ptr ds : [eax]
					mov eax, [eax + 0x3E0]
					mov cachedRenderStates, eax
				}
				auto cachedRenderState = *(uint8_t**)(cachedRenderStates + 0x44) + 0x4D8 * (cachedRenderStateIndex & 0xffff);

				*(int8_t *)(cachedRenderState + 0x474) = 0;

				auto index = 0;
				auto pColor = (DWORD *)(cachedRenderState + 0x460);
				do
				{
					Blam::Math::RealColorRGB color;
					if (object_get_color_permutation(objectIndex, ++index, &color))
					{
						if (shouldOverride && mpProperties)
						{
							auto isReforgeObject = CanThemeObject(objectIndex);
							if (isReforgeObject)
							{
								const auto ReforgeProperties = (Forge::ReforgeObjectProperties*)&mpProperties->RadiusWidth;
								color.Red = ReforgeProperties->ColorR / 255.0f;
								color.Green = ReforgeProperties->ColorG / 255.0f;
								color.Blue = ReforgeProperties->ColorB / 255.0f;
							}
							else if (mpProperties->ObjectType == 0xC) // player respawn location
							{
								switch (mpProperties->TeamIndex)
								{
								case 0: // red 
									color = { 0.38f, 0.04f, 0.04f };
									break;
								case 1: // blue
									color = { 0.04f, 0.14f, 0.38f };
									break;
								case 2: // green
									color = { 0.12f, 0.21f, 0.01f };
									break;
								case 3: // orange
									color = { 0.73f, 0.30f, 0.00f };
									break;
								case 4: // purple
									color = { 0.11f, 0.06f, 0.32f };
									break;
								case 5: // gold
									color = { 0.65f, 0.47f, 0.03f };
									break;
								case 6: // brown
									color = { 0.10f, 0.05f, 0.00f };
									break;
								case 7: // pink
									color = { 1.00f, 0.30f, 0.54f };
									break;
								}
								
							}
						}

						*pColor = (((signed int)(color.Red * 255.0) & 0xFF) << 16)
							| (((signed int)(color.Green * 255.0) & 0xFF) << 8)
							| (signed int)(color.Blue * 255.0) & 0xFF;


						++*(int8_t *)(cachedRenderState + 0x474);
					}
					++pColor;
				} while (index < 5);
			}
		}
		else
		{
			sub_B31B80(objectIndex, 2, invalidated);
		}
	}

	bool __cdecl sub_980E40_hook(int a1, uint32_t objectIndex)
	{
		const auto sub_980E40 = (bool(*)(int a1, uint32_t objectIndex))(0x980E40);
		if (!sub_980E40(a1, objectIndex))
			return false;

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);

		switch (object->TagIndex)
		{
		case Forge::PrematchCamera::CAMERA_OBJECT_TAG_INDEX:
		case Forge::Volumes::KILL_VOLUME_TAG_INDEX:
		case Forge::Volumes::GARBAGE_VOLUME_TAG_INDEX:
			if (!Forge::GetEditorModeState(playerIndex, nullptr, nullptr))
				return false;
		}

		auto mpProperties = object->GetMultiplayerProperties();
		if (mpProperties && CanThemeObject(objectIndex))
		{
			if (mpProperties->TeleporterChannel == int8_t(INVISIBLE_MATERIAL_INDEX)
				&& !Forge::GetEditorModeState(Blam::Players::GetLocalPlayer(0), nullptr, nullptr)) // not in monitor mode or you can't grab it
			{
				return false;
			}
		}

		return true;
	}

	void sandbox_zone_shape_render_hook(ZoneShape *zone, float *color, uint32_t objectIndex)
	{
		const auto sandbox_zone_shape_render_hook = (void(*)(ZoneShape *zone, float *color, uint32_t objectIndex))(0x00BA0FC0);
		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return;

		// ignore forge volumes
		switch (object->TagIndex)
		{
		case Forge::Volumes::GARBAGE_VOLUME_TAG_INDEX:
		case Forge::Volumes::KILL_VOLUME_TAG_INDEX:
			return;
		}

		sandbox_zone_shape_render_hook(zone, color, objectIndex);

	}

	bool __fastcall sub_724890_hook(void *thisptr, void *unused, int a2, int a3, float *matrix)
	{
		const auto sub_724890 = (bool(__thiscall*)(void *thisptr, int a2, int a3, float *matrix))(0x724890);
		auto ret = sub_724890(thisptr, a2, a3, matrix);

		auto havokComponents = *(Blam::DataArray<Blam::DatumBase>**)0x02446080;
		auto &groundHavokComponentIndex = *(uint32_t*)((uint8_t*)thisptr + 0x1c);
		uint8_t *groundHavokComponent;
		if (groundHavokComponentIndex != -1 && (groundHavokComponent = (uint8_t*)havokComponents->Get(groundHavokComponentIndex)))
		{
			auto groundObjectIndex = *(uint32_t*)(groundHavokComponent + 0x8);
			if (groundObjectIndex != -1)
			{
				if (ObjectIsPhased(groundObjectIndex))
					groundHavokComponentIndex = -1;
			}
		}

		return ret;
	}

	void __fastcall c_game_engine_object_runtime_manager__on_object_spawned_hook(void *thisptr, void *unused, int16_t placementIndex, uint32_t objectIndex)
	{
		const auto c_game_engine_object_runtime_manager__on_object_spawned = (void(__thiscall*)(void *thisptr, int16_t placementIndex, uint32_t objectIndex))(0x00590600);
		if (!CanThemeObject(objectIndex)) // ignore reforge
			c_game_engine_object_runtime_manager__on_object_spawned(thisptr, placementIndex, objectIndex);
	}

	void __fastcall CameraFxHook(void *thisptr, void *unused, void *a2)
	{
		const auto sub_A3B990 = (void(__thiscall*)(void *thisptr, void *a2))(0xA3B990);
		sub_A3B990(thisptr, a2);

		if (std::abs(s_CameraFxSettings.Exposure) > 0.0001f)
			*(float*)((uint8_t*)thisptr + 0x29C) = s_CameraFxSettings.Exposure;
		if (std::abs(s_CameraFxSettings.LightIntensity) > 0.0001f)
			*(float*)((uint8_t*)thisptr + 0x1E48) = s_CameraFxSettings.LightIntensity;
		if (std::abs(s_CameraFxSettings.BloomIntensity) > 0.0001f)
			*(float*)((uint8_t*)thisptr + 0x2A0) = s_CameraFxSettings.BloomIntensity;
	}

	void __cdecl ShieldImpactBloomHook(int id, int count, float *data)
	{
		const auto rasterizer_set_pixel_shader_constant = (void(*)(int id, int count, float *data))(0x00A66270);
		if (std::abs(s_CameraFxSettings.BloomIntensity) > 0.0001f)
		{
			data[12] *= s_CameraFxSettings.BloomIntensity;
			data[13] *= s_CameraFxSettings.BloomIntensity;
			data[14] *= s_CameraFxSettings.BloomIntensity;
			data[15] *= s_CameraFxSettings.BloomIntensity;
		}
		rasterizer_set_pixel_shader_constant(id, count, data);
	}
}
