#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "../../../Pointer.hpp"
#include "../../Math/RealEulerAngles2D.hpp"
#include "../../Math/RealPoint3D.hpp"

namespace Blam::Tags::Scenario
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct Scenario;

	inline Scenario *GetCurrentScenario()
	{
		auto scenario = Pointer::Base(0x1EAAEB4);
		return scenario.Read<Scenario *>();
	}

	struct Scenario : TagGroup<'scnr'>
	{
		struct StructureBsp;
		struct SkyReference;
		struct BspGroup;
		struct ScenarioBspAudibility;
		struct ScenarioZonesetGroup;
		struct ObjectName;
		struct Scenery;
		struct SceneryPalette;
		struct Biped;
		struct BipedPalette;
		struct Vehicle;
		struct VehiclePalette;
		struct Equipment;
		struct EquipmentPalette;
		struct Weapon;
		struct WeaponPalette;
		struct DeviceGroup;
		struct Machine;
		struct MachinePalette;
		struct Terminal;
		struct TerminalPalette;
		struct AlternateRealityDevice;
		struct AlternateRealityDevicePalette;
		struct Control;
		struct ControlPalette;
		struct SoundScenery;
		struct SoundSceneryPalette;
		struct Giant;
		struct GiantPalette;
		struct EffectSceneryBlock;
		struct EffectSceneryBlock2;
		struct LightVolume;
		struct LightVolumesPalette;
		struct SandboxVehicle;
		struct SandboxWeapon;
		struct SandboxEquipment;
		struct SandboxScenery;
		struct SandboxTeleporter;
		struct SandboxGoalObject;
		struct SandboxSpawning;
		struct SoftCeiling;
		struct PlayerStartingProfileBlock;
		struct PlayerStartingLocation;
		struct TriggerVolume;
		struct MultiplayerConstantsOverride;
		struct Unknown41;
		struct Decal;
		struct DecalPalette;
		struct StylePalette;
		struct SquadGroup;
		struct Squad;
		struct Zone;
		struct Unknown82;
		struct CharacterPalette;
		struct AiPathfindingDatum;
		struct Script;
		struct Global;
		struct ScriptReference;
		struct ScriptingDatum;
		struct CutsceneFlag;
		struct CutsceneCameraPoint;
		struct CutsceneTitle;
		struct ScenarioResource;
		struct UnitSeatsMapping;
		struct ScenarioKillTrigger;
		struct ScenarioSafeTrigger;
		struct ScriptExpression;
		struct BackgroundSoundEnvironmentPalette;
		struct Unknown108;
		struct Fog;
		struct CameraFxBlock;
		struct ScenarioClusterDatum;
		struct SpawnDatum;
		struct Crate;
		struct CratePalette;
		struct FlockPalette;
		struct Flock;
		struct CreaturePalette;
		struct EditorFolder;
		struct MissionDialogue;
		struct Interpolator;
		struct SimulationDefinitionTable;
		struct Unknown134;
		struct AiObjective;
		struct DesignerZoneset;
		struct Unknown136;
		struct Cinematic;
		struct CinematicLighting;
		struct ScenarioMetagame;
		struct Unknown144;
		struct Unknown146;
		struct Unknown159;

		int16_t MapType;
		uint16_t Flags;
		int32_t Unknown;
		int32_t MapId;
		float LocalNorth;
		float SandboxBudget;
		TagBlock<StructureBsp> StructureBsps;
		TagReference Unknown2;
		TagBlock<SkyReference> SkyReferences;
		TagBlock<BspGroup> BspGroups;
		TagBlock<ScenarioBspAudibility> ScenarioBspAudibility2;
		TagBlock<ScenarioZonesetGroup> ScenarioZonesetGroups;
		uint32_t Unknown3;
		uint32_t Unknown4;
		uint32_t Unknown5;
		uint32_t Unknown6;
		uint32_t Unknown7;
		uint32_t Unknown8;
		uint32_t Unknown9;
		uint32_t Unknown10;
		uint32_t Unknown11;
		uint32_t Unknown12;
		uint32_t Unknown13;
		uint32_t Unknown14;
		uint32_t Unknown15;
		uint32_t Unknown16;
		uint32_t Unknown17;
		uint32_t Unknown18;
		uint32_t Unknown19;
		uint32_t Unknown20;
		uint32_t Unknown21;
		uint32_t Unknown22;
		uint32_t Unknown23;
		uint32_t Unknown24;
		uint32_t Unknown25;
		TagBlock<ObjectName> ObjectNames;
		TagBlock<Scenery> Scenery2;
		TagBlock<SceneryPalette> SceneryPalette2;
		TagBlock<Biped> Bipeds;
		TagBlock<BipedPalette> BipedPalette2;
		TagBlock<Vehicle> Vehicles;
		TagBlock<VehiclePalette> VehiclePalette2;
		TagBlock<Equipment> Equipment2;
		TagBlock<EquipmentPalette> EquipmentPalette2;
		TagBlock<Weapon> Weapons;
		TagBlock<WeaponPalette> WeaponPalette2;
		TagBlock<DeviceGroup> DeviceGroups;
		TagBlock<Machine> Machines;
		TagBlock<MachinePalette> MachinePalette2;
		TagBlock<Terminal> Terminals;
		TagBlock<TerminalPalette> TerminalPalette2;
		TagBlock<AlternateRealityDevice> AlternateRealityDevices;
		TagBlock<AlternateRealityDevicePalette> AlternateRealityDevicePalette2;
		TagBlock<Control> Controls;
		TagBlock<ControlPalette> ControlPalette2;
		TagBlock<SoundScenery> SoundScenery2;
		TagBlock<SoundSceneryPalette> SoundSceneryPalette2;
		TagBlock<Giant> Giants;
		TagBlock<GiantPalette> GiantPalette2;
		TagBlock<EffectSceneryBlock> EffectScenery;
		TagBlock<EffectSceneryBlock2> EffectScenery2;
		TagBlock<LightVolume> LightVolumes;
		TagBlock<LightVolumesPalette> LightVolumesPalette2;
		TagBlock<SandboxVehicle> SandboxVehicles;
		TagBlock<SandboxWeapon> SandboxWeapons;
		TagBlock<SandboxEquipment> SandboxEquipment2;
		TagBlock<SandboxScenery> SandboxScenery2;
		TagBlock<SandboxTeleporter> SandboxTeleporters;
		TagBlock<SandboxGoalObject> SandboxGoalObjects;
		TagBlock<SandboxSpawning> SandboxSpawning2;
		TagBlock<SoftCeiling> SoftCeilings;
		TagBlock<PlayerStartingProfileBlock> PlayerStartingProfile;
		TagBlock<PlayerStartingLocation> PlayerStartingLocations;
		TagBlock<TriggerVolume> TriggerVolumes;
		uint32_t Unknown26;
		uint32_t Unknown27;
		uint32_t Unknown28;
		uint32_t Unknown29;
		uint32_t Unknown30;
		uint32_t Unknown31;
		TagBlock<MultiplayerConstantsOverride> MultiplayerConstantsOverride2;
		uint32_t Unknown32;
		uint32_t Unknown33;
		uint32_t Unknown34;
		uint32_t Unknown35;
		uint32_t Unknown36;
		uint32_t Unknown37;
		uint32_t Unknown38;
		uint32_t Unknown39;
		uint32_t Unknown40;
		TagBlock<Unknown41> Unknown42;
		uint32_t Unknown43;
		uint32_t Unknown44;
		uint32_t Unknown45;
		uint32_t Unknown46;
		uint32_t Unknown47;
		uint32_t Unknown48;
		uint32_t Unknown49;
		uint32_t Unknown50;
		uint32_t Unknown51;
		uint32_t Unknown52;
		uint32_t Unknown53;
		uint32_t Unknown54;
		uint32_t Unknown55;
		uint32_t Unknown56;
		uint32_t Unknown57;
		uint32_t Unknown58;
		uint32_t Unknown59;
		uint32_t Unknown60;
		uint32_t Unknown61;
		uint32_t Unknown62;
		uint32_t Unknown63;
		uint32_t Unknown64;
		uint32_t Unknown65;
		uint32_t Unknown66;
		uint32_t Unknown67;
		uint32_t Unknown68;
		uint32_t Unknown69;
		uint32_t Unknown70;
		uint32_t Unknown71;
		uint32_t Unknown72;
		uint32_t Unknown73;
		uint32_t Unknown74;
		uint32_t Unknown75;
		uint32_t Unknown76;
		uint32_t Unknown77;
		uint32_t Unknown78;
		TagBlock<Decal> Decals;
		TagBlock<DecalPalette> DecalPalette2;
		uint32_t Unknown79;
		uint32_t Unknown80;
		uint32_t Unknown81;
		TagBlock<StylePalette> StylePalette2;
		TagBlock<SquadGroup> SquadGroups;
		TagBlock<Squad> Squads;
		TagBlock<Zone> Zones;
		TagBlock<Unknown82> Unknown83;
		uint32_t Unknown84;
		uint32_t Unknown85;
		uint32_t Unknown86;
		TagBlock<CharacterPalette> CharacterPalette2;
		uint32_t Unknown87;
		uint32_t Unknown88;
		uint32_t Unknown89;
		TagBlock<AiPathfindingDatum> AiPathfindingData;
		uint32_t Unknown90;
		uint32_t Unknown91;
		uint32_t Unknown92;
		TagData<uint8_t> ScriptStrings;
		TagBlock<Script> Scripts;
		TagBlock<Global> Globals;
		TagBlock<ScriptReference> ScriptReferences;
		uint32_t Unknown93;
		uint32_t Unknown94;
		uint32_t Unknown95;
		TagBlock<ScriptingDatum> ScriptingData;
		TagBlock<CutsceneFlag> CutsceneFlags;
		TagBlock<CutsceneCameraPoint> CutsceneCameraPoints;
		TagBlock<CutsceneTitle> CutsceneTitles;
		TagReference CustomObjectNameStrings;
		TagReference ChapterTitleStrings;
		TagBlock<ScenarioResource> ScenarioResources;
		TagBlock<UnitSeatsMapping> UnitSeatsMapping2;
		TagBlock<ScenarioKillTrigger> ScenarioKillTriggers;
		TagBlock<ScenarioSafeTrigger> ScenarioSafeTriggers;
		TagBlock<ScriptExpression> ScriptExpressions;
		uint32_t Unknown96;
		uint32_t Unknown97;
		uint32_t Unknown98;
		uint32_t Unknown99;
		uint32_t Unknown100;
		uint32_t Unknown101;
		TagBlock<BackgroundSoundEnvironmentPalette> BackgroundSoundEnvironmentPalette2;
		uint32_t Unknown102;
		uint32_t Unknown103;
		uint32_t Unknown104;
		uint32_t Unknown105;
		uint32_t Unknown106;
		uint32_t Unknown107;
		TagBlock<Unknown108> Unknown109;
		TagBlock<Fog> Fog2;
		TagBlock<CameraFxBlock> CameraFx;
		uint32_t Unknown110;
		uint32_t Unknown111;
		uint32_t Unknown112;
		uint32_t Unknown113;
		uint32_t Unknown114;
		uint32_t Unknown115;
		uint32_t Unknown116;
		uint32_t Unknown117;
		uint32_t Unknown118;
		TagBlock<ScenarioClusterDatum> ScenarioClusterData;
		uint32_t Unknown119;
		uint32_t Unknown120;
		uint32_t Unknown121;
		int32_t ObjectSalts1;
		int32_t ObjectSalts2;
		int32_t ObjectSalts3;
		int32_t ObjectSalts4;
		int32_t ObjectSalts5;
		int32_t ObjectSalts6;
		int32_t ObjectSalts7;
		int32_t ObjectSalts8;
		int32_t ObjectSalts9;
		int32_t ObjectSalts10;
		int32_t ObjectSalts11;
		int32_t ObjectSalts12;
		int32_t ObjectSalts13;
		int32_t ObjectSalts14;
		int32_t ObjectSalts15;
		int32_t ObjectSalts16;
		int32_t ObjectSalts17;
		int32_t ObjectSalts18;
		int32_t ObjectSalts19;
		int32_t ObjectSalts20;
		int32_t ObjectSalts21;
		int32_t ObjectSalts22;
		int32_t ObjectSalts23;
		int32_t ObjectSalts24;
		int32_t ObjectSalts25;
		int32_t ObjectSalts26;
		int32_t ObjectSalts27;
		int32_t ObjectSalts28;
		int32_t ObjectSalts29;
		int32_t ObjectSalts30;
		int32_t ObjectSalts31;
		int32_t ObjectSalts32;
		TagBlock<SpawnDatum> SpawnData;
		TagReference SoundEffectsCollection;
		TagBlock<Crate> Crates;
		TagBlock<CratePalette> CratePalette2;
		TagBlock<FlockPalette> FlockPalette2;
		TagBlock<Flock> Flocks;
		TagReference SubtitleStrings;
		uint32_t Unknown122;
		uint32_t Unknown123;
		uint32_t Unknown124;
		TagBlock<CreaturePalette> CreaturePalette2;
		TagBlock<EditorFolder> EditorFolders;
		TagReference TerritoryLocationNameStrings;
		uint32_t Unknown125;
		uint32_t Unknown126;
		TagBlock<MissionDialogue> MissionDialogue2;
		TagReference ObjectiveStrings;
		TagBlock<Interpolator> Interpolators;
		uint32_t Unknown127;
		uint32_t Unknown128;
		uint32_t Unknown129;
		uint32_t Unknown130;
		uint32_t Unknown131;
		uint32_t Unknown132;
		TagBlock<SimulationDefinitionTable> SimulationDefinitionTable2;
		TagReference DefaultCameraFx;
		TagReference DefaultScreenFx;
		TagReference Unknown133;
		TagReference SkyParameters;
		TagReference GlobalLighing;
		TagReference Lightmap;
		TagReference PerformanceThrottles;
		TagBlock<Unknown134> Unknown135;
		TagBlock<AiObjective> AiObjectives;
		TagBlock<DesignerZoneset> DesignerZonesets;
		TagBlock<Unknown136> Unknown137;
		uint32_t Unknown138;
		uint32_t Unknown139;
		uint32_t Unknown140;
		TagBlock<Cinematic> Cinematics;
		TagBlock<CinematicLighting> CinematicLighting2;
		uint32_t Unknown141;
		uint32_t Unknown142;
		uint32_t Unknown143;
		TagBlock<ScenarioMetagame> ScenarioMetagame2;
		TagBlock<Unknown144> Unknown145;
		TagBlock<Unknown146> Unknown147;
		uint32_t Unknown148;
		uint32_t Unknown149;
		uint32_t Unknown150;
		uint32_t Unknown151;
		uint32_t Unknown152;
		uint32_t Unknown153;
		uint32_t Unknown154;
		uint32_t Unknown155;
		uint32_t Unknown156;
		TagReference Unknown157;
		TagReference Unknown158;
		TagBlock<Unknown159> Unknown160;

		struct StructureBsp
		{
			TagReference StructureBspRef;
			TagReference Design;
			TagReference Lighting;
			int32_t Unknown;
			float Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			int16_t Unknown5;
			int16_t Unknown6;
			int16_t Unknown7;
			int16_t Unknown8;
			TagReference Cubemap;
			TagReference Wind;
			int32_t Unknown9;
		};
		TAG_STRUCT_SIZE_ASSERT(StructureBsp, 0x6C);

		struct SkyReference
		{
			TagReference SkyObject;
			int16_t NameIndex;
			uint16_t ActiveBsps;
		};
		TAG_STRUCT_SIZE_ASSERT(SkyReference, 0x14);

		struct BspGroup
		{
			struct BspChecksum;
			struct Bsp;
			struct Bsp2;

			int32_t IncludedBsps;
			int32_t Unknown;
			TagBlock<BspChecksum> BspChecksums;
			TagBlock<Bsp> Bsps;
			TagBlock<Bsp2> Bsps2;

			struct BspChecksum
			{
				int32_t StructureChecksum;
			};
			TAG_STRUCT_SIZE_ASSERT(BspChecksum, 0x4);

			struct Bsp
			{
				struct Cluster;
				struct Cluster2;
				struct ClusterSky;
				struct ClusterVisibleSky;
				struct Unknown;
				struct Unknown3;
				struct Cluster3;

				TagBlock<Cluster> Clusters;
				TagBlock<Cluster2> Clusters2;
				TagBlock<ClusterSky> ClusterSkies;
				TagBlock<ClusterVisibleSky> ClusterVisibleSkies;
				TagBlock<Unknown> Unknown2;
				TagBlock<Unknown3> Unknown4;
				TagBlock<Cluster3> Clusters3;

				struct Cluster
				{
					struct Bsp;

					TagBlock<Bsp> Bsps;

					struct Bsp
					{
						struct Unknown;

						TagBlock<Unknown> Unknown2;

						struct Unknown
						{
							int32_t Allow;
						};
						TAG_STRUCT_SIZE_ASSERT(Unknown, 0x4);
					};
					TAG_STRUCT_SIZE_ASSERT(Bsp, 0xC);
				};
				TAG_STRUCT_SIZE_ASSERT(Cluster, 0xC);

				struct Cluster2
				{
					struct Bsp;

					TagBlock<Bsp> Bsps;

					struct Bsp
					{
						struct Unknown;

						TagBlock<Unknown> Unknown2;

						struct Unknown
						{
							int32_t Allow;
						};
						TAG_STRUCT_SIZE_ASSERT(Unknown, 0x4);
					};
					TAG_STRUCT_SIZE_ASSERT(Bsp, 0xC);
				};
				TAG_STRUCT_SIZE_ASSERT(Cluster2, 0xC);

				struct ClusterSky
				{
					int8_t SkyIndex;
				};
				TAG_STRUCT_SIZE_ASSERT(ClusterSky, 0x1);

				struct ClusterVisibleSky
				{
					int8_t SkyIndex;
				};
				TAG_STRUCT_SIZE_ASSERT(ClusterVisibleSky, 0x1);

				struct Unknown
				{
					uint32_t Unknown1;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown, 0x4);

				struct Unknown3
				{
					uint32_t Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown3, 0x4);

				struct Cluster3
				{
					struct Unknown;

					TagBlock<Unknown> Unknown2;

					struct Unknown
					{
						int8_t Unknown1;
					};
					TAG_STRUCT_SIZE_ASSERT(Unknown, 0x1);
				};
				TAG_STRUCT_SIZE_ASSERT(Cluster3, 0xC);
			};
			TAG_STRUCT_SIZE_ASSERT(Bsp, 0x54);

			struct Bsp2
			{
				struct Unknown;
				struct Unknown3;

				TagBlock<Unknown> Unknown2;
				TagBlock<Unknown3> Unknown4;

				struct Unknown
				{
					int16_t Unknown1;
					int16_t Unknown2;
					int32_t Unknown3;
					int16_t Unknown4;
					int16_t Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown, 0xC);

				struct Unknown3
				{
					int16_t Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown3, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(Bsp2, 0x18);
		};
		TAG_STRUCT_SIZE_ASSERT(BspGroup, 0x2C);

		struct ScenarioBspAudibility
		{
			struct EncodedDoorPa;
			struct ClusterDoorPortalEncodedPa;
			struct AiDeafeningPa;
			struct ClusterDistance;
			struct Bsp;
			struct BspClusterList;
			struct ClusterMapping;

			int32_t DoorPortalCount;
			int32_t UniqueClusterCount;
			float ClusterDistanceBoundsMin;
			float ClusterDistanceBoundsMax;
			TagBlock<EncodedDoorPa> EncodedDoorPas;
			TagBlock<ClusterDoorPortalEncodedPa> ClusterDoorPortalEncodedPas;
			TagBlock<AiDeafeningPa> AiDeafeningPas;
			TagBlock<ClusterDistance> ClusterDistances;
			TagBlock<Bsp> Bsps;
			TagBlock<BspClusterList> BspClusterList2;
			TagBlock<ClusterMapping> ClusterMapping2;

			struct EncodedDoorPa
			{
				int32_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(EncodedDoorPa, 0x4);

			struct ClusterDoorPortalEncodedPa
			{
				int32_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(ClusterDoorPortalEncodedPa, 0x4);

			struct AiDeafeningPa
			{
				int32_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(AiDeafeningPa, 0x4);

			struct ClusterDistance
			{
				int8_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(ClusterDistance, 0x1);

			struct Bsp
			{
				int32_t Start;
				int32_t Count;
			};
			TAG_STRUCT_SIZE_ASSERT(Bsp, 0x8);

			struct BspClusterList
			{
				int32_t StartIndex;
				int32_t ClusterCount;
			};
			TAG_STRUCT_SIZE_ASSERT(BspClusterList, 0x8);

			struct ClusterMapping
			{
				int16_t Index;
			};
			TAG_STRUCT_SIZE_ASSERT(ClusterMapping, 0x2);
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioBspAudibility, 0x64);

		struct ScenarioZonesetGroup
		{
			StringID Name;
			int32_t BspGroupIndex;
			int32_t ImportLoadedBsps;
			int32_t LoadedBsps;
			int32_t LoadedDesignerZonesets;
			int32_t UnloadedDesignerZonesets;
			int32_t LoadedCinematicZonesets;
			int32_t BspAtlasIndex;
			int32_t ScenarioBspAudibilityIndex;
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioZonesetGroup, 0x24);

		struct ObjectName
		{
			char Name[32];
			int16_t Type;
			int16_t PlacementIndex;
		};
		TAG_STRUCT_SIZE_ASSERT(ObjectName, 0x24);

		struct Scenery
		{
			struct NodePositioning;
			struct PathfindingReference;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			int16_t PathfindingPolicy;
			int16_t LightmappingPolicy;
			TagBlock<PathfindingReference> PathfindingReferences;
			int16_t Unknown10;
			int16_t Unknown11;
			int32_t Symmetry;
			uint16_t EngineFlags;
			int16_t Team;
			int8_t SpawnSequence;
			int8_t RuntimeMinimum;
			int8_t RuntimeMaximum;
			uint8_t MultiplayerFlags;
			int16_t SpawnTime;
			int16_t AbandonTime;
			int8_t Unknown12;
			int8_t Shape;
			int8_t TeleporterChannel;
			int8_t Unknown13;
			int16_t Unknown14;
			int16_t AttachedNameIndex;
			uint32_t Unknown15;
			uint32_t Unknown16;
			float WidthRadius;
			float Depth;
			float Top;
			float Bottom;
			uint32_t Unknown17;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);

			struct PathfindingReference
			{
				int16_t BspIndex;
				int16_t PathfindingObjectIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(PathfindingReference, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(Scenery, 0xB8);

		struct SceneryPalette
		{
			TagReference Scenery;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(SceneryPalette, 0x30);

		struct Biped
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			float BodyVitalityPercentage;
			int32_t Flags;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Biped, 0x78);

		struct BipedPalette
		{
			TagReference Biped;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(BipedPalette, 0x30);

		struct Vehicle
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			float BodyVitalityPercentage;
			int32_t Flags;
			int32_t Symmetry;
			uint16_t EngineFlags;
			int16_t Team;
			int8_t SpawnSequence;
			int8_t RuntimeMinimum;
			int8_t RuntimeMaximum;
			uint8_t MultiplayerFlags;
			int16_t SpawnTime;
			int16_t AbandonTime;
			int8_t Unknown10;
			int8_t Shape;
			int8_t TeleporterChannel;
			int8_t Unknown11;
			int16_t Unknown12;
			int16_t AttachedNameIndex;
			uint32_t Unknown13;
			uint32_t Unknown14;
			float WidthRadius;
			float Depth;
			float Top;
			float Bottom;
			uint32_t Unknown15;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle, 0xAC);

		struct VehiclePalette
		{
			TagReference Vehicle;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(VehiclePalette, 0x30);

		struct Equipment
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			int32_t EquipmentFlags;
			int32_t Symmetry;
			uint16_t EngineFlags;
			int16_t Team;
			int8_t SpawnSequence;
			int8_t RuntimeMinimum;
			int8_t RuntimeMaximum;
			uint8_t MultiplayerFlags;
			int16_t SpawnTime;
			int16_t AbandonTime;
			int8_t Unknown6;
			int8_t Shape;
			int8_t TeleporterChannel;
			int8_t Unknown7;
			int16_t Unknown8;
			int16_t AttachedNameIndex;
			uint32_t Unknown9;
			uint32_t Unknown10;
			float WidthRadius;
			float Depth;
			float Top;
			float Bottom;
			uint32_t Unknown11;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Equipment, 0x8C);

		struct EquipmentPalette
		{
			TagReference Equipment;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(EquipmentPalette, 0x30);

		struct Weapon
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			int16_t RoundsLeft;
			int16_t RoundsLoaded;
			int32_t WeaponFlags;
			int32_t Symmetry;
			uint16_t EngineFlags;
			int16_t Team;
			int8_t SpawnSequence;
			int8_t RuntimeMinimum;
			int8_t RuntimeMaximum;
			uint8_t MultiplayerFlags;
			int16_t SpawnTime;
			int16_t AbandonTime;
			int8_t Unknown10;
			int8_t Shape;
			int8_t TeleporterChannel;
			int8_t Unknown11;
			int16_t Unknown12;
			int16_t AttachedNameIndex;
			uint32_t Unknown13;
			uint32_t Unknown14;
			float WidthRadius;
			float Depth;
			float Top;
			float Bottom;
			uint32_t Unknown15;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Weapon, 0xAC);

		struct WeaponPalette
		{
			TagReference Weapon;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(WeaponPalette, 0x30);

		struct DeviceGroup
		{
			char Name[32];
			float InitialValue;
			int32_t Flags;
			int16_t EditorFolderIndex;
			int16_t Unknown;
		};
		TAG_STRUCT_SIZE_ASSERT(DeviceGroup, 0x2C);

		struct Machine
		{
			struct NodePositioning;
			struct PathfindingReference;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			int16_t PowerGroup;
			int16_t PositionGroup;
			int32_t DeviceFlags;
			int32_t MachineFlags;
			TagBlock<PathfindingReference> PathfindingReferences;
			int16_t PathfindingPolicy;
			int16_t Unknown10;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);

			struct PathfindingReference
			{
				int16_t BspIndex;
				int16_t PathfindingObjectIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(PathfindingReference, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(Machine, 0x8C);

		struct MachinePalette
		{
			TagReference Machine;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(MachinePalette, 0x30);

		struct Terminal
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			int16_t PowerGroup;
			int16_t PositionGroup;
			int32_t DeviceFlags;
			uint32_t Unknown10;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Terminal, 0x7C);

		struct TerminalPalette
		{
			TagReference Terminal;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(TerminalPalette, 0x30);

		struct AlternateRealityDevice
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			int16_t PowerGroup;
			int16_t PositionGroup;
			int32_t DeviceFlags;
			char TapScriptName[32];
			char HoldScriptName[32];
			int16_t TapScriptIndex;
			int16_t HoldScriptIndex;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(AlternateRealityDevice, 0xBC);

		struct AlternateRealityDevicePalette
		{
			TagReference ArgDevice;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(AlternateRealityDevicePalette, 0x30);

		struct Control
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			int16_t PowerGroup;
			int16_t PositionGroup;
			int32_t DeviceFlags;
			int32_t ControlFlags;
			int16_t Unknown10;
			int16_t Unknown11;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Control, 0x80);

		struct ControlPalette
		{
			TagReference Control;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(ControlPalette, 0x30);

		struct SoundScenery
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			int32_t VolumeType;
			float Height;
			float OverrideDistanceMin;
			float OverrideDistanceMax;
			float OverrideConeAngleMin;
			float OverrideConeAngleMax;
			float OverrideOuterConeGain;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(SoundScenery, 0x70);

		struct SoundSceneryPalette
		{
			TagReference SoundScenery;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(SoundSceneryPalette, 0x30);

		struct Giant
		{
			struct NodePositioning;
			struct PathfindingReference;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			float BodyVitalityPercentage;
			int32_t Flags;
			int16_t Unknown10;
			int16_t Unknown11;
			TagBlock<PathfindingReference> PathfindingReferences;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);

			struct PathfindingReference
			{
				int16_t BspIndex;
				int16_t PathfindingObjectIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(PathfindingReference, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(Giant, 0x88);

		struct GiantPalette
		{
			TagReference Giant;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(GiantPalette, 0x30);

		struct EffectSceneryBlock
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(EffectSceneryBlock, 0x54);

		struct EffectSceneryBlock2
		{
			TagReference EffectScenery;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(EffectSceneryBlock2, 0x30);

		struct LightVolume
		{
			struct NodePositioning;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			int16_t PowerGroup;
			int16_t PositionGroup;
			int32_t DeviceFlags;
			int16_t Type2;
			uint16_t Flags;
			int16_t LightmapType;
			uint16_t LightmapFlags;
			float LightmapHalfLife;
			float LightmapLightScale;
			float X;
			float Y;
			float Z;
			float Width;
			float HeightScale;
			float FieldOfView;
			float FalloffDistance;
			float CutoffDistance;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				int16_t Unknown;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(LightVolume, 0x8C);

		struct LightVolumesPalette
		{
			TagReference LightVolume;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(LightVolumesPalette, 0x30);

		struct SandboxVehicle
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxVehicle, 0x30);

		struct SandboxWeapon
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxWeapon, 0x30);

		struct SandboxEquipment
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxEquipment, 0x30);

		struct SandboxScenery
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxScenery, 0x30);

		struct SandboxTeleporter
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxTeleporter, 0x30);

		struct SandboxGoalObject
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxGoalObject, 0x30);

		struct SandboxSpawning
		{
			TagReference Object;
			StringID Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxSpawning, 0x30);

		struct SoftCeiling
		{
			int16_t Type;
			int16_t Unknown;
			StringID Name;
			int16_t Unknown2;
			int16_t Unknown3;
		};
		TAG_STRUCT_SIZE_ASSERT(SoftCeiling, 0xC);

		struct PlayerStartingProfileBlock
		{
			char Name[32];
			float StartingHealthDamage;
			float StartingShieldDamage;
			TagReference PrimaryWeapon;
			int16_t RoundsLoaded;
			int16_t RoundsTotal;
			TagReference SecondaryWeapon;
			int16_t RoundsLoaded2;
			int16_t RoundsTotal2;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint8_t StartingFragGrenadeCount;
			uint8_t StartingPlasmaGrenadeCount;
			uint8_t StartingSpikeGrenadeCount;
			uint8_t StartingFirebombGrenadeCount;
			int16_t Unknown3;
			int16_t Unknown4;
		};
		TAG_STRUCT_SIZE_ASSERT(PlayerStartingProfileBlock, 0x60);

		struct PlayerStartingLocation
		{
			float PositionX;
			float PositionY;
			float PositionZ;
			float FacingY;
			float FacingP;
			int16_t Unknown;
			int16_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
		};
		TAG_STRUCT_SIZE_ASSERT(PlayerStartingLocation, 0x1C);

		struct TriggerVolume
		{
			struct Unknown11;
			struct Unknown13;

			StringID Name;
			int16_t ObjectName;
			int16_t Unknown;
			StringID NodeName;
			int16_t Unknown2;
			int16_t Unknown3;
			float Unknown4;
			float Unknown5;
			float Unknown6;
			float Unknown7;
			float Unknown8;
			float Unknown9;
			float PositionX;
			float PositionY;
			float PositionZ;
			float ExtentsX;
			float ExtentsY;
			float ExtentsZ;
			float Unknown10;
			TagBlock<Unknown11> Unknown12;
			TagBlock<Unknown13> Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			int16_t KillVolume;
			int16_t EditorFolderIndex;

			struct Unknown11
			{
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown11, 0x14);

			struct Unknown13Block
			{
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;
				uint32_t Unknown10;
				uint32_t Unknown11;
				uint32_t Unknown12;
				uint32_t Unknown13a;
				uint32_t Unknown14;
				uint32_t Unknown15;
				uint32_t Unknown16;
				uint32_t Unknown17;
				uint32_t Unknown18;
				uint32_t Unknown19;
				uint32_t Unknown20;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown13Block, 0x50);
		};
		TAG_STRUCT_SIZE_ASSERT(TriggerVolume, 0x7C);

		struct MultiplayerConstantsOverride
		{
			float Unknown;
			float Unknown2;
			float Unknown3;
			float Unknown4;
			float Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(MultiplayerConstantsOverride, 0x14);

		struct Unknown41
		{
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown41, 0x14);

		struct Decal
		{
			int16_t PaletteIndex;
			int8_t Yaw;
			int8_t Pitch;
			float I;
			float J;
			float K;
			float W;
			float X;
			float Y;
			float Z;
			float Scale;
		};
		TAG_STRUCT_SIZE_ASSERT(Decal, 0x24);

		struct DecalPalette
		{
			TagReference Decal;
		};
		TAG_STRUCT_SIZE_ASSERT(DecalPalette, 0x10);

		struct StylePalette
		{
			TagReference Style;
		};
		TAG_STRUCT_SIZE_ASSERT(StylePalette, 0x10);

		struct SquadGroup
		{
			char Name[32];
			int16_t ParentIndex;
			int16_t ObjectiveIndex;
			int16_t Unknown;
			int16_t EditorFolderIndex;
		};
		TAG_STRUCT_SIZE_ASSERT(SquadGroup, 0x28);

		struct Squad
		{
			struct GroupLocation;
			struct SingleLocation;
			struct SquadA;
			struct SquadB;

			char Name[32];
			int32_t Flags;
			int16_t Team;
			int16_t ParentSquadGroupIndex;
			int16_t ParentZone;
			int16_t ObjectiveIndex;
			int16_t ObjectiveRoleIndex;
			int16_t EditorFolderIndex;
			TagBlock<GroupLocation> GroupLocations;
			TagBlock<SingleLocation> SingleLocations;
			StringID SquadTemplateName;
			//"Short" tag reference
			int SquadTemplate;
			TagBlock<SquadA> SquadA2;
			TagBlock<SquadB> SquadB2;

			struct GroupLocation
			{
				struct MultiState;

				int16_t Unknown;
				int16_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				StringID Name;
				float PositionX;
				float PositionY;
				float PositionZ;
				int16_t ReferenceFrame;
				int16_t Unknown5;
				float FacingI;
				float FacingJ;
				float FacingK;
				StringID FormationType;
				float Unknown6;
				int16_t Unknown7;
				int16_t CommandScriptIndex;
				char CommandScriptName[32];
				StringID InitialState;
				int16_t Unknown8;
				int16_t Unknown9;
				TagBlock<MultiState> MultiState2;

				struct MultiState
				{
					int16_t Unknown;
					int16_t Unknown2;
					float Unknown3;
					float Unknown4;
					StringID State;
					uint32_t Unknown5;
					char CommandScriptName[32];
					int16_t CommandScriptIndex;
					int16_t Unknown6;
				};
				TAG_STRUCT_SIZE_ASSERT(MultiState, 0x38);
			};
			TAG_STRUCT_SIZE_ASSERT(GroupLocation, 0x6C);

			struct SingleLocation
			{
				struct MultiState;

				int16_t Unknown;
				int16_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				StringID Name;
				int16_t SquadMemberIndex;
				int16_t Unknown5;
				float PositionX;
				float PositionY;
				float PositionZ;
				int16_t ReferenceFrame;
				int16_t Unknown6;
				float FacingI;
				float FacingJ;
				float FacingK;
				uint16_t Flags;
				int16_t CharacterType;
				int16_t InitialPrimaryWeapon;
				int16_t InitialSecondaryWeapon;
				int16_t InitialEquipment;
				int16_t Vehicle;
				int16_t SeatType;
				int16_t InitialGrenades;
				uint32_t Unknown7;
				StringID ActorVariant;
				StringID VehicleVariant;
				float InitialMovementDistance;
				int16_t InitialMovementMode;
				int16_t EmitterVehicle;
				int16_t EmitterGiant;
				int16_t EmitterBiped;
				char CommandScriptName[32];
				int16_t CommandScriptIndex;
				int16_t Unknown8;
				StringID InitialState;
				int16_t Unknown9;
				int16_t Unknown10;
				TagBlock<MultiState> MultiState2;

				struct MultiState
				{
					int16_t Unknown;
					int16_t Unknown2;
					float Unknown3;
					float Unknown4;
					StringID State;
					uint32_t Unknown5;
					char CommandScriptName[32];
					int16_t CommandScriptIndex;
					int16_t Unknown6;
				};
				TAG_STRUCT_SIZE_ASSERT(MultiState, 0x38);
			};
			TAG_STRUCT_SIZE_ASSERT(SingleLocation, 0x90);

			struct SquadA
			{
				struct Actor;
				struct Weapon;
				struct SecondaryWeapon;
				struct Equipment;

				StringID Name;
				uint16_t Difficulty;
				int16_t Unknown;
				int16_t MinimumRound;
				int16_t MaximumRound;
				int16_t Unknown2;
				int16_t Unknown3;
				int16_t Count;
				int16_t Unknown4;
				TagBlock<Actor> Actors;
				TagBlock<Weapon> Weapons;
				TagBlock<SecondaryWeapon> SecondaryWeapons;
				TagBlock<Equipment> Equipment2;
				int16_t Unknown5;
				int16_t Vehicle;
				StringID VehicleVariant;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;
				uint32_t Unknown10;
				uint32_t Unknown11;
				uint32_t Unknown12;
				StringID CommandScriptName;
				int16_t CommandScriptIndex;
				int16_t Unknown13;
				uint32_t Unknown14;
				int16_t Unknown15;
				int16_t Unknown16;
				uint32_t Unknown17;
				uint32_t Unknown18;
				uint32_t Unknown19;

				struct Actor
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t Character;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(Actor, 0x10);

				struct Weapon
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t WeaponIndex;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(Weapon, 0x10);

				struct SecondaryWeapon
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t Weapon;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(SecondaryWeapon, 0x10);

				struct Equipment
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t EquipmentIndex;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(Equipment, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(SquadA, 0x84);

			struct SquadB
			{
				struct Actor;
				struct Weapon;
				struct SecondaryWeapon;
				struct Equipment;

				StringID Name;
				uint16_t Difficulty;
				int16_t Unknown;
				int16_t MinimumRound;
				int16_t MaximumRound;
				int16_t Unknown2;
				int16_t Unknown3;
				int16_t Count;
				int16_t Unknown4;
				TagBlock<Actor> Actors;
				TagBlock<Weapon> Weapons;
				TagBlock<SecondaryWeapon> SecondaryWeapons;
				TagBlock<Equipment> Equipment2;
				int16_t Unknown5;
				int16_t Vehicle;
				StringID VehicleVariant;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;
				uint32_t Unknown10;
				uint32_t Unknown11;
				uint32_t Unknown12;
				StringID CommandScriptName;
				int16_t CommandScriptIndex;
				int16_t Unknown13;
				uint32_t Unknown14;
				int16_t Unknown15;
				int16_t Unknown16;
				uint32_t Unknown17;
				uint32_t Unknown18;
				uint32_t Unknown19;

				struct Actor
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t Character;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(Actor, 0x10);

				struct Weapon
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t WeaponIndex;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(Weapon, 0x10);

				struct SecondaryWeapon
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t Weapon;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(SecondaryWeapon, 0x10);

				struct Equipment
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t MinimumRound;
					int16_t MaximumRound;
					uint32_t Unknown3;
					int16_t EquipmentIndex;
					int16_t Probability;
				};
				TAG_STRUCT_SIZE_ASSERT(Equipment, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(SquadB, 0x84);
		};
		TAG_STRUCT_SIZE_ASSERT(Squad, 0x68);

		struct Zone
		{
			struct FiringPosition;
			struct Area;

			char Name[32];
			int32_t Unknown;
			TagBlock<FiringPosition> FiringPositions;
			TagBlock<Area> Areas;

			struct FiringPosition
			{
				float PositionX;
				float PositionY;
				float PositionZ;
				int16_t ReferenceFrame;
				int16_t Unknown;
				uint16_t Flags;
				int16_t Unknown2;
				int16_t AreaIndex;
				int16_t ClusterIndex;
				int16_t Unknown3;
				int16_t Unknown4;
				float NormalY;
				float NormalP;
				uint32_t Unknown5;
			};
			TAG_STRUCT_SIZE_ASSERT(FiringPosition, 0x28);

			struct Area
			{
				struct FlightHint;
				struct Unknown13;

				char Name[32];
				int32_t AreaFlags;
				float PositionX;
				float PositionY;
				float PositionZ;
				int32_t Unknown;
				float Unknown2;
				int16_t FiringPositionStartIndex;
				int16_t FiringPositionCount;
				int16_t Unknown3;
				int16_t Unknown4;
				int32_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;
				uint32_t Unknown10;
				uint32_t Unknown11;
				int16_t ManualReferenceFrame;
				int16_t Unknown12;
				TagBlock<FlightHint> FlightHints;
				TagBlock<Unknown13> Unknown14;
				uint32_t Unknown15;
				uint32_t Unknown16;
				uint32_t Unknown17;
				uint32_t Unknown18;
				uint32_t Unknown19;
				uint32_t Unknown20;
				uint32_t Unknown21;
				uint32_t Unknown22;
				uint32_t Unknown23;
				uint32_t Unknown24;
				uint32_t Unknown25;
				uint32_t Unknown26;

				struct FlightHint
				{
					int16_t FlightHintIndex;
					int16_t PoitIndex;
					uint32_t Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(FlightHint, 0x8);

				struct Unknown13
				{
					float PositionX;
					float PositionY;
					float PositionZ;
					int16_t Unknown;
					int16_t Unknown2;
					float FacingY;
					float FacingP;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown13, 0x18);
			};
			TAG_STRUCT_SIZE_ASSERT(Area, 0xA8);
		};
		TAG_STRUCT_SIZE_ASSERT(Zone, 0x3C);

		struct Unknown82
		{
			struct Unknown2;
			struct Unknown4;
			struct Unknown6;

			StringID Unknown;
			TagBlock<Unknown2> Unknown3;
			TagBlock<Unknown4> Unknown5;
			TagBlock<Unknown6> Unknown7;
			uint32_t Unknown8;

			struct Unknown2
			{
				uint32_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown2, 0x4);

			struct Unknown4
			{
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4a;
				uint32_t Unknown5;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown4, 0x14);

			struct Unknown6
			{
				struct Unknown2;

				uint32_t Unknown;
				TagBlock<Unknown2> Unknown3;

				struct Unknown2
				{
					uint32_t Unknown;
					uint32_t Unknown2a;
					uint32_t Unknown3;
					uint32_t Unknown4;
					uint32_t Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown2, 0x14);
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown6, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown82, 0x2C);

		struct CharacterPalette
		{
			TagReference Character;
		};
		TAG_STRUCT_SIZE_ASSERT(CharacterPalette, 0x10);

		struct AiPathfindingDatum
		{
			struct Unknown22;
			struct Unknown24;

			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			TagBlock<Unknown22> Unknown23;
			TagBlock<Unknown24> Unknown25;

			struct Unknown22
			{
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown22, 0x18);

			struct Unknown24
			{
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown24, 0xC);
		};
		TAG_STRUCT_SIZE_ASSERT(AiPathfindingDatum, 0x6C);

		struct Script
		{
			struct Parameter;

			char ScriptName[32];
			int16_t ScriptType;
			int16_t ReturnType;
			uint16_t RootExpressionSalt;
			uint16_t RootExpressionIndex;
			TagBlock<Parameter> Parameters;

			struct Parameter
			{
				char Name[32];
				int16_t Type;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(Parameter, 0x24);
		};
		TAG_STRUCT_SIZE_ASSERT(Script, 0x34);

		struct Global
		{
			char Name[32];
			int16_t Type;
			int16_t Unknown;
			uint16_t InitializationExpressionSalt;
			uint16_t InitializationExpressionIndex;
		};
		TAG_STRUCT_SIZE_ASSERT(Global, 0x28);

		struct ScriptReference
		{
			TagReference Reference;
		};
		TAG_STRUCT_SIZE_ASSERT(ScriptReference, 0x10);

		struct ScriptingDatum
		{
			struct PointSet;

			TagBlock<PointSet> PointSets;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			uint32_t Unknown22;
			uint32_t Unknown23;
			uint32_t Unknown24;
			uint32_t Unknown25;
			uint32_t Unknown26;
			uint32_t Unknown27;
			uint32_t Unknown28;
			uint32_t Unknown29;
			uint32_t Unknown30;

			struct PointSet
			{
				struct Point;

				char Name[32];
				TagBlock<Point> Points;
				int16_t BspIndex;
				int16_t ManualReferenceFrame;
				int32_t Flags;
				int16_t EditorFolderIndex;
				int16_t Unknown;

				struct Point
				{
					char Name[32];
					float PositionX;
					float PositionY;
					float PositionZ;
					int16_t ReferenceFrame;
					int16_t Unknown;
					int32_t SurfaceIndex;
					float FacingDirectionY;
					float FacingDirectionP;
				};
				TAG_STRUCT_SIZE_ASSERT(Point, 0x3C);
			};
			TAG_STRUCT_SIZE_ASSERT(PointSet, 0x38);
		};
		TAG_STRUCT_SIZE_ASSERT(ScriptingDatum, 0x84);

		struct CutsceneFlag
		{
			uint32_t Unknown;
			StringID Name;
			float PositionX;
			float PositionY;
			float PositionZ;
			float FacingY;
			float FacingP;
			int16_t EditorFolderIndex;
			int16_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(CutsceneFlag, 0x20);

		struct CutsceneCameraPoint
		{
			uint16_t Flags;
			int16_t Type;
			char Name[32];
			int32_t Unknown;
			float PositionX;
			float PositionY;
			float PositionZ;
			float OrientationY;
			float OrientationP;
			float OrientationR;
		};
		TAG_STRUCT_SIZE_ASSERT(CutsceneCameraPoint, 0x40);

		struct CutsceneTitle
		{
			StringID Name;
			int16_t TextBoundsTop;
			int16_t TextBoundsLeft;
			int16_t TextBoundsBottom;
			int16_t TextBoundsRight;
			int16_t HorizontalJustification;
			int16_t VerticalJustification;
			int16_t Font;
			int16_t Unknown;
			uint8_t TextColorA;
			uint8_t TextColorR;
			uint8_t TextColorG;
			uint8_t TextColorB;
			uint8_t ShadowColorA;
			uint8_t ShadowColorR;
			uint8_t ShadowColorG;
			uint8_t ShadowColorB;
			float FadeInTime;
			float Uptime;
			float FadeOutTime;
		};
		TAG_STRUCT_SIZE_ASSERT(CutsceneTitle, 0x28);

		struct ScenarioResource
		{
			struct ScriptSource;
			struct AiResource;
			struct Reference;

			int32_t Unknown;
			TagBlock<ScriptSource> ScriptSource2;
			TagBlock<AiResource> AiResources;
			TagBlock<Reference> References;

			struct ScriptSource
			{
				TagReference HsSourceFile;
			};
			TAG_STRUCT_SIZE_ASSERT(ScriptSource, 0x10);

			struct AiResource
			{
				TagReference Tag;
			};
			TAG_STRUCT_SIZE_ASSERT(AiResource, 0x10);

			struct Reference
			{
				struct OtherScenery;
				struct OtherBiped;
				struct OtherDevice;
				struct OtherDecal;
				struct OtherDecorator;

				TagReference SceneryResource;
				TagBlock<OtherScenery> OtherScenery2;
				TagReference BipedsResource;
				TagBlock<OtherBiped> OtherBipeds;
				TagReference VehiclesResource;
				TagReference EquipmentResource;
				TagReference WeaponsResource;
				TagReference SoundSceneryResource;
				TagReference LightsResource;
				TagReference DevicesResource;
				TagBlock<OtherDevice> OtherDevices;
				TagReference EffectSceneryResource;
				TagReference DecalsResource;
				TagBlock<OtherDecal> OtherDecals;
				TagReference CinematicsResource;
				TagReference TriggerVolumesResource;
				TagReference ClusterDataResource;
				TagReference CommentsResource;
				TagReference CreatureResource;
				TagReference StructureLightingResource;
				TagReference DecoratorsResource;
				TagBlock<OtherDecorator> OtherDecorators;
				TagReference SkyReferencesResource;
				TagReference CubemapResource;

				struct OtherScenery
				{
					TagReference SceneryResource;
				};
				TAG_STRUCT_SIZE_ASSERT(OtherScenery, 0x10);

				struct OtherBiped
				{
					TagReference BipedsResource;
				};
				TAG_STRUCT_SIZE_ASSERT(OtherBiped, 0x10);

				struct OtherDevice
				{
					TagReference DevicesResource;
				};
				TAG_STRUCT_SIZE_ASSERT(OtherDevice, 0x10);

				struct OtherDecal
				{
					TagReference DecalsResource;
				};
				TAG_STRUCT_SIZE_ASSERT(OtherDecal, 0x10);

				struct OtherDecorator
				{
					TagReference DecoratorsResource;
				};
				TAG_STRUCT_SIZE_ASSERT(OtherDecorator, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(Reference, 0x16C);
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioResource, 0x28);

		struct UnitSeatsMapping
		{
			//"Short" tag reference
			int UnitTag;
			int32_t Seats;
			int32_t Seats2;
		};
		TAG_STRUCT_SIZE_ASSERT(UnitSeatsMapping, 0xC);

		struct ScenarioKillTrigger
		{
			int16_t TriggerVolume;
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioKillTrigger, 0x2);

		struct ScenarioSafeTrigger
		{
			int16_t TriggerVolume;
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioSafeTrigger, 0x2);

		struct ScriptExpression
		{
			uint16_t Salt;
			uint16_t Opcode;
			int16_t ValueType;
			int16_t ExpressionType;
			uint16_t NextExpressionSalt;
			uint16_t NextExpressionIndex;
			uint32_t StringAddress;
			int8_t Value03Msb;
			int8_t Value02Byte;
			int8_t Value01Byte;
			int8_t Value00Lsb;
			int16_t LineNumber;
			int16_t Unknown;
		};
		TAG_STRUCT_SIZE_ASSERT(ScriptExpression, 0x18);

		struct BackgroundSoundEnvironmentPalette
		{
			StringID Name;
			TagReference SoundEnvironment;
			int32_t Unknown;
			float CutoffDistance;
			float InterpolationSpeed;
			TagReference BackgroundSound;
			TagReference InsideClusterSound;
			float CutoffDistance2;
			int32_t ScaleFlags;
			float InteriorScale;
			float PortalScale;
			float ExteriorScale;
			float InterpolationSpeed2;
		};
		TAG_STRUCT_SIZE_ASSERT(BackgroundSoundEnvironmentPalette, 0x58);

		struct Unknown108
		{
			char Name[32];
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			uint32_t Unknown22;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown108, 0x78);

		struct Fog
		{
			StringID Name;
			int16_t Unknown;
			int16_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(Fog, 0x8);

		struct CameraFxBlock
		{
			StringID Name;
			TagReference Tag;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
		};
		TAG_STRUCT_SIZE_ASSERT(CameraFxBlock, 0x30);

		struct ScenarioClusterDatum
		{
			struct BackgroundSoundEnvironment;
			struct Unknown;
			struct Unknown3;
			struct ClusterCentroid;
			struct Unknown5;
			struct Fog;
			struct CameraEffect;

			TagReference Bsp;
			TagBlock<BackgroundSoundEnvironment> BackgroundSoundEnvironments;
			TagBlock<Unknown> Unknown2;
			TagBlock<Unknown3> Unknown4;
			int32_t BspChecksum;
			TagBlock<ClusterCentroid> ClusterCentroids;
			TagBlock<Unknown5> Unknown6;
			TagBlock<Fog> Fog2;
			TagBlock<CameraEffect> CameraEffects;

			struct BackgroundSoundEnvironment
			{
				int16_t BackgroundSoundEnvironmentIndex;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(BackgroundSoundEnvironment, 0x4);

			struct Unknown
			{
				int16_t Unknown1;
				int16_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown, 0x4);

			struct Unknown3
			{
				int16_t Unknown;
				int16_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown3, 0x4);

			struct ClusterCentroid
			{
				float CentroidX;
				float CentroidY;
				float CentroidZ;
			};
			TAG_STRUCT_SIZE_ASSERT(ClusterCentroid, 0xC);

			struct Unknown5
			{
				int16_t Unknown;
				int16_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown5, 0x4);

			struct Fog
			{
				int16_t FogIndex;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(Fog, 0x4);

			struct CameraEffect
			{
				int16_t CameraEffectIndex;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(CameraEffect, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioClusterDatum, 0x68);

		struct SpawnDatum
		{
			struct DynamicSpawnOverload;
			struct StaticRespawnZone;
			struct StaticInitialSpawnZone;

			float DynamicSpawnLowerHeight;
			float DynamicSpawnUpperHeight;
			float GameObjectResetHeight;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			TagBlock<DynamicSpawnOverload> DynamicSpawnOverloads;
			TagBlock<StaticRespawnZone> StaticRespawnZones;
			TagBlock<StaticInitialSpawnZone> StaticInitialSpawnZones;

			struct DynamicSpawnOverload
			{
				int16_t OverloadType;
				int16_t Unknown;
				float InnerRadius;
				float OuterRadius;
				float Weight;
			};
			TAG_STRUCT_SIZE_ASSERT(DynamicSpawnOverload, 0x10);

			struct StaticRespawnZone
			{
				StringID Name;
				int32_t RelevantTeams;
				int32_t RelevantGames;
				int32_t Flags;
				float PositionX;
				float PositionY;
				float PositionZ;
				float LowerHeight;
				float UpperHeight;
				float InnerRadius;
				float OuterRadius;
				float Weight;
			};
			TAG_STRUCT_SIZE_ASSERT(StaticRespawnZone, 0x30);

			struct StaticInitialSpawnZone
			{
				StringID Name;
				int32_t RelevantTeams;
				int32_t RelevantGames;
				int32_t Flags;
				float PositionX;
				float PositionY;
				float PositionZ;
				float LowerHeight;
				float UpperHeight;
				float InnerRadius;
				float OuterRadius;
				float Weight;
			};
			TAG_STRUCT_SIZE_ASSERT(StaticInitialSpawnZone, 0x30);
		};
		TAG_STRUCT_SIZE_ASSERT(SpawnDatum, 0x6C);

		struct Crate
		{
			struct NodePositioning;
			struct Unknown11;

			int16_t PaletteIndex;
			int16_t NameIndex;
			int32_t PlacementFlags;
			float PositionX;
			float PositionY;
			float PositionZ;
			float RotationI;
			float RotationJ;
			float RotationK;
			float Scale;
			TagBlock<NodePositioning> NodePositioning2;
			int16_t Unknown;
			uint16_t OldManualBspFlagsNowZonesets;
			StringID UniqueName;
			uint16_t UniqueIdIndex;
			uint16_t UniqueIdSalt;
			int16_t OriginBspIndex;
			int8_t Type;
			int8_t Source;
			int8_t BspPolicy;
			int8_t Unknown2;
			int16_t EditorFolderIndex;
			int16_t Unknown3;
			int16_t ParentNameIndex;
			StringID ChildName;
			StringID Unknown4;
			uint16_t AllowedZonesets;
			int16_t Unknown5;
			StringID Variant;
			uint8_t ActiveChangeColors;
			int8_t Unknown6;
			int8_t Unknown7;
			int8_t Unknown8;
			uint8_t PrimaryColorA;
			uint8_t PrimaryColorR;
			uint8_t PrimaryColorG;
			uint8_t PrimaryColorB;
			uint8_t SecondaryColorA;
			uint8_t SecondaryColorR;
			uint8_t SecondaryColorG;
			uint8_t SecondaryColorB;
			uint8_t TertiaryColorA;
			uint8_t TertiaryColorR;
			uint8_t TertiaryColorG;
			uint8_t TertiaryColorB;
			uint8_t QuaternaryColorA;
			uint8_t QuaternaryColorR;
			uint8_t QuaternaryColorG;
			uint8_t QuaternaryColorB;
			uint32_t Unknown9;
			uint32_t Unknown10;
			TagBlock<Unknown11> Unknown12;
			int32_t Symmetry;
			uint16_t EngineFlags;
			int16_t Team;
			int8_t SpawnSequence;
			int8_t RuntimeMinimum;
			int8_t RuntimeMaximum;
			uint8_t MultiplayerFlags;
			int16_t SpawnTime;
			int16_t AbandonTime;
			int8_t Unknown13;
			int8_t Shape;
			int8_t TeleporterChannel;
			int8_t Unknown14;
			int16_t Unknown15;
			int16_t AttachedNameIndex;
			uint32_t Unknown16;
			uint32_t Unknown17;
			float WidthRadius;
			float Depth;
			float Top;
			float Bottom;
			uint32_t Unknown18;

			struct NodePositioning
			{
				struct NodeFlag;
				struct Orientation;
				struct NodeFlagsReadable;
				struct OrientationsReadable;

				int16_t NodeCount;
				PAD16;
				TagBlock<NodeFlag> NodeFlags;
				TagBlock<Orientation> Orientations;

				struct NodeFlag
				{
					uint8_t Flags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeFlag, 0x1);

				struct Orientation
				{
					int16_t RotationXYZW;
				};
				TAG_STRUCT_SIZE_ASSERT(Orientation, 0x2);
			};
			TAG_STRUCT_SIZE_ASSERT(NodePositioning, 0x1C);

			struct Unknown11
			{
				uint32_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown11, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(Crate, 0xB4);

		struct CratePalette
		{
			TagReference Crate;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(CratePalette, 0x30);

		struct FlockPalette
		{
			TagReference Flock;
		};
		TAG_STRUCT_SIZE_ASSERT(FlockPalette, 0x10);

		struct Flock
		{
			struct Source;
			struct Sink;

			StringID Name;
			int16_t FlockPaletteIndex;
			int16_t BspIndex;
			int16_t BoundingTriggerVolume;
			uint16_t Flags;
			float EcologyMargin;
			TagBlock<Source> Sources;
			TagBlock<Sink> Sinks;
			float ProductionFrequencyMin;
			float ProductionFrequencyMax;
			float ScaleMin;
			float ScaleMax;
			float Unknown;
			uint32_t Unknown2;
			int16_t CreaturePaletteIndex;
			int16_t BoidCountMin;
			int16_t BoidCountMax;
			int16_t Unknown3;

			struct Source
			{
				int32_t Unknown;
				float PositionX;
				float PositionY;
				float PositionZ;
				float StartingY;
				float StartingP;
				float Radius;
				float Weight;
				int8_t Unknown2;
				int8_t Unknown3;
				int8_t Unknown4;
				int8_t Unknown5;
			};
			TAG_STRUCT_SIZE_ASSERT(Source, 0x24);

			struct Sink
			{
				float PositionX;
				float PositionY;
				float PositionZ;
				float Radius;
			};
			TAG_STRUCT_SIZE_ASSERT(Sink, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(Flock, 0x48);

		struct CreaturePalette
		{
			TagReference Creature;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(CreaturePalette, 0x30);

		struct EditorFolder
		{
			int32_t ParentFolder;
			char Name[256];
		};
		TAG_STRUCT_SIZE_ASSERT(EditorFolder, 0x104);

		struct MissionDialogue
		{
			TagReference Tag;
		};
		TAG_STRUCT_SIZE_ASSERT(MissionDialogue, 0x10);

		struct Interpolator
		{
			StringID Name;
			StringID AcceleratorName;
			StringID MultiplierName;
			TagData<uint8_t> Function;
			int16_t Unknown;
			int16_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(Interpolator, 0x24);

		struct SimulationDefinitionTable
		{
			//"Short" tag reference
			int Tag;
		};
		TAG_STRUCT_SIZE_ASSERT(SimulationDefinitionTable, 0x4);

		struct Unknown134
		{
			int16_t Unknown;
			int16_t Unknown2;
			int16_t Unknown3;
			int16_t Unknown4;
			int16_t Unknown5;
			int16_t Unknown6;
			int16_t Unknown7;
			int16_t Unknown8;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown134, 0x10);

		struct AiObjective
		{
			struct Role;

			StringID Name;
			int16_t Zone;
			int16_t Unknown;
			TagBlock<Role> Roles;

			struct Role
			{
				struct Unknown84;
				struct Condition;
				struct Unknown15;
				struct PointGeometry;

				int16_t Unknown;
				int16_t Unknown2;
				int16_t Unknown3;
				int16_t Unknown4;
				int16_t Unknown5;
				int16_t Unknown6;
				uint32_t Unknown7;
				char CommandScriptName1[32];
				char CommandScriptName2[32];
				char CommandScriptName3[32];
				int16_t CommandScriptIndex1;
				int16_t CommandScriptIndex2;
				int16_t CommandScriptIndex3;
				int16_t Unknown8;
				int16_t Unknown9;
				int16_t Unknown10;
				TagBlock<Unknown84> Unknown84_2;
				StringID Task;
				int16_t HierarchyLevelFrom100;
				int16_t PreviousRole;
				int16_t NextRole;
				int16_t ParentRole;
				TagBlock<Condition> Conditions;
				int16_t ScriptIndex;
				int16_t Unknown11;
				int16_t Unknown12;
				int16_t Filter;
				int16_t Min;
				int16_t Max;
				int16_t Bodies;
				int16_t Unknown13;
				uint32_t Unknown14;
				TagBlock<Unknown15> Unknown16;
				TagBlock<PointGeometry> PointGeometry2;

				struct Unknown84
				{
					uint32_t Unknown;
					uint32_t Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown84, 0x8);

				struct Condition
				{
					char Name[32];
					char ConditionName[256];
					int16_t Unknown;
					int16_t Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(Condition, 0x124);

				struct Unknown15
				{
					int16_t Unknown;
					int16_t Unknown2;
					int16_t Unknown3;
					int16_t Unknown4;
					int16_t Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown15, 0xA);

				struct PointGeometry
				{
					float Point0X;
					float Point0Y;
					float Point0Z;
					int16_t ReferenceFrame;
					int16_t Unknown;
					float Point1X;
					float Point1Y;
					float Point1Z;
					int16_t ReferenceFrame2;
					int16_t Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(PointGeometry, 0x20);
			};
			TAG_STRUCT_SIZE_ASSERT(Role, 0xCC);
		};
		TAG_STRUCT_SIZE_ASSERT(AiObjective, 0x14);

		struct DesignerZoneset
		{
			struct Biped;
			struct Vehicle;
			struct Weapon;
			struct Equipment;
			struct Scenery;
			struct Machine;
			struct Terminal;
			struct Control;
			struct Unknown2;
			struct Crate;
			struct Creature;
			struct Giant;
			struct Unknown4;
			struct Character;

			StringID Name;
			uint32_t Unknown;
			TagBlock<Biped> Bipeds;
			TagBlock<Vehicle> Vehicles;
			TagBlock<Weapon> Weapons;
			TagBlock<Equipment> Equipment2;
			TagBlock<Scenery> Scenery2;
			TagBlock<Machine> Machines;
			TagBlock<Terminal> Terminals;
			TagBlock<Control> Controls;
			TagBlock<Unknown2> Unknown3;
			TagBlock<Crate> Crates;
			TagBlock<Creature> Creatures;
			TagBlock<Giant> Giants;
			TagBlock<Unknown4> Unknown5;
			TagBlock<Character> Characters;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;

			struct Biped
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Biped, 0x2);

			struct Vehicle
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Vehicle, 0x2);

			struct Weapon
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Weapon, 0x2);

			struct Equipment
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Equipment, 0x2);

			struct Scenery
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Scenery, 0x2);

			struct Machine
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Machine, 0x2);

			struct Terminal
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Terminal, 0x2);

			struct Control
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Control, 0x2);

			struct Unknown2
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown2, 0x2);

			struct Crate
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Crate, 0x2);

			struct Creature
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Creature, 0x2);

			struct Giant
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Giant, 0x2);

			struct Unknown4
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown4, 0x2);

			struct Character
			{
				int16_t PaletteIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(Character, 0x2);
		};
		TAG_STRUCT_SIZE_ASSERT(DesignerZoneset, 0xBC);

		struct Unknown136
		{
			int16_t Unknown;
			int16_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown136, 0x4);

		struct Cinematic
		{
			TagReference Tag;
		};
		TAG_STRUCT_SIZE_ASSERT(Cinematic, 0x10);

		struct CinematicLighting
		{
			StringID Name;
			TagReference CinematicLight;
		};
		TAG_STRUCT_SIZE_ASSERT(CinematicLighting, 0x14);

		struct ScenarioMetagame
		{
			struct TimeMultiplier;
			struct Survival;

			TagBlock<TimeMultiplier> TimeMultipliers;
			float ParScore;
			TagBlock<Survival> Survival2;

			struct TimeMultiplier
			{
				float Time;
				float Multiplier;
			};
			TAG_STRUCT_SIZE_ASSERT(TimeMultiplier, 0x8);

			struct Survival
			{
				int16_t InsertionIndex;
				int16_t Unknown;
				float ParScore;
			};
			TAG_STRUCT_SIZE_ASSERT(Survival, 0x8);
		};
		TAG_STRUCT_SIZE_ASSERT(ScenarioMetagame, 0x1C);

		struct Unknown144
		{
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown144, 0x18);

		struct Unknown146
		{
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			int16_t Unknown4;
			int16_t Unknown5;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown146, 0x10);

		struct Unknown159
		{
			TagReference Unknown;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown159, 0x10);
	};
	TAG_STRUCT_SIZE_ASSERT(Scenario, 0x824);
}
