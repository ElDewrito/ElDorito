#pragma once
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct Globals : Tag<'matg'>
		{
			struct HavokCleanupResource;
			struct SoundGlobal;
			struct AiGlobal;
			struct DamageTable;
			struct Sound;
			struct Camera;
			struct PlayerControl;
			struct Difficulty;
			struct Grenade;
			struct InterfaceTag;
			struct PlayerInformation;
			struct PlayerRepresentation;
			struct PlayerFallingDamage;
			struct Unknown59;
			struct Material;
			struct CinematicAnchor;
			struct MetagameGlobal;
			struct DamageReportingType;

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
			uint32_t Unknown31;
			uint32_t Unknown32;
			uint32_t Unknown33;
			uint32_t Unknown34;
			uint32_t Unknown35;
			uint32_t Unknown36;
			uint32_t Unknown37;
			uint32_t Unknown38;
			uint32_t Unknown39;
			uint32_t Unknown40;
			uint32_t Unknown41;
			uint32_t Unknown42;
			uint32_t Unknown43;
			int32_t Language;
			TagBlock<HavokCleanupResource> HavokCleanupResources;
			TagBlock<SoundGlobal> SoundGlobals;
			TagBlock<AiGlobal> AiGlobals;
			TagReference AiGlobals2;
			TagBlock<DamageTable> DamageTable2;
			uint32_t Unknown44;
			uint32_t Unknown45;
			uint32_t Unknown46;
			TagBlock<Sound> Sounds;
			TagBlock<Camera> Camera2;
			TagBlock<PlayerControl> PlayerControl2;
			TagBlock<Difficulty> Difficulty2;
			TagBlock<Grenade> Grenades;
			uint32_t Unknown47;
			uint32_t Unknown48;
			uint32_t Unknown49;
			TagBlock<InterfaceTag> InterfaceTags;
			uint32_t Unknown50;
			uint32_t Unknown51;
			uint32_t Unknown52;
			uint32_t Unknown53;
			uint32_t Unknown54;
			uint32_t Unknown55;
			TagBlock<PlayerInformation> PlayerInformation2;
			TagBlock<PlayerRepresentation> PlayerRepresentation2;
			uint32_t Unknown56;
			uint32_t Unknown57;
			uint32_t Unknown58;
			TagBlock<PlayerFallingDamage> FallingDamage2;
			TagBlock<Unknown59> Unknown60;
			TagBlock<Material> Materials;
			TagReference MultiplayerGlobals;
			TagReference SurvivalGlobals;
			TagBlock<CinematicAnchor> CinematicAnchors;
			TagBlock<MetagameGlobal> MetagameGlobals;
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
			uint32_t Unknown79;
			uint32_t Unknown80;
			uint32_t Unknown81;
			uint32_t Unknown82;
			uint32_t Unknown83;
			uint32_t Unknown84;
			uint32_t Unknown85;
			uint32_t Unknown86;
			uint32_t Unknown87;
			uint32_t Unknown88;
			uint32_t Unknown89;
			uint32_t Unknown90;
			uint32_t Unknown91;
			uint32_t Unknown92;
			uint32_t Unknown93;
			uint32_t Unknown94;
			uint32_t Unknown95;
			uint32_t Unknown96;
			uint32_t Unknown97;
			uint32_t Unknown98;
			uint32_t Unknown99;
			uint32_t Unknown100;
			uint32_t Unknown101;
			uint32_t Unknown102;
			uint32_t Unknown103;
			uint32_t Unknown104;
			uint32_t Unknown105;
			uint32_t Unknown106;
			uint32_t Unknown107;
			uint32_t Unknown108;
			uint32_t Unknown109;
			uint32_t Unknown110;
			uint32_t Unknown111;
			uint32_t Unknown112;
			uint32_t Unknown113;
			uint32_t Unknown114;
			uint32_t Unknown115;
			uint32_t Unknown116;
			uint32_t Unknown117;
			uint32_t Unknown118;
			uint32_t Unknown119;
			uint32_t Unknown120;
			uint32_t Unknown121;
			uint32_t Unknown122;
			uint32_t Unknown123;
			uint32_t Unknown124;
			uint32_t Unknown125;
			uint32_t Unknown126;
			uint32_t Unknown127;
			uint32_t Unknown128;
			uint32_t Unknown129;
			uint32_t Unknown130;
			uint32_t Unknown131;
			uint32_t Unknown132;
			uint32_t Unknown133;
			uint32_t Unknown134;
			uint32_t Unknown135;
			uint32_t Unknown136;
			uint32_t Unknown137;
			uint32_t Unknown138;
			uint32_t Unknown139;
			uint32_t Unknown140;
			uint32_t Unknown141;
			uint32_t Unknown142;
			uint32_t Unknown143;
			uint32_t Unknown144;
			uint32_t Unknown145;
			uint32_t Unknown146;
			uint32_t Unknown147;
			uint32_t Unknown148;
			uint32_t Unknown149;
			uint32_t Unknown150;
			uint32_t Unknown151;
			uint32_t Unknown152;
			uint32_t Unknown153;
			uint32_t Unknown154;
			uint32_t Unknown155;
			uint32_t Unknown156;
			uint32_t Unknown157;
			uint32_t Unknown158;
			uint32_t Unknown159;
			uint32_t Unknown160;
			uint32_t Unknown161;
			uint32_t Unknown162;
			uint32_t Unknown163;
			uint32_t Unknown164;
			uint32_t Unknown165;
			uint32_t Unknown166;
			uint32_t Unknown167;
			uint32_t Unknown168;
			uint32_t Unknown169;
			uint32_t Unknown170;
			uint32_t Unknown171;
			uint32_t Unknown172;
			uint32_t Unknown173;
			uint32_t Unknown174;
			uint32_t Unknown175;
			uint32_t Unknown176;
			uint32_t Unknown177;
			uint32_t Unknown178;
			uint32_t Unknown179;
			uint32_t Unknown180;
			uint32_t Unknown181;
			uint32_t Unknown182;
			uint32_t Unknown183;
			uint32_t Unknown184;
			uint32_t Unknown185;
			uint32_t Unknown186;
			uint32_t Unknown187;
			uint32_t Unknown188;
			uint32_t Unknown189;
			uint32_t Unknown190;
			uint32_t Unknown191;
			uint32_t Unknown192;
			uint32_t Unknown193;
			uint32_t Unknown194;
			uint32_t Unknown195;
			uint32_t Unknown196;
			uint32_t Unknown197;
			uint32_t Unknown198;
			uint32_t Unknown199;
			uint32_t Unknown200;
			uint32_t Unknown201;
			uint32_t Unknown202;
			uint32_t Unknown203;
			uint32_t Unknown204;
			uint32_t Unknown205;
			uint32_t Unknown206;
			uint32_t Unknown207;
			uint32_t Unknown208;
			uint32_t Unknown209;
			uint32_t Unknown210;
			uint32_t Unknown211;
			uint32_t Unknown212;
			uint32_t Unknown213;
			uint32_t Unknown214;
			uint32_t Unknown215;
			uint32_t Unknown216;
			uint32_t Unknown217;
			uint32_t Unknown218;
			uint32_t Unknown219;
			uint32_t Unknown220;
			uint32_t Unknown221;
			uint32_t Unknown222;
			uint32_t Unknown223;
			uint32_t Unknown224;
			uint32_t Unknown225;
			uint32_t Unknown226;
			uint32_t Unknown227;
			uint32_t Unknown228;
			uint32_t Unknown229;
			uint32_t Unknown230;
			uint32_t Unknown231;
			uint32_t Unknown232;
			uint32_t Unknown233;
			uint32_t Unknown234;
			uint32_t Unknown235;
			uint32_t Unknown236;
			uint32_t Unknown237;
			uint32_t Unknown238;
			uint32_t Unknown239;
			uint32_t Unknown240;
			uint32_t Unknown241;
			uint32_t Unknown242;
			uint32_t Unknown243;
			uint32_t Unknown244;
			uint32_t Unknown245;
			uint32_t Unknown246;
			uint32_t Unknown247;
			uint32_t Unknown248;
			uint32_t Unknown249;
			uint32_t Unknown250;
			uint32_t Unknown251;
			uint32_t Unknown252;
			uint32_t Unknown253;
			uint32_t Unknown254;
			uint32_t Unknown255;
			uint32_t Unknown256;
			uint32_t Unknown257;
			uint32_t Unknown258;
			uint32_t Unknown259;
			uint32_t Unknown260;
			uint32_t Unknown261;
			uint32_t Unknown262;
			uint32_t Unknown263;
			uint32_t Unknown264;
			TagReference RasterizerGlobals;
			TagReference DefaultCameraEffect;
			TagReference PodiumDefinition;
			TagReference DefaultWind;
			TagReference DefaultDamageEffect;
			TagReference DefaultCollisionDamage;
			int32_t UnknownMaterial;
			int16_t UnknownGlobalMaterialIndex;
			int16_t Unknown265;
			TagReference EffectGlobals;
			TagReference GameProgressionGlobals;
			TagReference AchievementGlobals;
			TagReference InputGlobals;
			uint32_t Unknown266;
			uint32_t Unknown267;
			uint32_t Unknown268;
			uint32_t Unknown269;
			DataReference<uint8_t> Unknown270;
			uint32_t Unknown271;
			uint32_t Unknown272;
			uint32_t Unknown273;
			uint32_t Unknown274;
			uint32_t Unknown275;
			TagBlock<DamageReportingType> DamageReportingTypes;
			uint32_t Unknown276;

			struct HavokCleanupResource
			{
				TagReference ObjectCleanupEffect;
			};
			TAG_STRUCT_SIZE_ASSERT(HavokCleanupResource, 0x10);

			struct SoundGlobal
			{
				TagReference SoundClasses;
				TagReference SoundEffects;
				TagReference SoundMix;
				TagReference SoundCombatDialogueConstants;
				TagReference SoundGlobalPropagation;
				TagReference GfxUiSounds;
			};
			TAG_STRUCT_SIZE_ASSERT(SoundGlobal, 0x60);

			struct AiGlobal
			{
				struct GravemindProperty;
				struct AiStyle;

				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
				float DangerBroadlyFacing;
				float DangerShootingNear;
				float DangerShootingAt;
				float DangerExtremelyClose;
				float DangerShieldDamage;
				float DangerExtendedShieldDamage;
				float DangerBodyDamage;
				float DangerExtendedBodyDamage;
				TagReference GlobalDialogue;
				float DefaultMissionDialogueSoundEffect;
				float JumpDown;
				float JumpStep;
				float JumpCrouch;
				float JumpStand;
				float JumpStorey;
				float JumpTower;
				float MaxJumpDownHeightDown;
				float MaxJumpDownHeightStep;
				float MaxJumpDownHeightCrouch;
				float MaxJumpDownHeightStand;
				float MaxJumpDownHeightStorey;
				float MaxJumpDownHeightTower;
				float HoistStepMin;
				float HoistStepMax;
				float HoistCrouchMin;
				float HoistCrouchMax;
				float HoistStandMin;
				float HoistStandMax;
				float VaultStepMin;
				float VaultStepMax;
				float VaultCrouchMin;
				float VaultCrouchMax;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				TagBlock<GravemindProperty> GravemindProperties;
				float ScaryTargetThreshold;
				float ScaryWeaponThreshold;
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
				TagBlock<AiStyle> Styles;
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
				uint32_t Unknown31;
				uint32_t Unknown32;
				uint32_t Unknown33;
				uint32_t Unknown34;
				uint32_t Unknown35;
				uint32_t Unknown36;
				uint32_t Unknown37;
				uint32_t Unknown38;

				struct GravemindProperty
				{
					float MinimumRetreatTime;
					float IdealRetreatTime;
					float MaximumRetreatTime;
				};
				TAG_STRUCT_SIZE_ASSERT(GravemindProperty, 0xC);

				struct AiStyle
				{
					TagReference Style;
				};
				TAG_STRUCT_SIZE_ASSERT(AiStyle, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(AiGlobal, 0x144);

			struct DamageTable
			{
				struct DamageGroup;

				TagBlock<DamageGroup> DamageGroups;

				struct DamageGroup
				{
					struct ArmorModifier;

					int32_t Name;
					TagBlock<ArmorModifier> ArmorModifiers;

					struct ArmorModifier
					{
						int32_t Name;
						float DamageMultiplier;
					};
					TAG_STRUCT_SIZE_ASSERT(ArmorModifier, 0x8);
				};
				TAG_STRUCT_SIZE_ASSERT(DamageGroup, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(DamageTable, 0xC);

			struct Sound
			{
				TagReference SoundObsolete;
			};
			TAG_STRUCT_SIZE_ASSERT(Sound, 0x10);

			struct Camera
			{
				TagReference DefaultUnitCameraTrack;
				float Unknown;
				float Unknown2;
				float Unknown3;
				float Unknown4;
				float Unknown5;
				float Unknown6;
				float Unknown7;
				float Unknown8;
				float Unknown9;
				float Unknown10;
				float Unknown11;
				float Unknown12;
				float Unknown13;
				float Unknown14;
				float Unknown15;
				float Unknown16;
				float Unknown17;
				float Unknown18;
				float Unknown19;
				float Unknown20;
				float Unknown21;
				float Unknown22;
				float Unknown23;
				float Unknown24;
				int16_t Unknown25;
				int16_t Unknown26;
				uint32_t Unknown27;
				uint32_t Unknown28;
				uint32_t Unknown29;
				uint32_t Unknown30;
				uint32_t Unknown31;
				uint32_t Unknown32;
				uint32_t Unknown33;
				uint32_t Unknown34;
				uint32_t Unknown35;
				uint32_t Unknown36;
				uint32_t Unknown37;
				uint32_t Unknown38;
			};
			TAG_STRUCT_SIZE_ASSERT(Camera, 0xA4);

			struct PlayerControl
			{
				struct LookFunction;

				uint32_t MagnetismFriction;
				uint32_t MagnetismAdhesion;
				uint32_t InconsequentialTargetScale;
				uint32_t CrosshairLocationX;
				uint32_t CrosshairLocationY;
				uint32_t SecondsToStart;
				uint32_t SecondsToFullSpeed;
				uint32_t DecayRate;
				uint32_t FullSpeedMultiplierS1X;
				uint32_t PeggedMagnitude;
				uint32_t PeggedAngularThreshold;
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t LookDefaultPitchRate;
				uint32_t LookDefaultYawRate;
				uint32_t LookPegThreshold;
				uint32_t LookYawAccelerationTime;
				uint32_t LookYawAccelerationScale;
				uint32_t LookPitchAccelerationTime;
				uint32_t LookPitchAccelerationScale;
				uint32_t LookAutolevelingScale;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t GravityScale;
				int16_t Unknown5;
				int16_t MinimumAutolevelingTicks;
				TagBlock<LookFunction> LookFunction2;

				struct LookFunction
				{
					float Scale;
				};
				TAG_STRUCT_SIZE_ASSERT(LookFunction, 0x4);
			};
			TAG_STRUCT_SIZE_ASSERT(PlayerControl, 0x70);

			struct Difficulty
			{
				float EasyEnemyDamage;
				float NormalEnemyDamage;
				float HardEnemyDamage;
				float ImpossibleEnemyDamage;
				float EasyEnemyVitality;
				float NormalEnemyVitality;
				float HardEnemyVitality;
				float ImpossibleEnemyVitality;
				float EasyEnemyShield;
				float NormalEnemyShield;
				float HardEnemyShield;
				float ImpossibleEnemyShield;
				float EasyEnemyRecharge;
				float NormalEnemyRecharge;
				float HardEnemyRecharge;
				float ImpossibleEnemyRecharge;
				float EasyFriendDamage;
				float NormalFriendDamage;
				float HardFriendDamage;
				float ImpossibleFriendDamage;
				float EasyFriendVitality;
				float NormalFriendVitality;
				float HardFriendVitality;
				float ImpossibleFriendVitality;
				float EasyFriendShield;
				float NormalFriendShield;
				float HardFriendShield;
				float ImpossibleFriendShield;
				float EasyFriendRecharge;
				float NormalFriendRecharge;
				float HardFriendRecharge;
				float ImpossibleFriendRecharge;
				float EasyInfectionForms;
				float NormalInfectionForms;
				float HardInfectionForms;
				float ImpossibleInfectionForms;
				float EasyUnknown;
				float NormalUnknown;
				float HardUnknown;
				float ImpossibleUnknown;
				float EasyRateOfFire;
				float NormalRateOfFire;
				float HardRateOfFire;
				float ImpossibleRateOfFire;
				float EasyProjectileError;
				float NormalProjectileError;
				float HardProjectileError;
				float ImpossibleProjectileError;
				float EasyBurstError;
				float NormalBurstError;
				float HardBurstError;
				float ImpossibleBurstError;
				float EasyTargetDelay;
				float NormalTargetDelay;
				float HardTargetDelay;
				float ImpossibleTargetDelay;
				float EasyBurstSeparation;
				float NormalBurstSeparation;
				float HardBurstSeparation;
				float ImpossibleBurstSeparation;
				float EasyTargetTracking;
				float NormalTargetTracking;
				float HardTargetTracking;
				float ImpossibleTargetTracking;
				float EasyTargetLeading;
				float NormalTargetLeading;
				float HardTargetLeading;
				float ImpossibleTargetLeading;
				float EasyOverchargeChance;
				float NormalOverchargeChance;
				float HardOverchargeChance;
				float ImpossibleOverchargeChance;
				float EasySpecialFireDelay;
				float NormalSpecialFireDelay;
				float HardSpecialFireDelay;
				float ImpossibleSpecialFireDelay;
				float EasyGuidanceVsPlayer;
				float NormalGuidanceVsPlayer;
				float HardGuidanceVsPlayer;
				float ImpossibleGuidanceVsPlayer;
				float EasyMeleeDelayBase;
				float NormalMeleeDelayBase;
				float HardMeleeDelayBase;
				float ImpossibleMeleeDelayBase;
				float EasyMeleeDelayScale;
				float NormalMeleeDelayScale;
				float HardMeleeDelayScale;
				float ImpossibleMeleeDelayScale;
				float EasyUnknown2;
				float NormalUnknown2;
				float HardUnknown2;
				float ImpossibleUnknown2;
				float EasyGrenadeChanceScale;
				float NormalGrenadeChanceScale;
				float HardGrenadeChanceScale;
				float ImpossibleGrenadeChanceScale;
				float EasyGrenadeTimerScale;
				float NormalGrenadeTimerScale;
				float HardGrenadeTimerScale;
				float ImpossibleGrenadeTimerScale;
				float EasyUnknown3;
				float NormalUnknown3;
				float HardUnknown3;
				float ImpossibleUnknown3;
				float EasyUnknown4;
				float NormalUnknown4;
				float HardUnknown4;
				float ImpossibleUnknown4;
				float EasyUnknown5;
				float NormalUnknown5;
				float HardUnknown5;
				float ImpossibleUnknown5;
				float EasyMajorUpgradeNormal;
				float NormalMajorUpgradeNormal;
				float HardMajorUpgradeNormal;
				float ImpossibleMajorUpgradeNormal;
				float EasyMajorUpgradeFew;
				float NormalMajorUpgradeFew;
				float HardMajorUpgradeFew;
				float ImpossibleMajorUpgradeFew;
				float EasyMajorUpgradeMany;
				float NormalMajorUpgradeMany;
				float HardMajorUpgradeMany;
				float ImpossibleMajorUpgradeMany;
				float EasyPlayerVehicleRamChance;
				float NormalPlayerVehicleRamChance;
				float HardPlayerVehicleRamChance;
				float ImpossiblePlayerVehicleRamChance;
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
				uint32_t Unknown31;
				uint32_t Unknown32;
				uint32_t Unknown33;
			};
			TAG_STRUCT_SIZE_ASSERT(Difficulty, 0x284);

			struct Grenade
			{
				int16_t MaximumCount;
				int16_t Unknown;
				TagReference ThrowingEffect;
				uint32_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				TagReference Equipment;
				TagReference Projectile;
			};
			TAG_STRUCT_SIZE_ASSERT(Grenade, 0x44);

			struct InterfaceTag
			{
				struct GfxUiString;

				TagReference Spinner;
				TagReference Obsolete;
				TagReference ScreenColorTable;
				TagReference HudColorTable;
				TagReference EditorColorTable;
				TagReference DialogColorTable;
				TagReference MotionSensorSweepBitmap;
				TagReference MotionSensorSweepBitmapMask;
				TagReference MultiplayerHudBitmap;
				TagReference HudDigitsDefinition;
				TagReference MotionSensorBlipBitmap;
				TagReference InterfaceGooMap1;
				TagReference InterfaceGooMap2;
				TagReference InterfaceGooMap3;
				TagReference MainMenuUiGlobals;
				TagReference SinglePlayerUiGlobals;
				TagReference MultiplayerUiGlobals;
				TagReference HudGlobals;
				TagBlock<GfxUiString> GfxUiStrings;

				struct GfxUiString
				{
					char Name[32];
					TagReference Strings;
				};
				TAG_STRUCT_SIZE_ASSERT(GfxUiString, 0x30);
			};
			TAG_STRUCT_SIZE_ASSERT(InterfaceTag, 0x12C);

			struct PlayerInformation
			{
				float WalkingSpeed;
				float RunForward;
				float RunBackward;
				float RunSideways;
				float RunAcceleration;
				float SneakForward;
				float SneakBackward;
				float SneakSideways;
				float SneakAcceleration;
				float AirborneAcceleration;
				float GrenadeOriginX;
				float GrenadeOriginY;
				float GrenadeOriginZ;
				float StunMovementPenalty;
				float StunTurningPenalty;
				float StunJumpingPenalty;
				float MinimumStunTime;
				float MaximumStunTime;
				float FirstPersonIdleTimeMin;
				float FirstPersonIdleTimeMax;
				float FirstPersonSkipFraction;
				uint32_t Unknown;
				TagReference Unknown2;
				TagReference Unknown3;
				TagReference Unknown4;
				int32_t BinocularsZoomCount;
				float BinocularZoomRangeMin;
				float BinocularZoomRangeMax;
				uint32_t Unknown5;
				uint32_t Unknown6;
				TagReference FlashlightOn;
				TagReference FlashlightOff;
				TagReference DefaultDamageResponse;
			};
			TAG_STRUCT_SIZE_ASSERT(PlayerInformation, 0xCC);

			struct PlayerRepresentation
			{
				int32_t Name;
				uint32_t Flags;
				TagReference FirstPersonHands;
				TagReference FirstPersonBody;
				TagReference ThirdPersonUnit;
				int32_t ThirdPersonVariant;
				TagReference BinocularsZoomInSound;
				TagReference BinocularsZoomOutSound;
				TagReference Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(PlayerRepresentation, 0x6C);

			struct PlayerFallingDamage
			{
				float HarmfulFallingDistanceMin;
				float HarmfulFallingDistanceMax;
				TagReference FallingDamage;
				TagReference Unknown;
				TagReference SoftLanding;
				TagReference HardLanding;
				TagReference ScriptDamage;
				float MaximumFallingDistance;
				TagReference DistanceDamage;
				float Unknown2;
				float Unknown3;
				float Unknown4;
			};
			TAG_STRUCT_SIZE_ASSERT(PlayerFallingDamage, 0x78);

			struct Unknown59
			{
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown59, 0xC);

			struct Material
			{
				struct WaterDragProperty;
				struct WaterInteraction;

				int32_t Name;
				int32_t ParentName;
				int16_t ParentIndex;
				uint16_t Flags;
				int32_t GeneralArmor;
				int32_t SpecificArmor;
				uint32_t Unknown;
				float Friction;
				float Restitution;
				float Density;
				TagBlock<WaterDragProperty> WaterDragProperties;
				TagReference BreakableSurface;
				TagReference SoundSweetenerSmall;
				TagReference SoundSweetenerMedium;
				TagReference SoundSweetenerLarge;
				TagReference SoundSweetenerRolling;
				TagReference SoundSweetenerGrinding;
				TagReference SoundSweetenerMeleeSmall;
				TagReference SoundSweetenerMeleeMedium;
				TagReference SoundSweetenerMeleeLarge;
				TagReference EffectSweetenerSmall;
				TagReference EffectSweetenerMedium;
				TagReference EffectSweetenerLarge;
				TagReference EffectSweetenerRolling;
				TagReference EffectSweetenerGrinding;
				TagReference EffectSweetenerMelee;
				TagReference WaterRippleSmall;
				TagReference WaterRippleMedium;
				TagReference WaterRippleLarge;
				uint32_t SweetenerInheritanceFlags;
				TagReference MaterialEffects;
				TagBlock<WaterInteraction> WaterInteraction2;
				uint32_t Unknown2;
				int16_t Unknown3;
				int16_t Unknown4;

				struct WaterDragProperty
				{
					float Unknown;
					float Unknown2;
					float Unknown3;
					float Unknown4;
					float Unknown5;
					float Unknown6;
					float Unknown7;
					float Unknown8;
					float Unknown9;
					uint32_t Unknown10;
				};
				TAG_STRUCT_SIZE_ASSERT(WaterDragProperty, 0x28);

				struct WaterInteraction
				{
					int32_t SurfaceName;
					int32_t SubmergedName;
					int16_t SurfaceIndex;
					int16_t SubmergedIndex;
				};
				TAG_STRUCT_SIZE_ASSERT(WaterInteraction, 0xC);
			};
			TAG_STRUCT_SIZE_ASSERT(Material, 0x178);

			struct CinematicAnchor
			{
				TagReference Anchor;
				uint32_t Unknown;
				uint32_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(CinematicAnchor, 0x18);

			struct MetagameGlobal
			{
				struct Medal;
				struct Difficulty;
				struct PrimarySkull;
				struct SecondarySkull;

				TagBlock<Medal> Medals;
				TagBlock<Difficulty> Difficulty2;
				TagBlock<PrimarySkull> PrimarySkulls;
				TagBlock<SecondarySkull> SecondarySkulls;
				int32_t Unknown;
				int32_t DeathPenalty;
				int32_t BetrayalPenalty;
				int32_t Unknown2;
				float MultikillWindow;
				float EmpWindow;
				float Unknown3;
				float Unknown4;
				float Unknown5;
				int32_t FirstWeaponSpree;
				int32_t SecondWeaponSpree;
				int32_t KillingSpree;
				int32_t KillingFrenzy;
				int32_t RunningRiot;
				int32_t Rampage;
				int32_t Untouchable;
				int32_t Invincible;
				int32_t DoubleKill;
				int32_t TripleKill;
				int32_t Overkill;
				int32_t Killtacular;
				int32_t Killtrocity;
				int32_t Killimanjaro;
				int32_t Killtastrophe;
				int32_t Killpocalypse;
				int32_t Killionaire;

				struct Medal
				{
					float Multiplier;
					int32_t AwardedPoints;
					int32_t MedalUptime;
					int32_t EventName;
				};
				TAG_STRUCT_SIZE_ASSERT(Medal, 0x10);

				struct Difficulty
				{
					float Multiplier;
				};
				TAG_STRUCT_SIZE_ASSERT(Difficulty, 0x4);

				struct PrimarySkull
				{
					float Multiplier;
				};
				TAG_STRUCT_SIZE_ASSERT(PrimarySkull, 0x4);

				struct SecondarySkull
				{
					float Multiplier;
				};
				TAG_STRUCT_SIZE_ASSERT(SecondarySkull, 0x4);
			};
			TAG_STRUCT_SIZE_ASSERT(MetagameGlobal, 0x98);

			struct DamageReportingType
			{
				int16_t Index;
				int16_t Version;
				char Name[32];
			};
			TAG_STRUCT_SIZE_ASSERT(DamageReportingType, 0x24);
		};
		TAG_STRUCT_SIZE_ASSERT(Globals, 0x608);
	}
}
