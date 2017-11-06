#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Game
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct MultiplayerGlobals : TagGroup<'mulg'>
	{
		struct Universal;
		struct Runtime;

		TagBlock<MultiplayerGlobals::Universal> Universal;
		TagBlock<MultiplayerGlobals::Runtime> Runtime;

		struct MultiplayerGlobals::Universal
		{
			struct ArmorCustomization;
			struct GameEquipment;
			struct Unknown;
			struct GameVariantWeapon;
			struct GameVariantVehicle;
			struct GameVariantEquipment;
			struct WeaponSet;
			struct VehicleSet;
			struct PodiumAnimation;

			TagReference RandomPlayerNameStrings;
			TagReference TeamNameStrings;
			TagBlock<MultiplayerGlobals::Universal::ArmorCustomization> SpartanArmorCustomization;
			TagBlock<MultiplayerGlobals::Universal::ArmorCustomization> EliteArmorCustomization;
			TagBlock<MultiplayerGlobals::Universal::GameEquipment> Equipment;
			TagBlock<MultiplayerGlobals::Universal::Unknown> Unknown;
			TagReference MultiplayerStrings;
			TagReference SandboxUIStrings;
			TagReference SandboxUIProperties;
			TagBlock<MultiplayerGlobals::Universal::GameVariantWeapon> GameVariantWeapons;
			TagBlock<MultiplayerGlobals::Universal::GameVariantVehicle> GameVariantVehicles;
			TagBlock<MultiplayerGlobals::Universal::GameVariantEquipment> GameVariantEquipment;
			TagBlock<MultiplayerGlobals::Universal::WeaponSet> WeaponSets;
			TagBlock<MultiplayerGlobals::Universal::VehicleSet> VehicleSets;
			TagBlock<MultiplayerGlobals::Universal::PodiumAnimation> PodiumAnimations;
			TagReference EngineSettings;

			struct MultiplayerGlobals::Universal::ArmorCustomization
			{
				struct Permutation;

				StringID PieceRegion;
				StringID BipedRegion;
				TagBlock<MultiplayerGlobals::Universal::ArmorCustomization::Permutation> Permutations;

				struct MultiplayerGlobals::Universal::ArmorCustomization::Permutation
				{
					StringID Name;
					TagReference ThirdPersonArmorObject;
					TagReference FirstPersonArmorModel;
					int16_t Unknown1;
					int16_t Unknown2;
					StringID ParentAttachMarker;
					StringID ChildAttachMarker;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::ArmorCustomization::Permutation, 0x30);
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::ArmorCustomization, 0x14);

			struct MultiplayerGlobals::Universal::GameEquipment
			{
				StringID Name;
				TagReference Equipment;
				int16_t Unknown;
				int16_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::GameEquipment, 0x18);

			struct MultiplayerGlobals::Universal::Unknown
			{
				int32_t Unknown1;
				int32_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::Unknown, 0x8);

			struct MultiplayerGlobals::Universal::GameVariantWeapon
			{
				StringID Name;
				float RandomChance;
				TagReference Weapon;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::GameVariantWeapon, 0x18);

			struct MultiplayerGlobals::Universal::GameVariantVehicle
			{
				StringID Name;
				TagReference Vehicle;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::GameVariantVehicle, 0x14);

			struct MultiplayerGlobals::Universal::GameVariantEquipment
			{
				StringID Name;
				TagReference Grenade;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::GameVariantEquipment, 0x14);

			struct MultiplayerGlobals::Universal::WeaponSet
			{
				struct Substitution;

				StringID Name;
				TagBlock<MultiplayerGlobals::Universal::WeaponSet::Substitution> Substitutions;

				struct MultiplayerGlobals::Universal::WeaponSet::Substitution
				{
					StringID OriginalWeapon;
					StringID SubstitutedWeapon;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::WeaponSet::Substitution, 0x8);
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::WeaponSet, 0x10);

			struct MultiplayerGlobals::Universal::VehicleSet
			{
				struct Substitution;

				StringID Name;
				TagBlock<MultiplayerGlobals::Universal::VehicleSet::Substitution> Substitutions;

				struct MultiplayerGlobals::Universal::VehicleSet::Substitution
				{
					StringID OriginalVehicle;
					StringID SubstitutedVehicle;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::VehicleSet::Substitution, 0x8);
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::VehicleSet, 0x10);

			struct MultiplayerGlobals::Universal::PodiumAnimation
			{
				struct Stance;
				struct Alternates;

				TagReference AnimationGraph;
				StringID DefaultUnarmed;
				StringID DefaultArmed;
				TagBlock<MultiplayerGlobals::Universal::PodiumAnimation::Stance> Stances;
				TagBlock<MultiplayerGlobals::Universal::PodiumAnimation::Alternates> Alternates;

				struct MultiplayerGlobals::Universal::PodiumAnimation::Stance
				{
					char Name[32];
					StringID BaseAnimation;
					StringID LoopAnimation;
					StringID UnarmedTransition;
					StringID ArmedTransition;
					float CameraDistanceOffset;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::PodiumAnimation::Stance, 0x34);

				struct MultiplayerGlobals::Universal::PodiumAnimation::Alternates
				{
					enum class Weapon : int32_t;

					char Name[32];
					StringID InAnimation;
					StringID LoopAnimation;
					StringID OutAnimation;
					MultiplayerGlobals::Universal::PodiumAnimation::Alternates::Weapon Weapon : 32;
					TagReference CustomPrimaryWeapon;
					TagReference CustomSecondaryWeapon;

					enum class MultiplayerGlobals::Universal::PodiumAnimation::Alternates::Weapon : int32_t
					{
						Unarmed,
						LoadoutPrimary,
						LoadoutSecondary,
						Custom
					};
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::PodiumAnimation::Alternates, 0x50);
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal::PodiumAnimation, 0x30);
		};
		TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Universal, 0xD8);

		struct MultiplayerGlobals::Runtime
		{
			struct SoundReference;
			struct LoopingSoundReference;
			struct Event;
			struct MultiplayerConstant;
			struct StateResponse;

			TagReference SandboxEditorUnit;
			TagReference SandboxEditorObject;
			TagReference Flag;
			TagReference Ball;
			TagReference Bomb;
			TagReference VIPZone;
			TagReference InGameStrings;
			TagReference Unknown1;
			TagReference Unknown2;
			TagReference Unknown3;
			TagReference Unknown4;
			TagReference Unknown5;
			TagBlock<MultiplayerGlobals::Runtime::SoundReference> Sounds;
			TagBlock<MultiplayerGlobals::Runtime::LoopingSoundReference> LoopingSounds;
			TagBlock<MultiplayerGlobals::Runtime::Event> UnknownEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> GeneralEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> FlavorEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> SlayerEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> CTFEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> OddballEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> KOTHEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> VIPEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> JuggernautEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> TerritoriesEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> AssaultEvents;
			TagBlock<MultiplayerGlobals::Runtime::Event> InfectionEvents;
			int32_t DefaultFragGrenadeCount;
			int32_t DefaultPlasmaGrenadeCount;
			TagBlock<MultiplayerGlobals::Runtime::MultiplayerConstant> MultiplayerConstants;
			TagBlock<MultiplayerGlobals::Runtime::StateResponse> StateResponses;
			TagReference ScoreboardEmblemBitmap;
			TagReference ScoreboardDeadEmblemBitmap;
			TagReference DefaultShapeShader;
			TagReference Unknown6;
			TagReference CTFIntroUI;
			TagReference SlayerIntroUI;
			TagReference OddballIntroUI;
			TagReference KOTHIntroUI;
			TagReference SandboxIntroUI;
			TagReference VIPIntroUI;
			TagReference JuggernautIntroUI;
			TagReference TerritoriesIntroUI;
			TagReference AssaultIntroUI;
			TagReference InfectionIntroUI;
			TagReference MenuMusic1;
			TagReference MenuMusic2;
			TagReference MenuMusic3;
			TagReference Unknown7;

			struct MultiplayerGlobals::Runtime::SoundReference
			{
				TagReference Sound;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::SoundReference, 0x10);

			struct MultiplayerGlobals::Runtime::LoopingSoundReference
			{
				TagReference LoopingSound;
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::LoopingSoundReference, 0x10);

			struct MultiplayerGlobals::Runtime::Event
			{
				enum class Flags : uint16_t;
				enum class Type : int16_t;
				enum class Audience : int16_t;
				enum class Team : int16_t;
				enum class Field : int16_t;
				enum class SoundFlags : uint16_t;

				MultiplayerGlobals::Runtime::Event::Flags Flags : 16;
				MultiplayerGlobals::Runtime::Event::Type Type : 16;
				StringID EventName;
				MultiplayerGlobals::Runtime::Event::Audience Audience : 16;
				int16_t Unknown1;
				MultiplayerGlobals::Runtime::Event::Team Team : 16;
				int16_t Unknown2;
				StringID DisplayString;
				StringID DisplayMedal;
				uint32_t Unknown3;
				uint32_t Unknown4;
				MultiplayerGlobals::Runtime::Event::Field RequiredField : 16;
				MultiplayerGlobals::Runtime::Event::Field ExcludedAudience : 16;
				MultiplayerGlobals::Runtime::Event::Field RequiredField2 : 16;
				MultiplayerGlobals::Runtime::Event::Field ExcludedAudience2 : 16;
				StringID PrimaryString;
				int32_t PrimaryStringDuration;
				StringID PluralDisplayString;
				float AnnouncerSoundDelay;
				MultiplayerGlobals::Runtime::Event::SoundFlags SoundFlags : 16;
				int16_t Unknown5;
				TagReference EnglishSound;
				TagReference JapaneseSound;
				TagReference GermanSound;
				TagReference FrenchSound;
				TagReference SpanishSound;
				TagReference LatinAmericanSpanishSound;
				TagReference ItalianSound;
				TagReference KoreanSound;
				TagReference ChineseTraditionalSound;
				TagReference ChineseSimplifiedSound;
				TagReference PortugueseSound;
				TagReference PolishSound;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;

				enum class MultiplayerGlobals::Runtime::Event::Flags : uint16_t
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

				enum class MultiplayerGlobals::Runtime::Event::Type : int16_t
				{
					General,
					Flavor,
					Slayer,
					CTF,
					Oddball,
					Unused,
					KOTH,
					VIP,
					Juggernaut,
					Territories,
					Assault,
					Infection,
					Survival,
					Unknown
				};

				enum class MultiplayerGlobals::Runtime::Event::Audience : int16_t
				{
					CausePlayer,
					CauseTeam,
					EffectPlayer,
					EffectTeam,
					All
				};

				enum class MultiplayerGlobals::Runtime::Event::Team : int16_t
				{
					PlayerOnly,
					Cause,
					Effect,
					All
				};

				enum class MultiplayerGlobals::Runtime::Event::Field : int16_t
				{
					None,
					CausePlayer,
					CauseTeam,
					EffectPlayer,
					EffectTeam
				};

				enum class MultiplayerGlobals::Runtime::Event::SoundFlags : uint16_t
				{
					None,
					AnnouncerSound = 1 << 0
				};
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::Event, 0x10C);

			struct MultiplayerGlobals::Runtime::MultiplayerConstant
			{
				struct GameWeapon;
				struct GameVehicle;
				struct GameProjectile;
				struct GameEquipment;

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
				TagBlock<MultiplayerGlobals::Runtime::MultiplayerConstant::GameWeapon> Weapons;
				TagBlock<MultiplayerGlobals::Runtime::MultiplayerConstant::GameVehicle> Vehicles;
				TagBlock<MultiplayerGlobals::Runtime::MultiplayerConstant::GameProjectile> Projectiles;
				TagBlock<MultiplayerGlobals::Runtime::MultiplayerConstant::GameEquipment> Equipment;
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
				float Unknown44;
				float Unknown45;
				float Unknown46;
				float Unknown47;
				float Unknown48;
				float Unknown49;
				float Unknown50;
				float Unknown51;
				float Unknown52;
				float Unknown53;
				float Unknown54;
				float Unknown55;
				float Unknown56;
				float Unknown57;
				float Unknown58;
				float Unknown59;
				float Unknown60;
				float Unknown61;
				float Unknown62;
				float Unknown63;
				float Unknown64;
				float Unknown65;
				float Unknown66;
				float MaximumRandomSpawnBias;
				float TeleporterRechargeTime;
				float GrenadeDangerWeight;
				float GrenadeDangerInnerRadius;
				float GrenadeDangerOuterRadius;
				float GrenadeDangerLeadTime;
				float VehicleDangerMinimumSpeed;
				float VehicleDangerWeight;
				float VehicleDangerRadius;
				float VehicleDangerLeadTime;
				float VehicleNearbyPlayerDistance;
				TagReference HillShader;
				float Unknown67;
				float Unknown68;
				float Unknown69;
				float Unknown70;
				TagReference BombExplodeEffect;
				TagReference Unknown71;
				TagReference BombExplodeDamageEffect;
				TagReference BombDefuseEffect;
				TagReference CursorImpactEffect;
				StringID BombDefusalString;
				StringID BlockedTeleporterString;
				int32_t Unknown72;
				StringID Unknown73;
				StringID SpawnAllowedDefaultRespawnString;
				StringID SpawnAtPlayerLookingAtSelfString;
				StringID SpawnAtPlayerLookingAtTargetString;
				StringID SpawnAtPlayerLookingAtPotentialTargetString;
				StringID SpawnAtTerritoryAllowedLookingAtTargetString;
				StringID SpawnAtTerritoryAllowedLookingAtPotentialTargetString;
				StringID PlayerOutOfLivesString;
				StringID InvalidSpawnTargetString;
				StringID TargettedPlayerEnemiesNearbyString;
				StringID TargettedPlayerUnfriendlyTeamString;
				StringID TargettedPlayerIsDeadString;
				StringID TargettedPlayerInCombatString;
				StringID TargettedPlayerTooFarFromOwnedFlagString;
				StringID NoAvailableNetpointsString;
				StringID NetpointContestedString;

				struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameWeapon
				{
					TagReference Weapon;
					float Unknown1;
					float Unknown2;
					float Unknown3;
					float Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameWeapon, 0x20);

				struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameVehicle
				{
					TagReference Vehicle;
					float Unknown1;
					float Unknown2;
					float Unknown3;
					float Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameVehicle, 0x20);

				struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameProjectile
				{
					TagReference Projectile;
					float Unknown1;
					float Unknown2;
					float Unknown3;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameProjectile, 0x1C);

				struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameEquipment
				{
					TagReference Equipment;
					float Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::MultiplayerConstant::GameEquipment, 0x14);
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::MultiplayerConstant, 0x220);

			struct MultiplayerGlobals::Runtime::StateResponse
			{
				enum class Flags : uint16_t;
				enum class State : int16_t;

				MultiplayerGlobals::Runtime::StateResponse::Flags Flags : 16;
				int16_t Unknown1;
				MultiplayerGlobals::Runtime::StateResponse::State State : 16;
				int16_t Unknown2;
				StringID FreeForAllMessage;
				StringID TeamMessage;
				TagReference Unknown3;
				uint32_t Unknown4;

				enum class MultiplayerGlobals::Runtime::StateResponse::Flags : uint16_t
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

				enum class MultiplayerGlobals::Runtime::StateResponse::State : int16_t
				{
					WaitingForSpaceToClear,
					Observing,
					RespawningSoon,
					SittingOut,
					OutOfLives,
					PlayingWinning,
					PlayingTied,
					PlayingLosing,
					GameOverWon,
					GameOverTied,
					GameOverLost,
					GameOverTied2,
					YouHaveFlag,
					EnemyHasFlag,
					FlagNotHome,
					CarryingOddball,
					YouAreJuggernaut,
					YouControlHill,
					SwitchingSidesSoon,
					PlayerRecentlyStarted,
					YouHaveBomb,
					FlagContested,
					BombContested,
					LimitedLivesMultiple,
					LimitedLivesSingle,
					LimitedLivesFinal,
					PlayingWinningUnlimited,
					PlayingTiedUnlimited,
					PlayingLosingUnlimited
				};
			};
			TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime::StateResponse, 0x24);
		};
		TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals::Runtime, 0x2A8);
	};
	TAG_STRUCT_SIZE_ASSERT(struct MultiplayerGlobals, 0x18);
}

