#pragma once

#include "../Blam/BlamTypes.hpp"
#include "../Blam/Math/RealMatrix4x3.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Forge
{
	const auto kMaxForgeLightRange = 50.0f;
	const auto kMaxForgeLightIntensity = 2.0f;
	const auto kMaxForgeLightIlluminationFrequency = 2.0f;
	const auto kMaxForgeLightFovDegrees = 180.0f;

	struct ForgeLightProperties
	{
		enum
		{
			eForgeLightIlluminationFunctionType_None = 0,
			eForgeLightIlluminationFunctionType_Pulse,
			eForgeLightIlluminationFunctionType_Flicker
		};

		enum
		{
			eForgeLightType_Point = 0,
			eForgeLightType_Directional
		};

		uint8_t Type : 2;
		uint8_t Flags : 6;
		uint8_t ColorR;
		uint8_t ColorG;
		uint8_t ColorB;
		uint8_t Intensity;
		uint16_t Range;
		uint8_t FieldOfView;
		uint8_t NearWidth; // for later
		struct {
			uint8_t Type : 3;
			uint8_t Reserved : 5;
			uint8_t Base;
			uint8_t Freq;
		} IlluminationFunction;
	};
	static_assert(sizeof(ForgeLightProperties) < 16, "ForgeLightProperties must be 16 bytes or less in size");


	struct ForgeScreenFxProperties
	{
		uint8_t Flags;
		uint8_t LightIntensity;
		uint8_t Hue;
		uint8_t Saturation;
		uint8_t Desaturation;
		uint8_t GammaIncrease;
		uint8_t GammaDecrease;
		uint8_t MaximumDistance;
		uint8_t ColorFilterR;
		uint8_t ColorFilterG;
		uint8_t ColorFilterB;
		uint8_t ColorFloorR;
		uint8_t ColorFloorG;
		uint8_t ColorFloorB;
		uint8_t Tracing;
		uint8_t DistanceFalloffFunction : 4;
		uint8_t TimeEvolutionFunction : 4;
	};
	static_assert(sizeof(ForgeScreenFxProperties) <= 16, "ForgeScreenFxData must be 16 bytes or less in size");

	const float kReforgeMaxTextureScale = 4.0f;
	struct ReforgeObjectProperties
	{
		enum Flags
		{
			MaterialOverrideTextureCoords = (1 << 0),
			MaterialAllowsProjectiles = (1 << 1)
		};

		uint8_t ColorR;
		uint8_t ColorG;
		uint8_t ColorB;
		uint8_t Flags;
		struct
		{
			uint8_t Scale;
			uint8_t OffsetX;
			uint8_t OffsetY;		
		} TextureData;
	};
	static_assert(sizeof(ReforgeObjectProperties) <= 16, "ReforgeObjectProperties must be 16 bytes or less in size");
	

	struct ForgeMapModifierProperties
	{
		enum MapModifierFlags : uint16_t
		{
			eMapModifierFlags_DisablePushBarrier = (1 << 0),
			eMapModifierFlags_DisableDeathBarrier = (1 << 1)
		};
		uint16_t Flags;
		uint8_t CameraFxExposure;
		uint8_t CameraFxLightIntensity;
		uint8_t CameraFxBloom;
		uint8_t CameraFxLightBloom;
		struct  {
			uint8_t Flags : 4;
			uint8_t Weather : 4;
			uint8_t FogColorR;
			uint8_t FogColorG;
			uint8_t FogColorB;
			uint8_t FogDensity;
			uint8_t FogVisibility;
			uint8_t Brightness;
			uint8_t Skybox : 8;
			uint8_t SkyboxZOffset;
		} AtmosphereProperties;
		uint8_t PhysicsGravity;
	};
	static_assert(sizeof(ForgeMapModifierProperties) <= 16, "ForgeMapModifierProperties must be 16 bytes or less in size");

	struct ForgeGarbageVolumeProperties
	{
		enum GarbageVolumeFlags
		{
			eGarbageVolumeFlags_CollectDeadBipeds = (1 << 0),
			eGarbageVolumeFlags_CollectWeapons = (1 << 1),
			eGarbageVolumeFlags_CollectObjectives = (1 << 2),
			eGarbageVolumeFlags_CollectEquipment = (1 << 3),
			eGarbageVolumeFlags_CollectGrenades = (1 << 4),
			eGarbageVolumeFlags_CollectVehicles = (1 << 5)
		};

		enum GarbageVolumeInterval
		{
			eGarbageVolumeInterval_Instant = 0,
			eGarbageVolumeInterval_3_Seconds = 1,
			eGarbageVolumeInterval_15_Seconds = 2,
			eGarbageVolumeInterval_30_Seconds = 3,
		};

		uint8_t Flags : 6;
		uint8_t Interval : 2;
	};
	static_assert(sizeof(ForgeGarbageVolumeProperties) <= 1, "ForgeGarbageVolumeProperties must be 1 bytes or less in size");

	struct ForgeKillVolumeProperties
	{
		enum KillVolumeFlags
		{
			eKillVolumeFlags_AlwaysVisible = (1 << 0),
			eKillVolumeFlags_DestroyVehicles = (1 << 1)
		};

		enum KillVolumeDamageCause
		{
			eKillVolumeDamageCause_Default = 0,
			eKillVolumeDamageCause_Guardians,
			eKillVolumeDamageCause_Falling
		};

		uint8_t Flags : 6;
		uint8_t DamageCause : 2;
	};
	static_assert(sizeof(ForgeKillVolumeProperties) <= 1, "ForgeKillVolumeProperties must be 1 bytes or less in size");

	struct ZoneShape
	{
		int Shape;
		int ShapeShader;
		float RadiusWidth;
		float Length;
		float Top;
		float Bottom;
		Blam::Math::RealMatrix4x3 Transform;
		float BoundingRadius;
	};
	static_assert(sizeof(ZoneShape) == 0x50, "Invalid ZoneShape size");

	struct SandboxGlobals
	{
		uint32_t Unknown00;
		uint32_t Unknown04;
		uint16_t PlayersInMonitorModeBitSet;
		uint16_t Unknown06;
		uint32_t HeldObjects[16];
		float HeldObjectDistances[16];
		Blam::Math::RealVector3D CrosshairPoints[16];
		Blam::Math::RealVector3D CrosshairIntersectNormals[16];
		Blam::Math::RealVector3D CrosshairDirections[16];
		uint32_t CrosshairObjects[16];
		uint8_t UnknownE49c[0x43c];
	};
	static_assert(sizeof(SandboxGlobals) == 0x748, "Invalid SandboxGlobals size");

	struct ForgeMessage
	{
		uint32_t Type;
		uint32_t TagIndex;
		uint32_t PlacementIndex;
		uint32_t PlayerIndex;
		uint32_t Unknown10;
		int8_t QuotaMin;
		int8_t QuotaMax;
		uint16_t Unknown16;
		Blam::Math::RealVector3D CrosshairPoint;
		uint32_t SelectedGameType;
		uint16_t EngineFlags;
		uint8_t Flags;
		uint8_t Team;
		uint8_t SharedStorage;
		uint8_t SpawnTime;
		uint8_t ObjectType;
		uint8_t ShapeType;
		uint32_t Unknown30;
		uint32_t Unknown34;
		uint32_t Unknown38;
		uint32_t Unknown3c;
	};
	static_assert(sizeof(ForgeMessage) == 0x40, "Invalid ForgeMessage size");


	const auto GetObjectZoneShape = (void(__cdecl *)(uint32_t objectIndex, ZoneShape* zoneShape, int a3))(0xBA0AD0);
	const auto PointIntersectsZone = (bool(__cdecl*)(const Blam::Math::RealVector3D* point, const ZoneShape* zone))(0x00BA11F0);
	const auto GetObjectTransformationMatrix = (void(__cdecl*)(uint32_t objectIndex, Blam::Math::RealMatrix4x3* outMatrix))(0x00B2EC60);
	const auto GetObjectPosition = (void(*)(uint32_t objectIndex, Blam::Math::RealVector3D *position))(0xB2E5A0);
	const auto SpawnObject = (uint32_t(__thiscall *)(Blam::MapVariant* thisptr, uint32_t tagIndex, int a3, int16_t placementIndex,
		const Blam::Math::RealVector3D *position, const Blam::Math::RealVector3D *rightVec, const Blam::Math::RealVector3D *upVec, int16_t scnrPlacementBlockIndex,
		int objectType, const Blam::MapVariant::VariantProperties* variantProperties, uint16_t placementFlags))(0x00582110);
	const auto GetPlayerHoldingObject = (uint32_t(__cdecl*)(int objectIndex))(0x0059BB90);
	const auto GetEditorModeState = (bool(__cdecl *)(uint32_t playerIndex, uint32_t* heldObjectIndex, uint32_t* objectIndexUnderCrosshair))(0x59A6F0);
	const auto GetSandboxGlobals = (SandboxGlobals&(*)())(0x0059BC10);
	const auto ObjectIsPhased = (bool(*)(uint32_t objectIndex))(0x0059A7B0);
	const auto PrintKillFeedText = (void(__cdecl *)(int hudIndex, wchar_t *text, int a3))(0x00A95920);

	Blam::MapVariant* GetMapVariant();

	struct AABB { float MinX, MaxX, MinY, MaxY, MinZ, MaxZ; };
	const AABB* GetObjectBoundingBox(uint32_t tagIndex);
	Blam::Math::RealVector3D GetClosestCardianalAxix(const Blam::Math::RealVector3D& v);
	void DeleteObject(uint16_t playerIndex, int16_t placementIndex);
	uint32_t CloneObject(uint32_t playerIndex, uint32_t objectIndex, float depth, const Blam::Math::RealVector3D &normal);
	void ThrowObject(uint32_t playerIndex, uint32_t objectIndex, float throwForce);
	bool PointInWorldBounds(Blam::Math::RealVector3D &point);
	bool ObjectInWorldBounds(uint32_t objectIndex);

	extern bool IsReforgeObject(uint32_t objectIndex);

	struct ForgeObjectQuota
	{
		uint16_t TotalObjectsAvailable;
		uint16_t TotalObjectsUsed;
	};
	ForgeObjectQuota CalculateObjectQuota();
}
