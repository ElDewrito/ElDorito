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
#include "../Modules/ModulePlayer.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Web/Ui/WebForge.hpp"
#include "../Web/Ui/WebConsole.hpp"
#include <cassert>
#include <queue>
#include <stack>
#include "../CommandMap.hpp"
#include "../Blam/Tags/Globals/ForgeGlobalsDefinition.hpp"

using namespace Forge;
using namespace Blam;
using namespace Blam::Objects;
using namespace Blam::Math;

namespace
{
	using Blam::Tags::Globals::ForgeGlobalsDefinition;

	const auto HELDOBJECT_DISTANCE_MIN = 0.1f;
	const auto HELDOBJECT_DISTANCE_CHANGE_MULTIPLIER = 0.1f;
	const auto HELDOBJECT_ROTATION_SENSITIVTY_BASE = 0.5f;
	const auto REFORGE_DEFAULT_SHADER = 0x3ab0;
	const auto INVISIBLE_MATERIAL_INDEX = 121;
	const auto MAP_MODIFIER_TAG_INDEX = 0x5728;

	const auto UI_PlaySound = (void(*)(int index, uint32_t uiSoundTagIndex))(0x00AA5CD0);


	void UpdateMapModifier();
	bool CheckKillTriggersHook(int a0, void *a1);
	bool ObjectSafeZoneHook(void *a0);
	void* PushBarriersGetStructureDesignHook(int index);
	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot);
	void SpecialWeaponHUDHook(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objectsInCluster, int16_t objectcount, BYTE* activeSpecialChudTypes);
	void ObjectGrabbedHook(uint32_t playerIndex, uint16_t placementIndex);
	void ObjectGrabbedHook2(uint32_t playerIndex, uint32_t objectIndex);
	void ObjectDroppedHook(uint16_t placementIndex, float throwForce, int a3);
	void ObjectDeleteHook(uint16_t placementIndex, uint32_t playerIndex);
	void ObjectSpawnedHook(uint32_t tagIndex, uint32_t playerIndex, const RealVector3D* position);
	void ObjectPropertiesChangeHook(uint32_t playerIndex, uint16_t placementIndex, MapVariant::VariantProperties* properties);
	void UnitFlyingHook(uint32_t unitObjectIndex, int a2, int a3, int a4, int a5, int a6, int a7);
	void UpdateHeldObjectTransformHook(int a1, uint32_t objectIndex, RealVector3D* position, RealVector3D* forwardVec, RealVector3D* upVec);
	bool HeldObjectHitTestHook(int a1, int a2, RealVector3D *start, RealVector3D *end, int objectIndexA, int objectIndexB, void *results);
	bool Forge_UpdatePlayerEditorGlobalsHook(int16_t playerIndex, uint32_t* pObjectIndex);
	bool Forge_SpawnItemCheckHook(uint32_t tagIndex, RealVector3D *position, uint32_t unitObjectIndex);
	void SandboxEngineInitHook();
	void SandboxEngineShutdownHook();
	void SandboxEngineTickHook();
	void __fastcall SandboxEngineObjectDisposeHook(void* thisptr, void* unused, uint32_t objectIndex);
	void* ObjectPropertiesUIAllocateHook(int size);
	void* ObjectCreationUIAllocateHook(int size);
	void RenderMeshPartHook(void* data, int a2);
	int ObjectLightingQualityOverrideHook();
	void RenderObjectTransparentHook();
	void RenderObjectCompressionInfoHook(uint8_t *compressionInfo);
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
	void MapVariant_BuildVariantPropertiesFromObjectHook(Blam::MapVariant::VariantPlacement *placement);

	void __fastcall c_map_variant_initialize_from_scenario_hook(Blam::MapVariant *thisptr, void* unused);
	void __fastcall c_map_variant_update_item_budget_hook(Blam::MapVariant *thisptr, void* unused, int budgetIndex, char arg_4);
	void __fastcall c_map_variant_copy_budget_hook(Blam::MapVariant *thisptr, void *unused, Blam::MapVariant *other);

	void MapVariant_SpawnObjectHook();

	void __fastcall c_game_engine_object_runtime_manager__on_object_spawned_hook(void *thisptr, void *unused, int16_t placementIndex, uint32_t objectIndex);

	void UpdateLightHook(uint32_t lightDatumIndex, int a2, float intensity, int a4);
	uint32_t __fastcall SpawnItemHook(MapVariant *thisptr, void *unused, uint32_t tagIndex, int a3, int placementIndex,
		RealVector3D *position, RealVector3D *forward, RealVector3D *up,
		int scenarioPlacementIndex, int objectType, uint8_t *placementProps, uint16_t placementFlags);
	void LightIlluminationFunctionHook();
	void LightColorFunctionHook();

	void sandbox_zone_shape_render_hook(ZoneShape *zone, float *color, uint32_t objectIndex);

	struct ScreenEffectData;
	void ScreenEffectsHook(RealVector3D *a1, RealVector3D *a2, ScreenEffectData *renderData, void *a4, int localPlayerIndex);
	void GameEngineTickHook();
	bool GameObjectResetHook(uint32_t objectIndex);

	bool __fastcall sub_724890_hook(void *thisptr, void *unused, int a2, int a3, float *matrix);

	void __fastcall CameraFxHook(void *thisptr, void *unused, void *a2);
	void __cdecl ShieldImpactBloomHook(int id, int count, float *data);
	void RenderAtmosphereHook(short bspIndex, void *state);
	void BspWeatherHook();
	void BackgroundSoundEnvironmentHook(float a1);
	void scenario_place_sky_scenery_hook(uint32_t scenarioTagIndex, uint32_t activeBsp);

	void EvaluateRigidBodyNodesHook();
	uint32_t __cdecl HavokCleanupHook(signed int a1, int islandEntityIndex, char a3, char a4, char a5, int a6);
	void __fastcall c_havok_component__activate_hook(uint8_t *thisptr, void *unused);

	void GrabSelection(uint32_t playerIndex);
	void DoClone(uint32_t playerIndex, uint32_t objectIndexUnderCrosshair);
	void HandleMovementSpeed();
	void HandleMonitorNoclip();
	void HandleCommands();
	bool UpdateWeatherEffects();
	void OnMonitorModeChange(bool isMonitor);
	bool CanThemeObject(uint32_t objectIndex);

	std::vector<Patches::Forge::ItemSpawnedCallback> s_ItemSpawnedCallbacks;

	const float MONITOR_MOVEMENT_SPEEDS[] = { 0.001f, 0.05f, 0.25f, 1.0f, 2.0f, 4.0f };

	struct SandboxPaletteItem
	{
		Blam::Tags::TagReference Object;
		uint32_t Name;
		int32_t MaxAllowed;
		float Cost;
		uint8_t Unknown1C[0x14];
	};
	static_assert(sizeof(SandboxPaletteItem) == 0x30, "Invalid SandboxPaletteItem size");

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

	struct s_object_looping_sound : Blam::DatumBase
	{
		uint16_t field_2;
		uint16_t Flags;
		uint16_t AttachmentIndex;
		float Scale;
		uint32_t LoopingSound;
		uint32_t field_10;
		uint32_t field_14;
		uint32_t field_18;
		uint16_t PrimaryScale;
		uint8_t field_1E;
		uint8_t field_1F;
	};
	static_assert(sizeof(s_object_looping_sound) == 0x20, "invalid");


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


	enum ForgeSkyFlags
	{
		eForgeSkyFlags_UseMapModifierTransform = (1 << 0)
	};

	struct {
		bool IsValid = false;
		bool IsActive = false;
		bool WaitingForNewRound;
		uint32_t ObjectIndex;
		struct {
			bool KillBarriersEnabled;
			bool PushBarriersEnabled;
		} MapBarriers;
		struct {
			bool Enabled;
			float Exposure;
			float LightIntensity;
			float BloomIntensity;
			float LightBloomIntensity;
		} CameraFx;
		struct {
			bool Enabled;
			uint8_t WeatherIndex = 0xff;
			Blam::Math::RealColorRGB FogColor;
			float FogDensity;
			float FogVisibility;
			float Brightness;	
		} Atmosphere;

		struct {
			uint32_t Index;
			uint32_t Flags;
			uint32_t SceneryObjectIndex;
			uint32_t BackgroundSoundDatumIndex;
			uint32_t ScreenEffectDatumIndex;

		} ForgeSky;
	} 
	mapModifierState;

	ForgeGlobalsDefinition::Sky mapDefaultSky;

	struct {
		bool IsValid = false;
		bool MonitorLastTick = false;
		bool NoclipLastTick = false;
		uint32_t HeldObjectIndex = -1;
		uint32_t HeldObjectComplexRootRigidBodyNodeIndex = -1;
		RealVector3D GrabOffset;
	} monitorState;

	struct {
		bool IsValid;
		float Scale;
		float OffsetX;
		float OffsetY;
	} customTextureModeState;

	const auto SpawnWeatherEffect = (unsigned int(*)(int effectTagIndex))(0x005B8BD0);
	const auto FreeWeatherEffect = (void(*)(signed int effectDatumIndex))(0x005B6FC0);

	ForgeGlobalsDefinition *forgeGlobals_;

	std::bitset<0xffff> cachedReforgeTagIndexSet;

	bool objectLightingQualityOverride = false;
	bool inSandboxEngine = false;
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
		Hook(0x19B14D, ObjectGrabbedHook2, HookFlags::IsCall).Apply();
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
		// function uses XMM registers to copy the zone properties which can destroy the data stored there, reimplement it
		Hook(0x186680, MapVariant_BuildVariantPropertiesFromObjectHook).Apply();

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
		// hack to fix lighting on scaled bipeds
		Hook(0x647695, ObjectLightingQualityOverrideHook, HookFlags::IsCall).Apply();

		// prevent the object from lurching forward when being rotated
		Pointer(0x0059FA95 + 4).Write((float*)&HELDOBJECT_DISTANCE_MIN);
		// slow down in/out movement
		Pointer(0x0059FA7A + 4).Write((float*)&HELDOBJECT_DISTANCE_CHANGE_MULTIPLIER);
		Patch(0x19FACC, { 0x90,0x90 }).Apply(); // allow object to be extended fully
		// Prevent held object from clipping with bsp and other objects when no clipping
		Hook(0x19E8AD, HeldObjectHitTestHook, HookFlags::IsCall).Apply();

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
		Hook(0x79E1A9, LightIlluminationFunctionHook, HookFlags::IsCall).Apply();
		Hook(0x79E186, LightColorFunctionHook, HookFlags::IsCall).Apply();
		
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

		// prevent objectives getting reset when below the scenario game object reset point when map barriers are disabled
		Hook(0x58028E, GameObjectResetHook, HookFlags::IsCall).Apply();
		Hook(0x5D4CD5, GameObjectResetHook, HookFlags::IsCall).Apply();

		// disble projectile collisions on invisible material
		Hook(0x2D8F82, sub_980E40_hook, HookFlags::IsCall).Apply();

		Hook(0x19F0B8, sandbox_zone_shape_render_hook, HookFlags::IsCall).Apply();

		// fix jump canceling on forged objects
		Hook(0x3245FD, sub_724890_hook, HookFlags::IsCall).Apply();

		Hook(0x19004E, c_game_engine_object_runtime_manager__on_object_spawned_hook, HookFlags::IsCall).Apply();

		Hook(0x00639A68, CameraFxHook, HookFlags::IsCall).Apply();
		Hook(0x00639AC1, RenderAtmosphereHook, HookFlags::IsCall).Apply();
		// prevent weather from being reset
		Hook(0x27223E, BspWeatherHook).Apply();
		Hook(0x00653D77, ShieldImpactBloomHook, HookFlags::IsCall).Apply();

		// fix ragdolls falling through objects
		Patch(0x317265, { 0xBA,0x07,0x00,0x00,0x00 }).Apply();
		Patch(0x7D8A59, { 0xEB }).Apply();

		// reforge materials
		Hook(0x679F7F, RenderObjectTransparentHook).Apply();
		Hook(0x679285, RenderObjectCompressionInfoHook, HookFlags::IsCall).Apply();

		Hook(0x1D9BBE, BackgroundSoundEnvironmentHook, HookFlags::IsCall).Apply();
		Hook(0x7591E4, scenario_place_sky_scenery_hook, HookFlags::IsCall).Apply();
		// horrible hack to stop phased objects that have multiple, conflicting rigid bodies from shaking
		Hook(0x31293C, EvaluateRigidBodyNodesHook).Apply();
		// prevent havok from deleting reforge objects
		Hook(0x1C469A, HavokCleanupHook, HookFlags::IsCall).Apply();
		// force reforge rigid bodies into to a single fixed simulation island
		Hook(0x731E5F, c_havok_component__activate_hook, HookFlags::IsCall).Apply();
	}

	void Tick()
	{
		const auto game_options_is_valid = (bool(*)())(0x00532650);
		const auto game_is_sandbox_engine = (bool(*)())(0x0059A780);

		if (!game_options_is_valid())
		{
			mapModifierState.IsValid = false;
			monitorState.IsValid = false;
			mapModifierState.ObjectIndex = -1;
			monitorState.HeldObjectIndex = -1;
			forgeGlobals_ = nullptr;
			inSandboxEngine = false;
			customTextureModeState.IsValid = false;
		}
		else
		{
			if (!mapModifierState.IsValid)
			{
				inSandboxEngine = game_is_sandbox_engine();
				memset(&mapModifierState, 0, sizeof(mapModifierState));
				mapModifierState.ObjectIndex = -1;
				mapModifierState.Atmosphere.WeatherIndex = 0xff;
				mapModifierState.ForgeSky.SceneryObjectIndex = -1;
				mapModifierState.ForgeSky.BackgroundSoundDatumIndex = -1;
				mapModifierState.ForgeSky.ScreenEffectDatumIndex = -1;
				mapModifierState.ForgeSky.Index = -1;
				mapModifierState.MapBarriers.KillBarriersEnabled = true;
				mapModifierState.MapBarriers.PushBarriersEnabled = true;
				mapModifierState.IsValid = true;
				mapModifierState.IsActive = false;

				auto scenario = Blam::Tags::Scenario::GetCurrentScenario();
				if (scenario->SkyReferences.Count)
					mapDefaultSky.Object = scenario->SkyReferences[0].SkyObject;
				else
					mapDefaultSky.Object = Blam::Tags::TagReference(0, -1);
				mapDefaultSky.GlobalLighting = scenario->GlobalLighing;
				mapDefaultSky.Parameters = scenario->SkyParameters;
				mapDefaultSky.ScreenFX = scenario->DefaultScreenFx;
				mapDefaultSky.CameraFX = scenario->DefaultCameraFx;
				mapDefaultSky.Wind = scenario->StructureBsps[0].Wind;
				if(scenario->BackgroundSoundEnvironmentPalette2.Count)
					mapDefaultSky.BackgroundSound = scenario->BackgroundSoundEnvironmentPalette2[0].BackgroundSound;

				cachedReforgeTagIndexSet.reset();
			}

			if (!monitorState.IsValid)
			{
				monitorState.GrabOffset = RealVector3D(0, 0, 0);
				monitorState.MonitorLastTick = false;
				monitorState.NoclipLastTick = false;
				monitorState.IsValid = true;
				monitorState.HeldObjectIndex = -1;
			}
			
		}
	}

	void OnItemSpawned(ItemSpawnedCallback callback)
	{
		s_ItemSpawnedCallbacks.push_back(callback);
	}
}

namespace Forge
{
	bool IsReforgeObject(uint32_t objectIndex)
	{
		return CanThemeObject(objectIndex);
	}
}

namespace
{
	const auto object_dispose = (void(*)(uint32_t objectIndex))(0x00B2CD10);
	const auto object_spawn = (uint32_t(*)(void *placementData))(0x00B30440);
	const auto object_placement_data_new = (void(*)(void *placementData, uint32_t tagIndex, uint32_t ownerObjectIndex, void *a4))(0x00B31590);
	const auto object_set_position = (void(*)(uint32_t objectIndex, RealVector3D *position, RealVector3D *forward, RealVector3D *up, int scenarioLocation, int a8))(0x00B33550); // client only

	ForgeGlobalsDefinition *GetForgeGlobals()
	{
		if (!forgeGlobals_)
			forgeGlobals_ = Blam::Tags::TagInstance::Find('forg', "multiplayer\\forge_globals").GetDefinition<ForgeGlobalsDefinition>();
		return forgeGlobals_;
	}

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

		auto activeScreenCount = Pointer(0x05260F34)[0](0x3c).Read<int16_t>();
		if (activeScreenCount > 0)
			return;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == DatumHandle::Null)
			return;

		HandleCommands();

		uint32_t heldObjectIndex = -1, objectIndexUnderCrosshair = -1;


		auto isMonitor = GetEditorModeState(playerIndex, &heldObjectIndex, &objectIndexUnderCrosshair);

		if (isMonitor != monitorState.MonitorLastTick)
		{
			OnMonitorModeChange(isMonitor);
			monitorState.MonitorLastTick = isMonitor;
		}

		if (isMonitor)
		{
			const auto& moduleForge = Modules::ModuleForge::Instance();

			// only show selection when we're in monitor mode
			Forge::SelectionRenderer::SetEnabled(true);
			auto rendererType = Forge::SelectionRenderer::RendererImplementationType(moduleForge.VarSelectionRenderer->ValueInt);
			Forge::SelectionRenderer::SetRendererType(rendererType);
			Forge::Selection::Update();

			Forge::RotationSnap::Update(playerIndex, heldObjectIndex);
			HandleMovementSpeed();
			HandleMonitorNoclip();

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

				auto uiRightAction = GetActionState(Blam::Input::eGameActionUiRight);
				if (!(uiRightAction->Flags & Blam::Input::eActionStateFlagsHandled) && uiRightAction->Ticks == 50)
				{
					Forge::Selection::Clear();
					PrintKillFeedText(0, L"Selection Cleared", 0);
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

	uint32_t FindMapModifierObject()
	{
		auto objects = Blam::Objects::GetObjects();
		// Scan the object table to check if map modifier is spawned
		for(auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->Type != eObjectTypeScenery)
				continue;

			auto tagIndex = it->GetTagHandle().Index;
			if (tagIndex == MAP_MODIFIER_TAG_INDEX)
				return it.CurrentDatumIndex;
		}

		return -1;
	}

	Blam::Tags::Globals::ForgeGlobalsDefinition::Sky *GetForgeSky()
	{
		auto forgeGlobals = GetForgeGlobals();
		if (!forgeGlobals)
			return nullptr;
		if (mapModifierState.ForgeSky.Index == -1 || mapModifierState.ForgeSky.Index >= forgeGlobals->Skies.Count)
			return nullptr;

		return &forgeGlobals->Skies[mapModifierState.ForgeSky.Index];
	}

	Blam::DatumHandle FindObjectByName(Blam::Objects::ObjectType type, uint16_t name)
	{
		if (name == 0xffff)
			return Blam::DatumHandle::Null;

		auto objects = Blam::Objects::GetObjects();
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->Type == type && *(uint16_t*)((uint8_t*)it->Data + 0x9c) == name)
				return it.CurrentDatumIndex;
		}
		return Blam::DatumHandle::Null;
	}

	void ReplaceForgeSkyScenery(uint32_t newSceneryTagIndex)
	{
		// delete map default sky scenery children
		auto scenario = Blam::Tags::Scenario::GetCurrentScenario();
		for (auto &skyRef : scenario->SkyReferences)
		{
			for (auto &scenery : scenario->Scenery2)
			{
				if (scenery.ParentNameIndex != 0xffff && scenery.ParentNameIndex == skyRef.NameIndex)
				{
					auto objectIndex = FindObjectByName(Blam::Objects::ObjectType(scenery.Type), scenery.NameIndex);
					if (objectIndex != Blam::DatumHandle::Null && Blam::Objects::Get(objectIndex))
						object_dispose(objectIndex);
				}
			}
		}

		// delete the old sky scenery
		if (mapModifierState.ForgeSky.SceneryObjectIndex != -1)
		{
			if (Blam::Objects::Get(mapModifierState.ForgeSky.SceneryObjectIndex))
				object_dispose(mapModifierState.ForgeSky.SceneryObjectIndex);
			mapModifierState.ForgeSky.SceneryObjectIndex = -1;
		}

		if (newSceneryTagIndex == -1)
			return;

		// spawn the new scenery
		uint8_t objectPlacementData[0x18c];
		object_placement_data_new(objectPlacementData, newSceneryTagIndex, -1, nullptr);
		*(uint32_t*)&objectPlacementData[0x18] = 0x40u; // havok flags
		*(RealVector3D*)&objectPlacementData[0x1C] = RealVector3D(0, 0, 0.0f);
		auto skySceneryIndex = object_spawn(objectPlacementData);
		if (skySceneryIndex != -1)
		{
			auto skyObject = Blam::Objects::Get(skySceneryIndex);
			if (skyObject)
			{
				mapModifierState.ForgeSky.SceneryObjectIndex = skySceneryIndex;
			}
		}
	}

	void UpdateForgeSky(Forge::ForgeMapModifierProperties *mapModifierProperties)
	{
		const auto screen_effect_free = (int(*)(uint32_t screenEffectDatumIndex))(0x0);
		const auto GetLocalPlayerUnitObjectIndex = (uint32_t(*)(int playerIndex))(0x00589CC0);;
		const auto SpawnScreenEffect = (void(*)(uint32_t tagIndex, uint32_t unitObjectIndex, int a3, void* a4, void* a5))(0x683060);
		const auto object_looping_sound_new = (uint32_t(*)(uint32_t lsndTagIndex, uint32_t objectIndex, float a3))(0x005DB9A0);

		auto forgeGlobals = GetForgeGlobals();
		if (!forgeGlobals || !forgeGlobals->Skies.Count)
			return;

		auto desiredSkyIndex = mapModifierProperties->AtmosphereProperties.Skybox - 1;
		if (desiredSkyIndex >= 0)
		{
			if (mapModifierProperties->AtmosphereProperties.Flags & (1 << 1))
				mapModifierState.ForgeSky.Flags |= eForgeSkyFlags_UseMapModifierTransform;
			else
				mapModifierState.ForgeSky.Flags &= ~eForgeSkyFlags_UseMapModifierTransform;
		}

		auto &forgeSkyState = mapModifierState.ForgeSky;
		if (desiredSkyIndex != forgeSkyState.Index)
		{
			auto scenario = Blam::Tags::Scenario::GetCurrentScenario();

			ForgeGlobalsDefinition::Sky *desiredSky{ nullptr };

			// if the desired index is out of range or map default, populate the desired sky structure with scenario defaults
			if (desiredSkyIndex < 0 || desiredSkyIndex >= forgeGlobals->Skies.Count)
			{
				desiredSky = &mapDefaultSky;
			}
			else
			{
				desiredSky = &forgeGlobals->Skies[desiredSkyIndex];
			}

			auto screenEffects = ElDorito::GetMainTls(0x338).Read<Blam::DataArray<ScreenEffectDatum>*>();

			// if the scenario has a screen effect, delete it
			if (scenario->DefaultScreenFx.TagIndex != -1)
			{
				ScreenEffectDatum *datum{ nullptr };
				for (auto it = screenEffects->begin(); it != screenEffects->end(); ++it)
				{
					if (datum->TagIndex == scenario->DefaultScreenFx.TagIndex)
						screenEffects->Delete(it.CurrentDatumIndex);
				}
			}

			// delete the selected forge sky screen effect
			if (forgeSkyState.ScreenEffectDatumIndex != -1)
			{
				screenEffects->Delete(forgeSkyState.ScreenEffectDatumIndex);
				forgeSkyState.ScreenEffectDatumIndex = -1;
			}

			// spawn the new screen effect
			if (desiredSky->ScreenFX.TagIndex != -1)
			{
				auto screenEffects = ElDorito::GetMainTls(0x338).Read<Blam::DataArray<ScreenEffectDatum>*>();
				auto unitObjectIndex = GetLocalPlayerUnitObjectIndex(*(uint32_t*)0x018BF52C);

				uint8_t unk[0x3C] = { 0 };
				SpawnScreenEffect(desiredSky->ScreenFX.TagIndex, unitObjectIndex, 0, &unk, &unk);

				for (auto it = screenEffects->begin(); it != screenEffects->end(); ++it)
				{
					if (it->TagIndex == desiredSky->ScreenFX.TagIndex)
						forgeSkyState.ScreenEffectDatumIndex = it.CurrentDatumIndex;
				}
			}

			// if map default, try to find the existing sky scenery so that it can be replaced
			if (forgeSkyState.SceneryObjectIndex == -1)
			{
				auto skyObjectTagIndex = scenario->SkyReferences[0].SkyObject.TagIndex;
				auto objects = Blam::Objects::GetObjects();
				for (auto it = objects.begin(); it != objects.end(); ++it)
				{
					if (it->Type == Blam::Objects::eObjectTypeScenery && skyObjectTagIndex == it->Data->TagIndex)
						forgeSkyState.SceneryObjectIndex = it.CurrentDatumIndex;
				}
			}

			auto loopingSounds = ElDorito::GetMainTls(0xCC).Read<Blam::DataArray<s_object_looping_sound>*>();
			// delete any existing background sounds
			for (auto it = loopingSounds->begin(); it != loopingSounds->end(); ++it)
			{
				if (it->Flags & 3)
					loopingSounds->Delete(it.CurrentDatumIndex);
			}
			// delete the currently selected background sound
			if (mapModifierState.ForgeSky.BackgroundSoundDatumIndex != -1)
			{
				loopingSounds->Delete(forgeSkyState.BackgroundSoundDatumIndex);
				forgeSkyState.BackgroundSoundDatumIndex = -1;
			}
			// new up the background sound
			auto newObjectLoopingSoundIndex = object_looping_sound_new(desiredSky->BackgroundSound.TagIndex, -1, 1.0f);
			forgeSkyState.BackgroundSoundDatumIndex = newObjectLoopingSoundIndex;

			// replace sky scenery
			if (desiredSky)
			{
				forgeSkyState.Index = desiredSkyIndex;
				ReplaceForgeSkyScenery(desiredSky->Object.TagIndex);
			}

			scenario->SkyParameters = desiredSky->Parameters;
			scenario->GlobalLighing = desiredSky->GlobalLighting;
			scenario->DefaultCameraFx = desiredSky->CameraFX;
			scenario->StructureBsps[0].Wind = desiredSky->Wind;
		}
		else
		{
			if (forgeSkyState.Index != -1 && forgeSkyState.SceneryObjectIndex != -1)
			{
				
				if (mapModifierState.ForgeSky.Flags & eForgeSkyFlags_UseMapModifierTransform)
				{
					auto mapModifierObject = Blam::Objects::Get(mapModifierState.ObjectIndex);
					if (mapModifierObject)
					{
						auto zOffset = ((mapModifierProperties->AtmosphereProperties.SkyboxZOffset / 255.0f * 2.0f) - 1.0f) * 500.0f;
						auto position = mapModifierObject->Position;
						position.K += zOffset;
						object_set_position(forgeSkyState.SceneryObjectIndex, &position, &mapModifierObject->Forward, &mapModifierObject->Up, 0, 0);
					}
				}
				else
				{
					auto forgeSky = GetForgeSky();

					auto ci = std::cos(forgeSky->Orientation.I);
					auto cj = std::cos(forgeSky->Orientation.J);
					auto ck = std::cos(forgeSky->Orientation.K);
					auto si = std::sin(forgeSky->Orientation.I);
					auto sj = std::sin(forgeSky->Orientation.J);
					auto sk = std::sin(forgeSky->Orientation.K);

					RealVector3D forward{ ci*cj, (si*ck) - (sj*sk)*ci, (sj*ck)*ci + (si*sk) };
					RealVector3D up{ sj, (ci*sk) - (sj*ck*si), cj*ck };
					object_set_position(forgeSkyState.SceneryObjectIndex, &forgeSky->Translation, &forward, &up, 0, 0);
				}				
			}
		}
	}

	void UpdateMapModifier()
	{
		const auto physics_set_gravity_scale = (float(*)(float scale))(0x006818A0);
		const auto game_engine_round_in_progress = (bool(*)())(0x00550F90);

		auto mapModifierObjectIndex = FindMapModifierObject();
		mapModifierState.ObjectIndex = mapModifierObjectIndex;

		bool roundInProgress = game_engine_round_in_progress();
		
		if (mapModifierObjectIndex == -1 || (mapModifierState.WaitingForNewRound && roundInProgress))
		{
			// the map modifier was previously spawned, but has been deleted or new round has started
			if (mapModifierState.IsActive)
			{
				// delete any weather effects
				if (mapModifierState.Atmosphere.WeatherIndex != 0xff)
				{
					auto rasterizerGameStates = (uint8_t*)ElDorito::GetMainTls(0x3bc)[0];
					auto &currentWeatherEffectDatumIndex = *(uint32_t*)(rasterizerGameStates + 0x8);
					FreeWeatherEffect(currentWeatherEffectDatumIndex);
					currentWeatherEffectDatumIndex = -1;
				}

				auto forgeSky = GetForgeSky();
				if (forgeSky)
				{
					auto forgeSkyState = mapModifierState.ForgeSky;

					auto scenario = Blam::Tags::Scenario::GetCurrentScenario();
					ReplaceForgeSkyScenery(mapDefaultSky.Object.TagIndex);

					if (forgeSkyState.BackgroundSoundDatumIndex != -1)
					{
						auto loopingSounds = ElDorito::GetMainTls(0xCC).Read<Blam::DataArray<s_object_looping_sound>*>();
						loopingSounds->Delete(forgeSkyState.BackgroundSoundDatumIndex);
						forgeSkyState.BackgroundSoundDatumIndex = -1;
					}

					if (forgeSkyState.ScreenEffectDatumIndex != -1)
					{
						auto screenEffects = ElDorito::GetMainTls(0x338).Read<Blam::DataArray<ScreenEffectDatum>*>();
						screenEffects->Delete(forgeSkyState.ScreenEffectDatumIndex);
						forgeSkyState.ScreenEffectDatumIndex = -1;
					}

					scenario->SkyParameters = mapDefaultSky.Parameters;
					scenario->GlobalLighing = mapDefaultSky.GlobalLighting;
					scenario->DefaultCameraFx = mapDefaultSky.CameraFX;
					scenario->StructureBsps[0].Wind = mapDefaultSky.Wind;
				}

				physics_set_gravity_scale(1.0f);
			}

			// state will be reset next tick
			mapModifierState.IsValid = false;
		}

		mapModifierState.WaitingForNewRound = !roundInProgress;

		if (!mapModifierState.IsValid || !roundInProgress)
			return;

		auto mapModifierObject = Blam::Objects::Get(mapModifierObjectIndex);

		auto mpProperties = mapModifierObject->GetMultiplayerProperties();
		if (!mpProperties)
			return;

		mapModifierState.IsActive = true;

		auto mapModifierProperties = (Forge::ForgeMapModifierProperties*)&mpProperties->RadiusWidth;

		mapModifierState.MapBarriers.KillBarriersEnabled = !(mapModifierProperties->Flags & Forge::ForgeMapModifierProperties::eMapModifierFlags_DisableDeathBarrier);
		mapModifierState.MapBarriers.PushBarriersEnabled = !(mapModifierProperties->Flags & Forge::ForgeMapModifierProperties::eMapModifierFlags_DisablePushBarrier);

		const auto kMaxLightIntensity = 20000.0f;
		const auto kMaxBloomIntensity = 20000.0f;

		auto &cameraFxSettings = mapModifierState.CameraFx;
		auto &atmosphereSettings = mapModifierState.Atmosphere;

		cameraFxSettings.Enabled = true;
		cameraFxSettings.Exposure = std::pow((float)mapModifierProperties->CameraFxExposure * 0.05f, 2.2f) * 0.01f;
		auto expScale = cameraFxSettings.Exposure;
		if (std::abs(cameraFxSettings.Exposure) < 0.0001f)
			expScale = 1.0f;
		cameraFxSettings.LightIntensity = ((float)mapModifierProperties->CameraFxLightIntensity / expScale) * 0.07f;
		if (cameraFxSettings.LightIntensity > kMaxLightIntensity)
			cameraFxSettings.LightIntensity = kMaxLightIntensity;
		cameraFxSettings.BloomIntensity = ((float)mapModifierProperties->CameraFxBloom / expScale) * 0.07f;
		cameraFxSettings.LightBloomIntensity = std::pow(mapModifierProperties->CameraFxLightBloom / 255.0f * 2.0f, 2.2f);

		if (cameraFxSettings.BloomIntensity > kMaxBloomIntensity)
			cameraFxSettings.BloomIntensity = kMaxBloomIntensity;

		auto rasterizerGameStates = (uint8_t*)ElDorito::GetMainTls(0x3bc)[0];
		auto &currentWeatherEffectDatumIndex = *(uint32_t*)(rasterizerGameStates + 0x8);


		uint32_t newWeatherIndex = 0xff;

		auto forgeGlobals = GetForgeGlobals();
		auto desiredWeatherIndex = mapModifierProperties->AtmosphereProperties.Weather - 1;
		if (desiredWeatherIndex >= 0 && desiredWeatherIndex < forgeGlobals->WeatherEffects.Count)
		{
			newWeatherIndex = desiredWeatherIndex;
		}

		UpdateForgeSky(mapModifierProperties);

		if (atmosphereSettings.WeatherIndex != newWeatherIndex
			&& currentWeatherEffectDatumIndex != -1)
		{
			atmosphereSettings.WeatherIndex = 0xff;
			FreeWeatherEffect(currentWeatherEffectDatumIndex);
			currentWeatherEffectDatumIndex = -1;
		}
		else
		{
			atmosphereSettings.WeatherIndex = newWeatherIndex;
			UpdateWeatherEffects();
		}

		if (mapModifierProperties->AtmosphereProperties.Flags & 1)
		{
			atmosphereSettings.Enabled = true;
			atmosphereSettings.FogColor.Red = mapModifierProperties->AtmosphereProperties.FogColorR / 255.0f;
			atmosphereSettings.FogColor.Green = mapModifierProperties->AtmosphereProperties.FogColorG / 255.0f;
			atmosphereSettings.FogColor.Blue = mapModifierProperties->AtmosphereProperties.FogColorB / 255.0f;
			atmosphereSettings.FogDensity = mapModifierProperties->AtmosphereProperties.FogDensity / 255.0f * 200.0f;
			atmosphereSettings.FogVisibility = mapModifierProperties->AtmosphereProperties.FogVisibility / 255.0f * 200.0f;
			atmosphereSettings.Brightness = mapModifierProperties->AtmosphereProperties.Brightness / 255.0f * 4.0f;
		}
		else
		{
			atmosphereSettings.Enabled = false;
		}

		physics_set_gravity_scale(1.0f - (mapModifierProperties->PhysicsGravity / 255.0f));
	}

	bool CheckKillTriggersHook(int a0, void *a1)
	{
		if (!mapModifierState.MapBarriers.KillBarriersEnabled)
			return false;

		typedef bool(*CheckKillTriggersPtr)(int a0, void *a1);
		auto CheckKillTriggers = reinterpret_cast<CheckKillTriggersPtr>(0x68C410);
		return CheckKillTriggers(a0, a1);
	}

	bool ObjectSafeZoneHook(void *a0)
	{
		if (!mapModifierState.MapBarriers.KillBarriersEnabled)
			return true;

		typedef bool(*CheckSafeZonesPtr)(void *a0);
		auto CheckSafeZones = reinterpret_cast<CheckSafeZonesPtr>(0x4EB130);
		return CheckSafeZones(a0);
	}

	void* PushBarriersGetStructureDesignHook(int index)
	{
		if (!mapModifierState.MapBarriers.PushBarriersEnabled)
			return nullptr; // Return a null sddt if push barriers are disabled

		typedef void*(*GetStructureDesignPtr)(int index);
		auto GetStructureDesign = reinterpret_cast<GetStructureDesignPtr>(0x4E97D0);
		return GetStructureDesign(index);
	}

	void CanvasMap()
	{
		const auto c_map_variant_placement__ctor = (void(__thiscall*)(Blam::MapVariant::VariantPlacement *thisptr))(0x004AC1D0);

		auto mapv = GetMapVariant();
		if (!mapv)
			return;

		auto playerHandle = Blam::Players::GetLocalPlayer(0);

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (!placement.InUse())
				continue;

			auto& budget = mapv->Budget[placement.BudgetIndex];
			if (budget.TagIndex == -1 || budget.Cost == -1)
				continue;

			if (budget.TagIndex == 0x00002EA6)
				continue;

			if (placement.ObjectIndex != -1)
				DeleteObject(playerHandle.Index, i);
			else
			{
				if (i < mapv->ScnrPlacementsCount)
					placement.PlacementFlags |= 0x20u;
				else
				{
					memset(&placement, 0, sizeof(placement));
					c_map_variant_placement__ctor(&placement);
				}

				if (--budget.CountOnMap == 0)
				{
					budget.TagIndex = -1;
					budget.Cost = -1;
					budget.CountOnMap = 0;
					budget.DesignTimeMax = -1;
					budget.RuntimeMin = -1;
					budget.RuntimeMax = -1;
				}
			}
		}
	}

	void ResetRuntime()
	{
		struct c_game_engine_object_runtime_state
		{
			uint32_t Flags;
			int PlacementIndex;
			int ObjectIndex;
			int16_t TimeRemaining;
			uint16_t Unknown0E;
		};

		const auto c_game_engine_object_runtime_manager__respawn = (void(__thiscall *)(void*, bool a1))(0x00590CE0);
		const auto object_dispose = (void(*)(int objectIndex))(0x00B2CD10);

		auto runtime = (c_game_engine_object_runtime_state*)((uint8_t*)ElDorito::GetMainTls(0x48)[0] + 0x1059c);

		for (auto i = 0; i < 512; i++)
		{
			auto objectRuntime = &runtime[i];
			if (objectRuntime->PlacementIndex == -1 || objectRuntime->TimeRemaining == 0x7FFF)
				continue;

			if (objectRuntime->ObjectIndex != -1)
			{
				object_dispose(objectRuntime->ObjectIndex);
				objectRuntime->ObjectIndex = -1;
			}

			c_game_engine_object_runtime_manager__respawn(objectRuntime, true);
		}
	}

	void HandlePrefabCommands()
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		auto &modulePlayer = Modules::ModulePlayer::Instance();

		if (moduleForge.CommandState.Prefabs.LoadPrefab)
		{
			if (Prefabs::Load(moduleForge.CommandState.Prefabs.Path))
			{
				GrabSelection(Blam::Players::GetLocalPlayer(0));
			}
			else
			{
				PrintKillFeedText(0, L"ERROR: Failed to load prefab.", 0);
			}

			moduleForge.CommandState.Prefabs.LoadPrefab = false;
		}

		if (moduleForge.CommandState.Prefabs.SavePrefab)
		{
			if (!Prefabs::Save(modulePlayer.VarPlayerName->ValueString, moduleForge.CommandState.Prefabs.Name, moduleForge.CommandState.Prefabs.Path))
			{
				PrintKillFeedText(0, L"ERROR: Failed to save prefab.", 0);
			}
			moduleForge.CommandState.Prefabs.SavePrefab = false;
		}
	}

	void SpawnItem(int tagIndex)
	{
		if (tagIndex == -1)
			return;

		auto playerHandle = Blam::Players::GetLocalPlayer(0);
		if (playerHandle == DatumHandle::Null || !GetEditorModeState(playerHandle, nullptr, nullptr))
			return;

		Blam::Tags::TagInstance instance(tagIndex);
		auto def = instance.GetDefinition<void>();
		auto groupTag = def ? instance.GetGroupTag() : 0;

		if (!def || !(groupTag == 'weap' || groupTag == 'vehi' || groupTag == 'eqip'
			|| groupTag == 'scen' || groupTag == 'bloc' || groupTag == 'crea'
			|| groupTag == 'efsc' || groupTag == 'proj' || groupTag == 'bipd'
			|| groupTag == 'mach' || groupTag == 'ctrl' || groupTag == 'gint'
			|| groupTag == 'ssce'))
		{
			wchar_t buff[256];
			swprintf_s(buff, L"Tag not loaded or not a valid forge object [0x%04x]", instance.Index);
			PrintKillFeedText(0, buff, 0);
			return;
		}

		ForgeMessage msg = { 0 };
		msg.Type = 0;
		msg.PlayerIndex = playerHandle;
		msg.TagIndex = instance.Index;
		msg.CrosshairPoint = GetSandboxGlobals().CrosshairPoints[playerHandle.Index];

		static auto Forge_SendMessage = (void(*)(ForgeMessage*))(0x004735D0);
		Forge_SendMessage(&msg);
	}

	void HandleRuntimeReset()
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		if (moduleForge.CommandState.RuntimeReset)
		{
			ResetRuntime();
			moduleForge.CommandState.RuntimeReset = false;
		}
	}

	void HandleMapCanvas()
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		if (moduleForge.CommandState.MapCanvas)
		{
			ResetRuntime();
			CanvasMap();
			moduleForge.CommandState.MapCanvas = false;
		}
	}

	void HandleSetPrematchCamera()
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		if (moduleForge.CommandState.SetPrematchCamera)
		{
			::PrematchCamera::PlaceCameraObject();
			moduleForge.CommandState.SetPrematchCamera = false;
		}
	}

	void HandleSpawnItem()
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		auto &itemTagIndex = moduleForge.CommandState.SpawnItem;
		if (itemTagIndex != -1)
		{
			SpawnItem(itemTagIndex);
			itemTagIndex = -1;
		}
	}

	uint32_t SpawnMapmodifier()
	{
		auto mapv = Forge::GetMapVariant();
		if (!mapv)
			return -1;

		const auto Forge_SetPlacementVariantProperties = (void(*)(uint32_t playerIndex,
			int placementIndex, Blam::MapVariant::VariantProperties *properties))(0x0059B720);

		RealVector3D forward(1, 0, 0), up(0, 0, 1);
		auto mapModifierObjectIndex = Forge::SpawnObject(mapv, MAP_MODIFIER_TAG_INDEX, -1, -1,
			&Forge::GetSandboxGlobals().CrosshairPoints[0], &forward, &up, -1, -1, nullptr, 0);

		auto mapModifierObject = Blam::Objects::Get(mapModifierObjectIndex);

		if (mapModifierObjectIndex == -1 || !mapModifierObject || mapModifierObject->PlacementIndex == -1)
		{
			PrintKillFeedText(0, L"Failed to spawn map modifier object. Out of objects?", 0);
			return -1;
		}

		auto playerIndex = Blam::Players::GetLocalPlayer(0);

		auto &placement = mapv->Placements[mapModifierObject->PlacementIndex];
		placement.Properties.ZoneShape = 4; // phased
		Forge_SetPlacementVariantProperties(playerIndex, mapModifierObject->PlacementIndex, &placement.Properties);

		ObjectGrabbedHook(playerIndex, mapModifierObject->PlacementIndex);

		return mapModifierObjectIndex;
	}

	void HandleOpenMapOptions()
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		if (!moduleForge.CommandState.OpenMapOptions)
			return;

		moduleForge.CommandState.OpenMapOptions = false;

		auto mapModifierObjectIndex = FindMapModifierObject();
		if (mapModifierObjectIndex == -1)
			mapModifierObjectIndex = SpawnMapmodifier();

		if (mapModifierObjectIndex == -1)
			return;

		Web::Ui::WebConsole::Hide();
		Web::Ui::WebForge::ShowObjectProperties(mapModifierObjectIndex);
	}
	
	void HandleCommands()
	{
		HandleMapCanvas();
		HandleRuntimeReset();
		HandleSpawnItem();
		HandleSetPrematchCamera();
		HandlePrefabCommands();
		HandleOpenMapOptions();
	}

	void HandleRotationReset()
	{
		using namespace Blam::Input;

		auto uiUpAction = GetActionState(eGameActionUiUp);
		if ((!(uiUpAction->Flags & eActionStateFlagsHandled) && uiUpAction->Ticks == 1)
			|| GetMouseButtonTicks(eMouseButtonMiddle, eInputTypeGame) == 1)
		{
			uiUpAction->Flags |= Blam::Input::eActionStateFlagsHandled;

			uint32_t heldObjectIndex;
			if (Forge::GetEditorModeState(Blam::Players::GetLocalPlayer(0), &heldObjectIndex, nullptr))
			{
				Forge::RotationSnap::RotateToScripted(heldObjectIndex, RealQuaternion());
			}
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

	void HandleMonitorNoclip()
	{
		const auto object_set_havok_flags = (int(*)(uint32_t objectIndex, uint32_t havokFlags))(0x005C7380);

		const auto& moduleForge = Modules::ModuleForge::Instance();

		auto player = Blam::Players::GetPlayers().Get(Blam::Players::GetLocalPlayer(0));
		if (!player)
			return;
		auto unitObject = Blam::Objects::Get(player->SlaveUnit.Handle);
		if (!unitObject)
			return;

		auto noclipping = moduleForge.VarMonitorNoclip->ValueInt;
		if (bool(noclipping) != monitorState.NoclipLastTick)
		{
			if (!noclipping)
				object_set_havok_flags(player->SlaveUnit, 0);

			monitorState.NoclipLastTick = noclipping;
		}

		if (noclipping)
		{
			auto havokComponents = *(Blam::DataArray<Blam::DatumBase>**)0x02446080;
			auto havokComponent = (uint8_t*)havokComponents->Get(unitObject->HavokComponent);
			if (!havokComponent)
				return;
			auto rigidBodyCount = *(uint32_t*)(havokComponent + 0x18);
			if (!rigidBodyCount)
				return;
			auto rigidBody = *(uint8_t**)(*(uint8_t**)(havokComponent + 0x14) + 0x40);
			auto &collisionFilter = *(uint8_t*)(rigidBody + 0x2c);

			collisionFilter = 0x13;
		}
	}

	void __stdcall RotateHeldObjectHook(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot)
	{
		static auto RotateHeldObject = (void(__stdcall*)(uint32_t playerIndex, uint32_t objectIndex, float xRot, float yRot, float zRot))(0x0059DD50);

		static auto& moduleForge = Modules::ModuleForge::Instance();
		const auto currentSnap = moduleForge.VarRotationSnap->ValueInt;
		const auto rotationSensitvity = moduleForge.VarRotationSensitivity->ValueFloat;

		if (DatumHandle(playerIndex) != Blam::Players::GetLocalPlayer(0))
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
		monitorState.GrabOffset = RealVector3D(0, 0, 0);

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

		monitorState.GrabOffset = objectPtr(0x20).Read<RealVector3D>() - crosshairPoint;
		heldObjectDistance = (unitPos - crosshairPoint).Length();
	}

	uint32_t FindComplexObjectRigidBodyRootNode(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return -1;
		auto objectDefinition = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<Blam::Tags::Objects::Object>();
		auto modelDefinition = objectDefinition->Model.GetDefinition<uint8_t>();
		if (!modelDefinition)
			return -1;
		auto physicsModelDefinition = Blam::Tags::TagInstance(*(uint32_t*)(modelDefinition + 0x3C)).GetDefinition<uint8_t>();
		if (!physicsModelDefinition)
			return -1;
		auto rigidBodyCount = *(long*)(physicsModelDefinition + 0x58);
		if (rigidBodyCount < 2)
			return -1;

		for (auto i = 0; i < rigidBodyCount; i++)
		{
			auto rigidBody = *(uint8_t**)(physicsModelDefinition + 0x5C) + i * 0xB0;
			auto motionType = *(uint16_t*)(rigidBody + 0x1A);
			auto nodeIndex = *(uint16_t*)rigidBody;

			if (motionType == 0x4) // keyframed
				return nodeIndex;
		}

		return -1;
	}


	void ObjectGrabbedHook2(uint32_t playerIndex, uint32_t objectIndex)
	{
		static auto ObjectGrabbed2 = (void(__cdecl*)(uint32_t, uint32_t))(0x0059CF50);

		if (playerIndex == Blam::Players::GetLocalPlayer(0).Handle)
		{
			monitorState.HeldObjectIndex = objectIndex;
			monitorState.HeldObjectComplexRootRigidBodyNodeIndex = FindComplexObjectRigidBodyRootNode(objectIndex);
		}

		ObjectGrabbed2(playerIndex, objectIndex);
	}

	void __cdecl ObjectGrabbedHook(uint32_t playerIndex, uint16_t placementIndex)
	{
		static auto FreePlacement = (void(__thiscall *)(MapVariant* mapv, int16_t placementIndex, int a3))(0x585C00);
		static auto ObjectAttach = (void(__cdecl*)(uint32_t parentobjectIndex, uint32_t objectIndex, int a3))(0x00B2A250);
		static auto sub_59A620 = (void(__cdecl *)(int objectIndex, char a2))(0x59A620);

		auto& objects = Blam::Objects::GetObjects();
		auto mapv = GetMapVariant();

		auto objectIndex = mapv->Placements[placementIndex].ObjectIndex;

		if (playerIndex == Blam::Players::GetLocalPlayer(0).Handle)
		{
			monitorState.HeldObjectIndex = objectIndex;
		}

		static auto ObjectGrabbed = (void(__cdecl*)(uint32_t, uint32_t))(0x0059B080);
		ObjectGrabbed(playerIndex, placementIndex);

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

	void RecalculateMapVariantBudget()
	{
		auto mapv = GetMapVariant();

		// reset count on map
		for (auto i = 0; i < 256; i++)
			mapv->Budget[i].CountOnMap = 0;
		
		// count placements
		for (auto i = 0; i < 640; i++)
		{
			auto &placement = mapv->Placements[i];
			if(placement.PlacementFlags & 1)
				mapv->Budget[placement.BudgetIndex].CountOnMap++;
		}

		// fixup runtime
		for (auto i = 0; i < 256; i++)
		{
			auto &budgetItem = mapv->Budget[i];
			if (budgetItem.RuntimeMin > budgetItem.CountOnMap)
				budgetItem.RuntimeMin = budgetItem.CountOnMap;
			if (budgetItem.RuntimeMax < budgetItem.CountOnMap)
				budgetItem.RuntimeMax = budgetItem.CountOnMap;
		}
	}

	void ObjectOutOfWorldBoundsError()
	{
		PrintKillFeedText(0, L"ERROR: One or more objects outside of the world bounds", 0);
		UI_PlaySound(0, -1);
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

		auto droppedObject = Blam::Objects::Get(droppedObjectIndex);
		if (!droppedObject)
			return;


		if (!ObjectInWorldBounds(droppedObjectIndex))
		{
			ObjectOutOfWorldBoundsError();
			return;
		}

		std::stack<uint32_t> detachStack;
		auto& selection = Forge::Selection::GetSelection();
		if (selection.Contains(droppedObjectIndex))
		{
			for (auto objectIndex = droppedObject->FirstChild; objectIndex != DatumHandle::Null;)
			{
				auto object = Blam::Objects::Get(objectIndex);
				if (!object)
					continue;

				if (!ObjectInWorldBounds(objectIndex))
				{
					ObjectOutOfWorldBoundsError();
					return;
				}

				if (selection.Contains(objectIndex))
					detachStack.push(objectIndex);

				objectIndex = object->NextSibling;
			}
		}

		auto playerIndex = GetPlayerHoldingObject(droppedObjectIndex);
		if (playerIndex == Blam::Players::GetLocalPlayer(0).Handle)
		{
			monitorState.HeldObjectIndex = -1;
		}

		ObjectDropped(placementIndex, throwForce, a3);

		auto& magnetPair = Magnets::GetMagnetPair();
		if (magnetPair.IsValid)
		{
			RealVector3D heldObjectPosition;
			Object_GetPosition(droppedObjectIndex, &heldObjectPosition);
			heldObjectPosition = magnetPair.Dest->Position - (magnetPair.Source->Position - heldObjectPosition);
			Object_Transform(0, droppedObjectIndex, &heldObjectPosition, nullptr, nullptr);
		}

		while (!detachStack.empty())
		{
			auto objectIndex = detachStack.top();
			detachStack.pop();

			static auto ObjectDetach = (void(__cdecl*)(uint32_t objectIndex))(0x00B2D180);
			static auto AssignPlacement = (int(__thiscall *)(void *thisptr, uint32_t objectIndex, int16_t placementIndex))(0x5855E0);
			static auto sub_B313E0 = (void(__cdecl *)(int objectIndex, bool arg_4))(0xB313E0);

			auto object = Blam::Objects::Get(objectIndex);
			if (!object)
				continue;

			if (CreateOrGetBudgetForItem(mapv, object->TagIndex) == -1)
			{
				PrintKillFeedText(0, L"ERROR: Budget limit reached", 0);
				break;
			}

			ObjectDetach(objectIndex);
			AssignPlacement(mapv, objectIndex, -1);

			if (!IsReforgeObject(objectIndex))
			{
				sub_B313E0(objectIndex, true);
				ThrowObject(playerIndex, objectIndex, throwForce);
			}
		}

		RecalculateMapVariantBudget();
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
			monitorState.GrabOffset = RealVector3D(0, 0, 0);

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

		if (player && player->SlaveUnit == DatumHandle(unitObjectIndex) && GetEditorModeState(playerIndex, nullptr, nullptr))
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

		DatumHandle playerHandle = GetPlayerHoldingObject(objectIndex);

		const auto& selection = Forge::Selection::GetSelection();

		if (ObjectIsPhased(objectIndex))
		{
			auto offset = heldObject->Center - heldObject->Position - monitorState.GrabOffset;
			auto newPos = GetSandboxGlobals().CrosshairPoints[playerHandle.Index] - offset;
			Object_Transform(a1, objectIndex, &newPos, forwardVec, upVec);
		}
		else
		{
			Object_Transform(a1, objectIndex, position, forwardVec, upVec);
		}
	}

	bool HeldObjectHitTestHook(int a1, int a2, RealVector3D *start, RealVector3D *end, int objectIndexA, int objectIndexB, void *results)
	{
		const auto sub_6D7190 = (bool(*)(unsigned int flags, int a2, char a3, RealVector3D *start, RealVector3D *end, int a6, int a7, int a8, void *results))(0x6D7190);

		const auto &moduleForge = Modules::ModuleForge::Instance();
		if (objectIndexA != -1 && moduleForge.VarMonitorNoclip->ValueInt) // if we're holding an object in noclip
			return false;

		return sub_6D7190(a1, a2, 0, start, end, objectIndexA, objectIndexB, -1, results);
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

		if (object->TagIndex >= 0xffff) // something is wrong here...
			return false;
	
		if (cachedReforgeTagIndexSet.test(object->TagIndex))
			return true;

		if (!object->GetMultiplayerProperties())
			return false;
		auto objectDef = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<uint8_t>();

		auto hlmtDef = Blam::Tags::TagInstance(*(uint32_t*)(objectDef + 0x40)).GetDefinition<uint8_t>();
		if (!hlmtDef)
			return false;
		auto modeTagIndex = *(uint32_t*)(hlmtDef + 0xC);

		const auto modeDefinitionPtr = Pointer(Blam::Tags::TagInstance(modeTagIndex).GetDefinition<uint8_t>());
		if (!modeDefinitionPtr)
			return false;

		const auto materialCount = modeDefinitionPtr(0x48).Read<int32_t>();
		if (!materialCount)
			return false;

		const auto& firstMaterialShaderTagRef = modeDefinitionPtr(0x4c)[0].Read<Blam::Tags::TagReference>();
		if (firstMaterialShaderTagRef.TagIndex == REFORGE_DEFAULT_SHADER)
		{
			cachedReforgeTagIndexSet[object->TagIndex] = true;
			return true;
		}

		return false;
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

	bool ReforgeMaterialIsTransparent(void *renderData)
	{
		// TODO: this can probably be a little better optimized, exit as fast possible

		if (!renderData)
			return false;

		auto objectIndex = *(uint32_t*)((uint8_t*)renderData + 0x6C);

		auto modeTagIndex = Pointer(renderData)(0x4).Read<uint32_t>();

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

		auto materialRenderMethodTagIndex = *(uint32_t*)(*(uint8_t**)((uint8_t*)modeDefinitionPtr + 0x4C) + 0x24 * materialIndex + 0xC);

		if (materialRenderMethodTagIndex == -1)
			return false;

		auto renderMethod = Blam::Tags::TagInstance(materialRenderMethodTagIndex).GetDefinition<uint8_t>();
		
		return *(int32_t*)(*(uint8_t**)(renderMethod + 0x2C) + 0x68);
	}

	__declspec(naked) void RenderObjectTransparentHook()
	{
		__asm
		{
			add esp, 8
			cmp[eax + 0x68], 0
			jz loc_A79F8C
			mov bl, 1
			jmp loc_A79F8E

			loc_A79F8C:
			pushad
			push[ebp + 0xC]
			call ReforgeMaterialIsTransparent
			add esp, 4
			test al, al
			popad
			je still_not_transparent
			mov bl, 1
			jmp loc_A79F8E

			still_not_transparent:
			xor bl, bl
			loc_A79F8E :
			push 0x00A79F8E
			retn

		}
	}

	void RenderObjectCompressionInfoHook(uint8_t *compressionInfo)
	{
		const auto rasterizer_set_real_vertex_shader_constant = (void(*)(int start_register, int vec4_count, const float *data))(0x00A66410);

		struct s_render_model_compression_info {
			uint16_t field_0;
			uint16_t field_2;
			float position_bounds_x_min;
			float position_bounds_x_max;
			float position_bounds_y_min;
			float position_bounds_y_max;
			float position_bounds_z_min;
			float position_bounds_z_max;
			float texcoord_bounds_x_min;
			float texcoord_bounds_x_max;
			float texcoord_bounds_y_min;
			float texcoord_bounds_y_max;
		};

		auto info = (s_render_model_compression_info*)compressionInfo;

		float constantData[12];
		constantData[0] = info->position_bounds_x_max - info->position_bounds_x_min;
		constantData[1] = info->position_bounds_y_max - info->position_bounds_y_min;
		constantData[2] = info->position_bounds_z_max - info->position_bounds_z_min;
		constantData[3] = 0x3F800000;
		constantData[4] = info->position_bounds_x_min;
		constantData[5] = info->position_bounds_y_min;
		constantData[6] = info->position_bounds_z_min;
		constantData[7] = 0;

		if(customTextureModeState.IsValid)
		{
			auto tscale = customTextureModeState.Scale;
			constantData[8] = (info->texcoord_bounds_x_max - info->texcoord_bounds_x_min) / tscale;
			constantData[9] = (info->texcoord_bounds_y_max - info->texcoord_bounds_y_min) / tscale;
			constantData[10] = customTextureModeState.OffsetX;
			constantData[11] = customTextureModeState.OffsetY;
			rasterizer_set_real_vertex_shader_constant(12, 3, constantData);		
		}
		else
		{
			constantData[8] = info->texcoord_bounds_x_max - info->texcoord_bounds_x_min;
			constantData[9] = info->texcoord_bounds_y_max - info->texcoord_bounds_y_min;
			constantData[10] = info->texcoord_bounds_x_min;
			constantData[11] = info->texcoord_bounds_y_min;
		}
		
		rasterizer_set_real_vertex_shader_constant(12, 3, constantData);
	}

	int ObjectLightingQualityOverrideHook()
	{
		const auto preferences_get_lighting_quality = (int(*)())(0x0050B890);
		if (objectLightingQualityOverride)
			return 0;
		return preferences_get_lighting_quality();
	}

	bool ShouldOverrideLightingQuality(uint32_t objectIndex)
	{
		const auto kSpartanScaleThreshold = 1.5f;

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;

		auto objectType = *((uint8_t*)object + 0x9A);
		
		// biped object
		if (objectType == Blam::Objects::eObjectTypeBiped && object->Scale > kSpartanScaleThreshold)
			return true;
		// armor parts
		if (objectType == Blam::Objects::eObjectTypeScenery && object->Parent != Blam::DatumHandle::Null)
		{
			auto parentObject = Blam::Objects::Get(object->Parent);
			if (!parentObject)
				return false;

			auto parentObjectType = *((uint8_t*)parentObject + 0x9A);
			if (parentObjectType == Blam::Objects::eObjectTypeBiped
				&& parentObject->Scale > kSpartanScaleThreshold)
			{
				return true;
			}
		}

		return false;
	}

	void RenderMeshPartHook(void* data, int a2)
	{
		const auto is_forge = (bool(*)())(0x0059A780);

		static auto RenderMeshPart = (void(*)(void* data, int a2))(0xA78940);

		auto renderData = Pointer(data)[4];
		if (!renderData)
			return;

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

			if (ShouldOverrideLightingQuality(objectIndex))
			{
				objectLightingQualityOverride = true;
				RenderMeshPart(data, a2);
				objectLightingQualityOverride = false;
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

				auto properties = (ReforgeObjectProperties*)(&object->GetMultiplayerProperties()->RadiusWidth);
				if (properties->Flags & Forge::ReforgeObjectProperties::Flags::MaterialOverrideTextureCoords)
				{
					customTextureModeState.Scale = properties->TextureData.Scale / 255.0f * Forge::kReforgeMaxTextureScale;
					customTextureModeState.OffsetX = properties->TextureData.OffsetX / 255.0f;
					customTextureModeState.OffsetY = properties->TextureData.OffsetY / 255.0f;
					customTextureModeState.IsValid = true;
				}

				auto oldMaterialIndex = *materialIndexPtr;
				*materialIndexPtr = materialIndex;
				RenderMeshPart(data, a2);
				*materialIndexPtr = oldMaterialIndex;
				customTextureModeState.IsValid = false;
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

		if (Blam::Players::GetLocalPlayer(0).Index == playerIndex)
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

	void MapVariant_BuildVariantPropertiesFromObjectHook(Blam::MapVariant::VariantPlacement *placement)
	{
		const auto sub_B63150 = (uint16_t(*)(uint32_t objectIndex, uint16_t, bool))(0xB63150);

		if (placement->ObjectIndex == -1)
			return;
		auto objectHeader = Blam::Objects::GetObjects().Get(placement->ObjectIndex);
		if (!objectHeader)
			return;
		auto mpProperties = objectHeader->Data->GetMultiplayerProperties();
		if (!mpProperties)
			return;

		placement->Properties.EngineFlags = mpProperties->EngineFlags;
		placement->Properties.ObjectFlags = 0;
		placement->Properties.TeamAffilation = mpProperties->TeamIndex & 0xff;
		placement->Properties.SharedStorage = 0;
		placement->Properties.ZoneShape = mpProperties->Shape;

		if (((mpProperties->Flags & 0xff) >> 6) & 1)
			placement->Properties.ObjectFlags |= 1u;
		else
			placement->Properties.ObjectFlags &= ~1u;

		// placed at start
		if (((mpProperties->Flags & 0xff) >> 7) & 1)
			placement->Properties.ObjectFlags |= 2u;
		else
			placement->Properties.ObjectFlags &= ~2u;

		// symmetry
		placement->Properties.ObjectFlags |= 0xCu;
		if (mpProperties->Symmetry == 1)
			placement->Properties.ObjectFlags &= ~8u;
		else
		{
			if (mpProperties->Symmetry == 2)
				placement->Properties.ObjectFlags &= 0xFB;
		}

		if (mpProperties->SpawnTime < 0 || mpProperties->SpawnTime >= 255)
			placement->Properties.RespawnTime = 0;
		else
			placement->Properties.RespawnTime = mpProperties->SpawnTime & 0xff;

		// zone properties
		memcpy((void*)&placement->Properties.ZoneRadiusWidth, (void*)&mpProperties->RadiusWidth, 16);

		// spare clips
		placement->Properties.ObjectType = mpProperties->ObjectType;
		if (objectHeader->Type == Blam::Objects::eObjectTypeWeapon)
		{
			auto weaponDefinition = Blam::Tags::TagInstance(objectHeader->Data->TagIndex).GetDefinition<Blam::Tags::Items::Weapon>();
			if (weaponDefinition->Magazines.Count > 0)
			{
				auto &magazine = weaponDefinition->Magazines[0];
				auto rounds = sub_B63150(placement->ObjectIndex, 0, true);
				if (magazine.RoundsTotalLoadedMaximum)
					placement->Properties.SharedStorage = rounds / magazine.RoundsTotalLoadedMaximum;
			}
		}
		else
		{
			if (mpProperties->ObjectType >= 9 && mpProperties->ObjectType <= 11)
				placement->Properties.SharedStorage = mpProperties->TeleporterChannel;
			else
				placement->Properties.SharedStorage = mpProperties->SpareClips;
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
		const auto sub_715E90 = (char(__cdecl *)(float a1, float boundingRadius, int a3, int a4,
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

	struct LightDatum : public Blam::DatumBase
	{
		__int16 Flags;
		int TagIndex;
		int LightVolumeIndex;
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

	float __fastcall LightIlluminationFunction(uint8_t *thisptr, uint32_t lightDatumIndex, float a1, float a2)
	{
		struct {
			uint8_t type;
			uint8_t flags;
			uint8_t field_2;
			uint8_t field_3;
			float minimum;
			float maximum;
			uint32_t field_C;
			uint32_t field_10;
			uint32_t field_14;
			uint32_t field_18;
			uint32_t field_1C;
			struct {
				uint32_t function;
				float frequency;
				float field_8;
				float field_C;
				float field_10;
			} params;
		} static functionData;

		struct {
			uint32_t size;
			uint32_t field_4;
			uint32_t field_8;
			void *data;
			uint32_t field_10;
		} static functionRef;

		const auto sub_949CE0 = (float(__thiscall *)(void *thisptr, float a1, float a2))(0x00949CE0);
		const auto LightIlluminationFunction = (float(__thiscall*)(void *thisptr, float value, float scale))(0x00949CE0);

		auto& lightArray = ElDorito::GetMainTls(0x498)[0].Read<Blam::DataArray<LightDatum>>();
		auto light = lightArray.Get(lightDatumIndex);

		auto lightObject = Blam::Objects::Get(light->ObjectIndex);
		if (lightObject)
		{
			auto lightDef = Blam::Tags::TagInstance(light->TagIndex).GetDefinition<uint8_t>();
			auto lightFlags = *(uint32_t*)lightDef;

			auto mpProperties = lightObject->GetMultiplayerProperties();
			if (lightFlags & (1 << 31) && mpProperties)
			{
				auto lightProperties = reinterpret_cast<const ForgeLightProperties*>((uint8_t*)mpProperties + 0x14);
				if (lightProperties->IlluminationFunction.Type == ForgeLightProperties::eForgeLightIlluminationFunctionType_None)
					return 1.0f;

				functionData.type = 3;
				functionData.flags = 48;
				functionData.minimum = lightProperties->IlluminationFunction.Base / 255.0f;
				functionData.maximum = 1.0f;
				functionData.field_C = 0;
				functionData.field_10 = 0;
				functionData.field_14 = 0;
				functionData.field_18 = 0;
				functionData.field_1C = 14;

				float frequency = lightProperties->IlluminationFunction.Freq / 255.0f * kMaxForgeLightIlluminationFrequency;

				switch (lightProperties->IlluminationFunction.Type)
				{
				case ForgeLightProperties::eForgeLightIlluminationFunctionType_Pulse:
					functionData.params.function = 2;
					break;
				case ForgeLightProperties::eForgeLightIlluminationFunctionType_Flicker:
					frequency *= 0.2f;
					functionData.params.function = 8;
					break;

				}
				functionData.params.frequency = frequency;
				functionData.params.field_8 = 0;
				functionData.params.field_C = 0;
				functionData.params.field_10 = 1;

				functionRef.size = 52;
				functionRef.data = &functionData;

				return LightIlluminationFunction(&functionRef, a1, a2);
			}
		}

		return sub_949CE0(thisptr, a1, a2);
	}

	void __fastcall LightColorFunction(void *thisptr, uint32_t lightDatumIndex, float t, float a3, RealColorRGB *color)
	{
		const auto sub_9498D0 = (void(__thiscall *)(void *thisptr, float t, float a3, RealColorRGB *rgb))(0x9498D0);

		auto& lightArray = ElDorito::GetMainTls(0x498)[0].Read<Blam::DataArray<LightDatum>>();
		auto light = lightArray.Get(lightDatumIndex);
		auto lightObject = Blam::Objects::Get(light->ObjectIndex);
		if (lightObject)
		{
			auto lightDef = Blam::Tags::TagInstance(light->TagIndex).GetDefinition<uint8_t>();
			auto lightFlags = *(uint32_t*)lightDef;

			auto mpProperties = lightObject->GetMultiplayerProperties();
			if (lightFlags & (1 << 31) && mpProperties)
			{
				if (mpProperties)
				{
					auto lightProperties = reinterpret_cast<const ForgeLightProperties*>((uint8_t*)mpProperties + 0x14);
					auto intensity = lightProperties->Intensity / 255.0f * kMaxForgeLightIntensity * 300.0f;
					color->Red = lightProperties->ColorR / 255.0f * intensity;
					color->Green = lightProperties->ColorG / 255.0f * intensity;
					color->Blue = lightProperties->ColorB / 255.0f * intensity;
					return;
				}
			}
		}

		sub_9498D0(thisptr, t, a3, color);
	}

	void __declspec(naked) LightIlluminationFunctionHook()
	{
		__asm
		{
			mov edx, [ebp + 0x8]
			jmp LightIlluminationFunction
		}
	}

	void __declspec(naked) LightColorFunctionHook()
	{
		__asm
		{
			mov edx, [ebp + 0x8]
			jmp LightColorFunction
		}
	}

	void UpdateLightHook(uint32_t lightDatumIndex, int a2, float intensity, int a4)
	{		
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
			if (props)
			{
				auto lightProperties = reinterpret_cast<const Forge::ForgeLightProperties*>((uint8_t*)props + 0x14);

				auto v23 = *(float*)(lightDef + 0x74);
				auto v24 = lightProperties->Type == Forge::ForgeLightProperties::eForgeLightType_Point ? 1.0f : 0.0f;
				auto fieldOfView = lightProperties->FieldOfView / 255.0f * Forge::kMaxForgeLightFovDegrees * 0.0174533f;
				auto range = lightProperties->Range / 65535.0f * Forge::kMaxForgeLightRange;
				auto color = RealColorRGB {
					intensity * light->Color.Red,
					intensity * light->Color.Green,
					intensity *light->Color.Blue
				};

				return sub_A66900(a4, light->Center, color, 6.0f, range, light->field_A4, fieldOfView, v23, v24);
			}
		}

		sub_A667F0(lightDatumIndex, a2, intensity, a4);
	}


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
		const auto sub_682A90 = (unsigned __int32(__thiscall *)(void *thisptr, void *a2, float a3, float a4, int a5, void * a6))(0x682A90);
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
						if ((playerIndex != DatumHandle::Null && (player = Blam::Players::GetPlayers().Get(playerIndex))) &&
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
							screenEffectDef.PrimaryHue = uint32_t(properties->Hue / 255.0f * 360);
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

		UpdateMapModifier();

		if (!is_client())
			::Forge::Volumes::Update();
	}

	bool GameObjectResetHook(uint32_t objectIndex)
	{
		const auto sub_54E9B0 = (bool(*)(uint32_t))(0x54E9B0);

		if (mapModifierState.IsValid)
		{
			if (!mapModifierState.MapBarriers.KillBarriersEnabled || !mapModifierState.MapBarriers.PushBarriersEnabled)
				return false;
		}

		return sub_54E9B0(objectIndex);
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
			auto reforgeProperties = (Forge::ReforgeObjectProperties*)&mpProperties->RadiusWidth;
			if (reforgeProperties->Flags & Forge::ReforgeObjectProperties::Flags::MaterialAllowsProjectiles
				&& !Forge::GetEditorModeState(Blam::Players::GetLocalPlayer(0), nullptr, nullptr))
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

		auto &cameraFxSettings = mapModifierState.CameraFx;

		if (cameraFxSettings.Enabled)
		{
			if (std::abs(mapModifierState.CameraFx.Exposure) > 0.0001f)
				*(float*)((uint8_t*)thisptr + 0x29C) = cameraFxSettings.Exposure;
			if (std::abs(cameraFxSettings.LightIntensity) > 0.0001f)
				*(float*)((uint8_t*)thisptr + 0x1E48) = cameraFxSettings.LightIntensity;
			if (std::abs(cameraFxSettings.BloomIntensity) > 0.0001f)
				*(float*)((uint8_t*)thisptr + 0x2A0) = cameraFxSettings.BloomIntensity;
			if (std::abs(cameraFxSettings.LightBloomIntensity) > 0.0001f)
				*(float*)(*(uint8_t**)((uint8_t*)thisptr + 0x298) + 0x290) = cameraFxSettings.LightBloomIntensity;
			
		}
	}

	void __cdecl ShieldImpactBloomHook(int id, int count, float *data)
	{
		auto &cameraFxSettings = mapModifierState.CameraFx;

		const auto rasterizer_set_pixel_shader_constant = (void(*)(int id, int count, float *data))(0x00A66270);
		if (cameraFxSettings.Enabled && std::abs(cameraFxSettings.BloomIntensity) > 0.0001f)
		{
			data[12] *= cameraFxSettings.BloomIntensity;
			data[13] *= cameraFxSettings.BloomIntensity;
			data[14] *= cameraFxSettings.BloomIntensity;
			data[15] *= cameraFxSettings.BloomIntensity;
		}
		rasterizer_set_pixel_shader_constant(id, count, data);
	}

	void RenderAtmosphereHook(short bspIndex, void *state)
	{
		struct sky_atm_parameters_properties_definition
		{
			uint16_t Flags;
			uint16_t Unknown;
			uint32_t Name;
			float LightSourceY;
			float LightSourceX;
			Blam::Math::RealColorRGB FogColor;
			float Brightness;
			float FogGradientThreshold;
			float LightIntensity;
			float SkyinvisiblilityThroughFog;
			float Unknown2C;
			float Unknown30;
			float LightSourceSpread;
			float Unknown38;
			float FogIntensity;
			float Unknown40;
			float TintCyan;
			float TintMagenta;
			float TintYellow;
			float FogIntensityCyan;
			float FogIntensityMagenta;
			float FogIntensityYellow;
			float BackgroundColorRed;
			float BackgroundColorGreen;
			float BackgroundColorBlue;
			float TintRed;
			float TintGreen;
			float TintBlue;
			float FogIntensity2;
			float StartDistance;
			float EndDistance;
			float FogVelocityX;
			float FogVelocityY;
			float FogVelocityZ;
			Blam::Tags::TagReference Weather;
			float Unknown9C;
			uint32_t UnknownA0;
		};
		static_assert(sizeof(sky_atm_parameters_properties_definition) == 0xA4, "");


		const auto sub_6739A0 = (void(*)(short bspIndex, void *state))(0x6739A0);
		const auto sub_671D90 = (void(*)(__int16 sbspIndex, sky_atm_parameters_properties_definition *atmosphereProperties,
			void *state, float intensity))(0x671D90);

		const auto &atmosphereSettings = mapModifierState.Atmosphere;
		if (atmosphereSettings.Enabled)
		{
			auto scenario = Blam::Tags::Scenario::GetCurrentScenario();
			auto skya = Blam::Tags::TagInstance(scenario->SkyParameters.TagIndex).GetDefinition<uint8_t>();
			if (!skya)
				return;

			auto atmospherePropertiesBlock = (Blam::Tags::TagBlock<sky_atm_parameters_properties_definition>*)(skya + 0x34);
			if (!atmospherePropertiesBlock->Count)
				return;

			const auto properties = atmospherePropertiesBlock->Elements[0];

			sky_atm_parameters_properties_definition props = { 0 };
			props.Flags = 7;
			props.LightSourceY = properties.LightSourceY;
			props.LightSourceX = properties.LightSourceX;
			props.FogColor = atmosphereSettings.FogColor;
			props.Brightness = atmosphereSettings.Brightness;
			props.FogGradientThreshold = -100.0f + atmosphereSettings.FogVisibility;
			props.LightIntensity = 100;
			props.SkyinvisiblilityThroughFog = 25;
			props.Unknown2C = 0.08f;
			props.Unknown30 = 2.5f;
			props.LightSourceSpread = properties.LightSourceSpread;
			props.Unknown38 = 1.0f;
			props.FogIntensity = -100.0f + atmosphereSettings.FogDensity;
			props.Unknown40 = 65535;
			props.TintCyan = 0.0009897007f;
			props.TintMagenta = 0.0009897007f;
			props.TintYellow = 0.0009897007f;
			props.FogIntensityCyan = 0.1970554f;
			props.FogIntensityMagenta = 0.1970554f;
			props.FogIntensityYellow = 0.1970554f;
			props.BackgroundColorBlue = (float)6.008439E-05;
			props.BackgroundColorGreen = (float)6.008439E-05;
			props.BackgroundColorRed = (float)6.008439E-05;
			props.TintRed = 0.04638588f;
			props.TintGreen = 0.04638588f;
			props.TintBlue = 0.04638588f;
			props.FogIntensity2 = properties.FogIntensity2;
			props.StartDistance = properties.StartDistance;
			props.EndDistance = properties.EndDistance;
			props.FogVelocityX = properties.FogVelocityX;
			props.FogVelocityY = properties.FogVelocityY;
			props.FogVelocityZ = properties.FogVelocityZ;
			sub_671D90(bspIndex, &props, state, 1.0f);
		}
		else
		{
			sub_6739A0(bspIndex, state);
		}
	}



	bool UpdateWeatherEffects()
	{
		auto &atmosphereSettings = mapModifierState.Atmosphere;

		if (!mapModifierState.IsValid || atmosphereSettings.WeatherIndex == 0xff)
			return false;

		const auto sub_5B9720 = (char(*)(uint32_t effectDatumIndex, uint16_t *a2))(0x5B9720);
		const auto SpawnWeatherEffect = (unsigned int(*)(int effectTagIndex))(0x005B8BD0);

		auto rasterizerGameStates = (uint8_t*)ElDorito::GetMainTls(0x3bc)[0];
		auto &currentWeatherEffectDatumIndex = *(uint32_t*)(rasterizerGameStates + 0x8);

		if (currentWeatherEffectDatumIndex == -1)
		{
			auto forgeGlobals = GetForgeGlobals();
			if (forgeGlobals)
			{
				auto effectTagIndex = forgeGlobals->WeatherEffects[atmosphereSettings.WeatherIndex].Effect.TagIndex;
				if (effectTagIndex != -1)
					currentWeatherEffectDatumIndex = SpawnWeatherEffect(effectTagIndex);

				if (currentWeatherEffectDatumIndex != -1)
				{
					uint16_t scenarioLocation = 0;
					sub_5B9720(currentWeatherEffectDatumIndex, &scenarioLocation);
				}
			}
		}

		return true;
	}

	__declspec(naked) void BspWeatherHook()
	{
		__asm
		{
			cmp al, [ebp+0x10]
			jne disabled
			call UpdateWeatherEffects
			test al, al
			jne disabled
			push 0x00672244
			retn
			disabled:
			add esp, 0x28
			mov esp, ebp
			pop ebp
			ret
		}
	}

	void OnMonitorModeChange(bool isMonitor)
	{
	}

	void BackgroundSoundEnvironmentHook(float a1)
	{
		const auto sub_5DAB20 = (void(*)(float a1))(0x5DAB20);
		auto forgeSky = GetForgeSky();
		if (!mapModifierState.IsValid || !forgeSky)
			return sub_5DAB20(a1);	
	}

	void scenario_place_sky_scenery_hook(uint32_t scenarioTagIndex, uint32_t activeBsp)
	{
		const auto scenario_place_sky_scenery_hook = (void(*)(uint32_t scenarioTagIndex, uint32_t activeBsp))(0x00B594C0);

		if (GetForgeSky())
			return;

		scenario_place_sky_scenery_hook(scenarioTagIndex, activeBsp);
	}

	bool IgnoreRigidBodyNode(uint32_t objectIndex, int nodeIndex)
	{
		return monitorState.HeldObjectIndex != -1 
			&& monitorState.HeldObjectIndex == objectIndex 
			&& monitorState.HeldObjectComplexRootRigidBodyNodeIndex != -1
			&& nodeIndex != monitorState.HeldObjectComplexRootRigidBodyNodeIndex;
	}

	__declspec(naked) void EvaluateRigidBodyNodesHook()
	{
		__asm
		{
			movsx edx, word ptr[ecx]
			pushad
			push edx
			push[ebp + 0x8]
			call IgnoreRigidBodyNode
			add esp, 8
			test al, al
			popad
			jz skip
			push 0x00712998
			retn
			skip :
			cmp edx, -01
			push 0x00712942
			retn
		}
	}

	uint32_t HavokCleanupHook(signed int a1, int islandEntityIndex, char a3, char a4, char a5, int a6)
	{
		const auto HavokCleanup = (uint32_t(*)(signed int a1, int islandEntityIndex, char a3, char a4, char a5, int a6))(0x5C4E30);
		auto objectIndex = HavokCleanup(a1, islandEntityIndex, a3, a4, a5, a6);

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return -1;

		if (CanThemeObject(objectIndex))
			return -1;

		return objectIndex;
	}

	void __fastcall c_havok_component__activate_hook(uint8_t *thisptr, void *unused)
	{
		const auto c_havok_component = (void(__thiscall*)(void *thisptr))(0x005E6A50);

		auto objectIndex = *(uint32_t*)(thisptr + 0x8);
		if (objectIndex != -1 && IsReforgeObject(objectIndex))
		{
			auto rigidBodyCount = *(uint32_t*)(thisptr + 0x18);
			for (auto i = 0; i < rigidBodyCount; i++)
			{
				auto rigidBody = *(uint8_t**)(*(uint8_t**)(thisptr + 0x14) + 0x48 * i + 0x40);
				*(rigidBody + 0xA8) = 7; // fixed
			}
		}

		c_havok_component(thisptr);
	}
}
