#pragma once
#include <cstdint>
#include "../../Math/Bounds.hpp"
#include "../../Math/RealPoint2D.hpp"
#include "../../Math/RealPoint3D.hpp"
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Game
{
	using Blam::Math::Bounds;
	using Blam::Math::RealPoint2D;
	using Blam::Math::RealPoint3D;
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct Globals : TagGroup<'matg'>
	{
		struct HavokCleanupResource;
		struct SoundGlobal;
		struct AIGlobal;
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

		float Unknown1;
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
		float Unknown25;
		float Unknown26;
		float Unknown27;
		float Unknown28;
		float Unknown29;
		float Unknown30;
		float Unknown31;
		float Unknown32;
		float Unknown33;
		float Unknown34;
		float Unknown35;
		float Unknown36;
		float Unknown37;
		float Unknown38;
		float Unknown39;
		float Unknown40;
		float Unknown41;
		float Unknown42;
		float Unknown43;
		int32_t Language;
		TagBlock<Globals::HavokCleanupResource> HavokCleanupResources;
		TagBlock<Globals::SoundGlobal> SoundGlobals;
		TagBlock<Globals::AIGlobal> AIGlobals;
		TagReference AIGlobalsTag;
		TagBlock<Globals::DamageTable> DamageTable;
		float Unknown44;
		float Unknown45;
		float Unknown46;
		TagBlock<Globals::Sound> Sounds;
		TagBlock<Globals::Camera> Camera;
		TagBlock<Globals::PlayerControl> PlayerControl;
		TagBlock<Globals::Difficulty> Difficulty;
		TagBlock<Globals::Grenade> Grenades;
		float Unknown47;
		float Unknown48;
		float Unknown49;
		TagBlock<Globals::InterfaceTag> InterfaceTags;
		float Unknown50;
		float Unknown51;
		float Unknown52;
		float Unknown53;
		float Unknown54;
		float Unknown55;
		TagBlock<Globals::PlayerInformation> PlayerInformation;
		TagBlock<Globals::PlayerRepresentation> PlayerRepresentation;
		float Unknown56;
		float Unknown57;
		float Unknown58;
		TagBlock<Globals::PlayerFallingDamage> PlayerFallingDamage;
		TagBlock<Globals::Unknown59> Unknown60;
		TagBlock<Globals::Material> Materials;
		TagReference MultiplayerGlobals;
		TagReference SurvivalGlobals;
		TagBlock<Globals::CinematicAnchor> CinematicAnchors;
		TagBlock<Globals::MetagameGlobal> MetagameGlobals;
		float Unknown61;
		float Unknown62;
		float Unknown63;
		float Unknown64;
		float Unknown65;
		float Unknown66;
		float Unknown67;
		float Unknown68;
		float Unknown69;
		float Unknown70;
		float Unknown71;
		float Unknown72;
		float Unknown73;
		float Unknown74;
		float Unknown75;
		float Unknown76;
		float Unknown77;
		float Unknown78;
		float Unknown79;
		float Unknown80;
		float Unknown81;
		float Unknown82;
		float Unknown83;
		float Unknown84;
		float Unknown85;
		float Unknown86;
		float Unknown87;
		float Unknown88;
		float Unknown89;
		float Unknown90;
		float Unknown91;
		float Unknown92;
		float Unknown93;
		float Unknown94;
		float Unknown95;
		float Unknown96;
		float Unknown97;
		float Unknown98;
		float Unknown99;
		float Unknown100;
		float Unknown101;
		float Unknown102;
		float Unknown103;
		float Unknown104;
		float Unknown105;
		float Unknown106;
		float Unknown107;
		float Unknown108;
		float Unknown109;
		float Unknown110;
		float Unknown111;
		float Unknown112;
		float Unknown113;
		float Unknown114;
		float Unknown115;
		float Unknown116;
		float Unknown117;
		float Unknown118;
		float Unknown119;
		float Unknown120;
		float Unknown121;
		float Unknown122;
		float Unknown123;
		float Unknown124;
		float Unknown125;
		float Unknown126;
		float Unknown127;
		float Unknown128;
		float Unknown129;
		float Unknown130;
		float Unknown131;
		float Unknown132;
		float Unknown133;
		float Unknown134;
		float Unknown135;
		float Unknown136;
		float Unknown137;
		float Unknown138;
		float Unknown139;
		float Unknown140;
		float Unknown141;
		float Unknown142;
		float Unknown143;
		float Unknown144;
		float Unknown145;
		float Unknown146;
		float Unknown147;
		float Unknown148;
		float Unknown149;
		float Unknown150;
		float Unknown151;
		float Unknown152;
		float Unknown153;
		float Unknown154;
		float Unknown155;
		float Unknown156;
		float Unknown157;
		float Unknown158;
		float Unknown159;
		float Unknown160;
		float Unknown161;
		float Unknown162;
		float Unknown163;
		float Unknown164;
		float Unknown165;
		float Unknown166;
		float Unknown167;
		float Unknown168;
		float Unknown169;
		float Unknown170;
		float Unknown171;
		float Unknown172;
		float Unknown173;
		float Unknown174;
		float Unknown175;
		float Unknown176;
		float Unknown177;
		float Unknown178;
		float Unknown179;
		float Unknown180;
		float Unknown181;
		float Unknown182;
		float Unknown183;
		float Unknown184;
		float Unknown185;
		float Unknown186;
		float Unknown187;
		float Unknown188;
		float Unknown189;
		float Unknown190;
		float Unknown191;
		float Unknown192;
		float Unknown193;
		float Unknown194;
		float Unknown195;
		float Unknown196;
		float Unknown197;
		float Unknown198;
		float Unknown199;
		float Unknown200;
		float Unknown201;
		float Unknown202;
		float Unknown203;
		float Unknown204;
		float Unknown205;
		float Unknown206;
		float Unknown207;
		float Unknown208;
		float Unknown209;
		float Unknown210;
		float Unknown211;
		float Unknown212;
		float Unknown213;
		float Unknown214;
		float Unknown215;
		float Unknown216;
		float Unknown217;
		float Unknown218;
		float Unknown219;
		float Unknown220;
		float Unknown221;
		float Unknown222;
		float Unknown223;
		float Unknown224;
		float Unknown225;
		float Unknown226;
		float Unknown227;
		float Unknown228;
		float Unknown229;
		float Unknown230;
		float Unknown231;
		float Unknown232;
		float Unknown233;
		float Unknown234;
		float Unknown235;
		float Unknown236;
		float Unknown237;
		float Unknown238;
		float Unknown239;
		float Unknown240;
		float Unknown241;
		float Unknown242;
		float Unknown243;
		float Unknown244;
		float Unknown245;
		float Unknown246;
		float Unknown247;
		float Unknown248;
		float Unknown249;
		float Unknown250;
		float Unknown251;
		float Unknown252;
		float Unknown253;
		float Unknown254;
		float Unknown255;
		float Unknown256;
		float Unknown257;
		float Unknown258;
		float Unknown259;
		float Unknown260;
		float Unknown261;
		float Unknown262;
		float Unknown263;
		float Unknown264;
		TagReference RasterizerGlobals;
		TagReference DefaultCameraEffect;
		TagReference PodiumDefinition;
		TagReference DefaultWind;
		TagReference DefaultDamageEffect;
		TagReference DefaultCollisionDamage;
		StringID UnknownMaterial;
		int16_t UnknownGlobalMaterialIndex;
		int16_t Unknown265;
		TagReference EffectGlobals;
		TagReference GameProgressionGlobals;
		TagReference AchievementGlobals;
		TagReference InputGlobals;
		float Unknown266;
		float Unknown267;
		float Unknown268;
		float Unknown269;
		TagData<uint8_t> Unknown270;
		float Unknown271;
		float Unknown272;
		float Unknown273;
		float Unknown274;
		float Unknown275;
		TagBlock<Globals::DamageReportingType> DamageReportingTypes;
		float Unknown276;

		struct Globals::HavokCleanupResource
		{
			TagReference ObjectCleanupEffect;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::HavokCleanupResource, 0x10);

		struct Globals::SoundGlobal
		{
			TagReference SoundClasses;
			TagReference SoundEffects;
			TagReference SoundMix;
			TagReference SoundCombatDialogueConstants;
			TagReference SoundGlobalPropagation;
			TagReference GfxUiSounds;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::SoundGlobal, 0x60);

		struct Globals::AIGlobal
		{
			struct GravemindProperty;
			struct Style;

			float Unknown1;
			float Unknown2;
			float Unknown3;
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
			Bounds<float> HoistStepBounds;
			Bounds<float> HoistCrouchBounds;
			Bounds<float> HoistStandBounds;
			Bounds<float> VaultStepBounds;
			Bounds<float> VaultCrouchBounds;
			float Unknown4;
			float Unknown5;
			float Unknown6;
			float Unknown7;
			TagBlock<Globals::AIGlobal::GravemindProperty> GravemindProperties;
			float ScaryTargetThreshold;
			float ScaryWeaponThreshold;
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
			TagBlock<Globals::AIGlobal::Style> Styles;
			float Unknown19;
			float Unknown20;
			float Unknown21;
			float Unknown22;
			float Unknown23;
			float Unknown24;
			float Unknown25;
			float Unknown26;
			float Unknown27;
			float Unknown28;
			float Unknown29;
			float Unknown30;
			float Unknown31;
			float Unknown32;
			float Unknown33;
			float Unknown34;
			float Unknown35;
			float Unknown36;
			float Unknown37;
			float Unknown38;

			struct Globals::AIGlobal::GravemindProperty
			{
				float MinimumRetreatTime;
				float IdealRetreatTime;
				float MaximumRetreatTime;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::AIGlobal::GravemindProperty, 0xC);

			struct Globals::AIGlobal::Style
			{
				TagReference Reference;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::AIGlobal::Style, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::AIGlobal, 0x144);

		struct Globals::DamageTable
		{
			struct DamageGroup;

			TagBlock<Globals::DamageTable::DamageGroup> DamageGroups;

			struct Globals::DamageTable::DamageGroup
			{
				struct ArmorModifier;

				StringID Name;
				TagBlock<Globals::DamageTable::DamageGroup::ArmorModifier> ArmorModifiers;

				struct Globals::DamageTable::DamageGroup::ArmorModifier
				{
					StringID Name;
					float DamageMultiplier;
				};
				TAG_STRUCT_SIZE_ASSERT(struct Globals::DamageTable::DamageGroup::ArmorModifier, 0x8);
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::DamageTable::DamageGroup, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(struct DamageTable, 0xC);

		struct Globals::Sound
		{
			TagReference Reference; // Obsolete
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::Sound, 0x10);

		struct Globals::Camera
		{
			TagReference DefaultUnitCameraTrack;
			float Unknown1;
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
			float Unknown27;
			float Unknown28;
			float Unknown29;
			float Unknown30;
			float Unknown31;
			float Unknown32;
			float Unknown33;
			float Unknown34;
			float Unknown35;
			float Unknown36;
			float Unknown37;
			float Unknown38;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::Camera, 0xA4);

		struct Globals::PlayerControl
		{
			struct LookFunction;

			float MagnetismFriction;
			float MagnetismAdhesion;
			float InconsequentialTargetScale;
			RealPoint2D CrosshairLocation;
			float SprintSecondsToStart;
			float SprintSecondsToFullSpeed;
			float SprintDecayRate;
			float SprintFullSpeedMultiplier;
			float SprintPeggedMagnitude;
			float SprintPeggedAngularThreshold;
			float SprintUnknown1;
			float SprintUnknown2;
			float LookDefaultPitchRate;
			float LookDefaultYawRate;
			float LookPegThreshold;
			float LookYawAccelerationTime;
			float LookYawAccelerationScale;
			float LookPitchAccelerationTime;
			float LookPitchAccelerationScale;
			float LookAutolevelingScale;
			float Unknown1;
			float Unknown2;
			float GravityScale;
			int16_t Unknown5;
			int16_t MinimumAutolevelingTicks;
			TagBlock<Globals::PlayerControl::LookFunction> LookFunction;

			struct Globals::PlayerControl::LookFunction
			{
				float Scale;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::PlayerControl::LookFunction, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::PlayerControl, 0x70);

		struct Globals::Difficulty
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
			float Unknown25;
			float Unknown26;
			float Unknown27;
			float Unknown28;
			float Unknown29;
			float Unknown30;
			float Unknown31;
			float Unknown32;
			float Unknown33;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::Difficulty, 0x284);

		struct Globals::Grenade
		{
			int16_t MaximumCount;
			int16_t Unknown1;
			TagReference ThrowingEffect;
			float Unknown2;
			float Unknown3;
			float Unknown4;
			float Unknown5;
			TagReference Equipment;
			TagReference Projectile;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Grenade, 0x44);

		struct Globals::InterfaceTag
		{
			struct UIString;

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
			TagBlock<Globals::InterfaceTag::UIString> UIStrings;

			struct UIString
			{
				char Name[32];
				TagReference Strings;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::InterfaceTag::UIString, 0x30);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::InterfaceTag, 0x12C);

		struct Globals::PlayerInformation
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
			RealPoint3D GrenadeOrigin;
			float StunMovementPenalty;
			float StunTurningPenalty;
			float StunJumpingPenalty;
			Bounds<float> StunTimeBounds;
			Bounds<float> FirstPersonIdleTimeBounds;
			float FirstPersonSkipFraction;
			float Unknown1;
			TagReference Unknown2;
			TagReference Unknown3;
			TagReference Unknown4;
			int32_t BinocularsZoomCount;
			Bounds<float> BinocularZoomBounds;
			float Unknown5;
			float Unknown6;
			TagReference FlashlightOn;
			TagReference FlashlightOff;
			TagReference DefaultDamageResponse;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::PlayerInformation, 0xCC);

		struct Globals::PlayerRepresentation
		{
			enum class Flags : int32_t;

			StringID Name;
			Globals::PlayerRepresentation::Flags Flags : 32;
			TagReference FirstPersonHands;
			TagReference FirstPersonBody;
			TagReference ThirdPersonUnit;
			int32_t ThirdPersonVariant;
			TagReference BinocularsZoomInSound;
			TagReference BinocularsZoomOutSound;
			TagReference Unknown;

			enum class Globals::PlayerRepresentation::Flags : int32_t
			{
				None,
				Elite = 1 << 0
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct PlayerRepresentation, 0x6C);

		struct Globals::PlayerFallingDamage
		{
			Bounds<float> HarmfulFallingDistanceBounds;
			TagReference FallingDamage;
			TagReference Unknown1;
			TagReference SoftLanding;
			TagReference HardLanding;
			TagReference ScriptDamage;
			float MaximumFallingDistance;
			TagReference DistanceDamage;
			float Unknown2;
			float Unknown3;
			float Unknown4;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::PlayerFallingDamage, 0x78);

		struct Globals::Unknown59
		{
			float Unknown1;
			float Unknown2;
			float Unknown3;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::Unknown59, 0xC);

		struct Globals::Material
		{
			enum class Flags : uint16_t;
			struct WaterDragProperty;
			enum class SweetenerInheritanceFlags : int32_t;
			struct WaterInteraction;

			StringID Name;
			StringID ParentName;
			int16_t ParentIndex;
			Globals::Material::Flags Flags : 16;
			StringID GeneralArmor;
			StringID SpecificArmor;
			float Unknown1;
			float Friction;
			float Restitution;
			float Density;
			TagBlock<Globals::Material::WaterDragProperty> WaterDragProperties;
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
			Globals::Material::SweetenerInheritanceFlags SweetenerInheritanceFlags : 32;
			TagReference MaterialEffects;
			TagBlock<Globals::Material::WaterInteraction> WaterInteraction;
			float Unknown2;
			int16_t Unknown3;
			int16_t Unknown4;

			enum class Globals::Material::Flags : uint16_t
			{
				None,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			struct Globals::Material::WaterDragProperty
			{
				float Unknown1;
				float Unknown2;
				float Unknown3;
				float Unknown4;
				float Unknown5;
				float Unknown6;
				float Unknown7;
				float Unknown8;
				float Unknown9;
				float Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::Material::WaterDragProperty, 0x28);

			enum class Globals::Material::SweetenerInheritanceFlags : int32_t
			{
				None,
				SoundSmall = 1 << 0,
				SoundMedium = 1 << 1,
				SoundLarge = 1 << 2,
				SoundRolling = 1 << 3,
				SoundGrinding = 1 << 4,
				SoundMeleeSmall = 1 << 5,
				SoundMeleeMedium = 1 << 6,
				SoundMeleeLarge = 1 << 7,
				EffectSmall = 1 << 8,
				EffectMedium = 1 << 9,
				EffectLarge = 1 << 10,
				EffectRolling = 1 << 11,
				EffectGrinding = 1 << 12,
				EffectMelee = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15,
				Bit16 = 1 << 16
			};

			struct Globals::Material::WaterInteraction
			{
				StringID SurfaceName;
				StringID SubmergedName;
				int16_t SurfaceIndex;
				int16_t SubmergedIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::Material::WaterInteraction, 0xC);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::Material, 0x178);

		struct Globals::CinematicAnchor
		{
			TagReference Anchor;
			float Unknown1;
			float Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::CinematicAnchor, 0x18);

		struct Globals::MetagameGlobal
		{
			struct Medal;
			struct Difficulty;
			struct PrimarySkull;
			struct SecondarySkull;

			TagBlock<Globals::MetagameGlobal::Medal> Medals;
			TagBlock<Globals::MetagameGlobal::Difficulty> Difficulty;
			TagBlock<Globals::MetagameGlobal::PrimarySkull> PrimarySkulls;
			TagBlock<Globals::MetagameGlobal::SecondarySkull> SecondarySkulls;
			int32_t Unknown1;
			int32_t DeathPenalty;
			int32_t BetrayalPenalty;
			int32_t Unknown2;
			float MultikillWindow;
			float EMPWindow;
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

			struct Globals::MetagameGlobal::Medal
			{
				float Multiplier;
				int32_t AwardedPoints;
				int32_t MedalUptime;
				int32_t EventName;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::MetagameGlobal::Medal, 0x10);

			struct Globals::MetagameGlobal::Difficulty
			{
				float Multiplier;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::MetagameGlobal::Difficulty, 0x4);

			struct Globals::MetagameGlobal::PrimarySkull
			{
				float Multiplier;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::MetagameGlobal::PrimarySkull, 0x4);

			struct Globals::MetagameGlobal::SecondarySkull
			{
				float Multiplier;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Globals::MetagameGlobal::SecondarySkull, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::MetagameGlobal, 0x98);

		struct Globals::DamageReportingType
		{
			int16_t Index;
			int16_t Version;
			char Name[32];
		};
		TAG_STRUCT_SIZE_ASSERT(struct Globals::DamageReportingType, 0x24);
	};
	TAG_STRUCT_SIZE_ASSERT(struct Globals, 0x608);
}

