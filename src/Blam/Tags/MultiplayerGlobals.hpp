#pragma once
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct MultiplayerGlobals : Tag<'mulg'>
		{
			struct Universal;
			struct Runtime;

			TagBlock<Universal> Universal2;
			TagBlock<Runtime> Runtime2;

			struct Universal
			{
				struct SpartanArmorCustomization;
				struct EliteArmorCustomization;
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
				TagBlock<SpartanArmorCustomization> SpartanArmorCustomization2;
				TagBlock<EliteArmorCustomization> EliteArmorCustomization2;
				TagBlock<GameEquipment> Equipment2;
				TagBlock<Unknown> Unknown2;
				TagReference MultiplayerStrings;
				TagReference SandboxUiStrings;
				TagReference SandboxUiProperties;
				TagBlock<GameVariantWeapon> GameVariantWeapons;
				TagBlock<GameVariantVehicle> GameVariantVehicles;
				TagBlock<GameVariantEquipment> GameVariantEquipment2;
				TagBlock<WeaponSet> WeaponSets;
				TagBlock<VehicleSet> VehicleSets;
				TagBlock<PodiumAnimation> PodiumAnimations;
				TagReference EngineSettings;

				struct SpartanArmorCustomization
				{
					struct Permutation;

					int32_t ArmorObjectRegion;
					int32_t BipedRegion;
					TagBlock<Permutation> Permutations;

					struct Permutation
					{
						int32_t Name;
						TagReference ThirdPersonArmorObject;
						TagReference FirstPersonArmorModel;
						int16_t Unknown;
						int16_t Unknown2;
						int32_t ParentAttachMarker;
						int32_t ChildAttachMarker;
					};
					TAG_STRUCT_SIZE_ASSERT(Permutation, 0x30);
				};
				TAG_STRUCT_SIZE_ASSERT(SpartanArmorCustomization, 0x14);

				struct EliteArmorCustomization
				{
					struct Permutation;

					int32_t PieceRegion;
					int32_t CharacterRegion;
					TagBlock<Permutation> Permutations;

					struct Permutation
					{
						int32_t Name;
						TagReference ThirdPersonArmorObject;
						TagReference FirstPersonArmorModel;
						int16_t Unknown;
						int16_t Unknown2;
						int32_t ParentAttachMarker;
						int32_t ChildAttachMarker;
					};
					TAG_STRUCT_SIZE_ASSERT(Permutation, 0x30);
				};
				TAG_STRUCT_SIZE_ASSERT(EliteArmorCustomization, 0x14);

				struct GameEquipment
				{
					int32_t Name;
					TagReference Equipment;
					int16_t Unknown;
					int16_t Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(GameEquipment, 0x18);

				struct Unknown
				{
					int32_t Unknown1;
					int32_t Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown, 0x8);

				struct GameVariantWeapon
				{
					int32_t Name;
					float RandomChance;
					TagReference Weapon;
				};
				TAG_STRUCT_SIZE_ASSERT(GameVariantWeapon, 0x18);

				struct GameVariantVehicle
				{
					int32_t Name;
					TagReference Vehicle;
				};
				TAG_STRUCT_SIZE_ASSERT(GameVariantVehicle, 0x14);

				struct GameVariantEquipment
				{
					int32_t Name;
					TagReference Grenade;
				};
				TAG_STRUCT_SIZE_ASSERT(GameVariantEquipment, 0x14);

				struct WeaponSet
				{
					struct Substitution;

					int32_t Name;
					TagBlock<Substitution> Substitutions;

					struct Substitution
					{
						int32_t OriginalWeapon;
						int32_t SubstitutedWeapon;
					};
					TAG_STRUCT_SIZE_ASSERT(Substitution, 0x8);
				};
				TAG_STRUCT_SIZE_ASSERT(WeaponSet, 0x10);

				struct VehicleSet
				{
					struct Substitution;

					int32_t Name;
					TagBlock<Substitution> Substitutions;

					struct Substitution
					{
						int32_t OriginalVehicle;
						int32_t SubstitutedVehicle;
					};
					TAG_STRUCT_SIZE_ASSERT(Substitution, 0x8);
				};
				TAG_STRUCT_SIZE_ASSERT(VehicleSet, 0x10);

				struct PodiumAnimation
				{
					struct Stance;
					struct Alternates;

					TagReference AnimationGraph;
					int32_t DefaultUnarmed;
					int32_t DefaultArmed;
					TagBlock<Stance> Stances;
					TagBlock<Alternates> Alternates2;

					struct Stance
					{
						char Name[32];
						int32_t BaseAnimation;
						int32_t LoopAnimation;
						int32_t UnarmedTransition;
						int32_t ArmedTransition;
						float CameraDistanceOffset;
					};
					TAG_STRUCT_SIZE_ASSERT(Stance, 0x34);

					struct Alternates
					{
						char Name[32];
						int32_t InAnimation;
						int32_t LoopAnimation;
						int32_t OutAnimation;
						int32_t Weapon;
						TagReference CustomPrimaryWeapon;
						TagReference CustomSecondaryWeapon;
					};
					TAG_STRUCT_SIZE_ASSERT(Alternates, 0x50);
				};
				TAG_STRUCT_SIZE_ASSERT(PodiumAnimation, 0x30);
			};
			TAG_STRUCT_SIZE_ASSERT(Universal, 0xD8);

			struct Runtime
			{
				struct SoundReference;
				struct LoopingSoundReference;
				struct UnknownEvent;
				struct GeneralEvent;
				struct FlavorEvent;
				struct SlayerEvent;
				struct CtfEvent;
				struct OddballEvent;
				struct KingOfTheHillEvent;
				struct VipEvent;
				struct JuggernautEvent;
				struct TerritoriesEvent;
				struct AssaultEvent;
				struct InfectionEvent;
				struct MultiplayerConstant;
				struct StateRespons;

				TagReference SandboxEditorUnit;
				TagReference SandboxEditorObject;
				TagReference Flag;
				TagReference Ball;
				TagReference Bomb;
				TagReference VipZone;
				TagReference InGameStrings;
				TagReference Unknown;
				TagReference Unknown2;
				TagReference Unknown3;
				TagReference Unknown4;
				TagReference Unknown5;
				TagBlock<SoundReference> Sounds;
				TagBlock<LoopingSoundReference> LoopingSounds;
				TagBlock<UnknownEvent> UnknownEvents;
				TagBlock<GeneralEvent> GeneralEvents;
				TagBlock<FlavorEvent> FlavorEvents;
				TagBlock<SlayerEvent> SlayerEvents;
				TagBlock<CtfEvent> CtfEvents;
				TagBlock<OddballEvent> OddballEvents;
				TagBlock<KingOfTheHillEvent> KingOfTheHillEvents;
				TagBlock<VipEvent> VipEvents;
				TagBlock<JuggernautEvent> JuggernautEvents;
				TagBlock<TerritoriesEvent> TerritoriesEvents;
				TagBlock<AssaultEvent> AssaultEvents;
				TagBlock<InfectionEvent> InfectionEvents;
				int32_t DefaultFragGrenadeCount;
				int32_t DefaultPlasmaGrenadeCount;
				TagBlock<MultiplayerConstant> MultiplayerConstants;
				TagBlock<StateRespons> StateResponses;
				TagReference ScoreboardEmblemBitmap;
				TagReference ScoreboardDeadEmblemBitmap;
				TagReference DefaultShapeShader;
				TagReference Unknown6;
				TagReference CtfIntroUi;
				TagReference SlayerIntroUi;
				TagReference OddballIntroUi;
				TagReference KingOfTheHillIntroUi;
				TagReference SandboxIntroUi;
				TagReference VipIntroUi;
				TagReference JuggernautIntroUi;
				TagReference TerritoriesIntroUi;
				TagReference AssaultIntroUi;
				TagReference InfectionIntroUi;
				TagReference MenuMusic1;
				TagReference MenuMusic2;
				TagReference MenuMusic3;
				TagReference Unknown7;

				struct SoundReference
				{
					TagReference Sound;
				};
				TAG_STRUCT_SIZE_ASSERT(SoundReference, 0x10);

				struct LoopingSoundReference
				{
					TagReference LoopingSound;
				};
				TAG_STRUCT_SIZE_ASSERT(LoopingSoundReference, 0x10);

				struct UnknownEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(UnknownEvent, 0x10C);

				struct GeneralEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(GeneralEvent, 0x10C);

				struct FlavorEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(FlavorEvent, 0x10C);

				struct SlayerEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(SlayerEvent, 0x10C);

				struct CtfEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(CtfEvent, 0x10C);

				struct OddballEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(OddballEvent, 0x10C);

				struct KingOfTheHillEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(KingOfTheHillEvent, 0x10C);

				struct VipEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(VipEvent, 0x10C);

				struct JuggernautEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(JuggernautEvent, 0x10C);

				struct TerritoriesEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(TerritoriesEvent, 0x10C);

				struct AssaultEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(AssaultEvent, 0x10C);

				struct InfectionEvent
				{
					uint16_t Flags;
					int16_t Type;
					int32_t Event;
					int16_t Audience;
					int16_t Unknown;
					int16_t Team;
					int16_t Unknown2;
					int32_t DisplayString;
					int32_t DisplayMedal;
					uint32_t Unknown3;
					uint32_t Unknown4;
					int16_t RequiredField;
					int16_t ExcludedAudience;
					int16_t RequiredField2;
					int16_t ExcludedAudience2;
					int32_t PrimaryString;
					int32_t PrimaryStringDuration;
					int32_t PluralDisplayString;
					float SoundDelayAnnouncerOnly;
					uint16_t SoundFlags;
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
				};
				TAG_STRUCT_SIZE_ASSERT(InfectionEvent, 0x10C);

				struct MultiplayerConstant
				{
					struct GameWeapon;
					struct GameVehicle;
					struct GameProjectile;
					struct GameEquipment;

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
					TagBlock<GameWeapon> Weapons;
					TagBlock<GameVehicle> Vehicles;
					TagBlock<GameProjectile> Projectiles;
					TagBlock<GameEquipment> Equipment2;
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
					int32_t BombDefusalString;
					int32_t BlockedTeleporterString;
					int32_t Unknown72;
					int32_t Unknown73;
					int32_t SpawnAllowedDefaultRespawnString;
					int32_t SpawnAtPlayerLookingAtSelfString;
					int32_t SpawnAtPlayerLookingAtTargetString;
					int32_t SpawnAtPlayerLookingAtPotentialTargetString;
					int32_t SpawnAtTerritoryAllowedLookingAtTargetString;
					int32_t SpawnAtTerritoryAllowedLookingAtPotentialTargetString;
					int32_t PlayerOutOfLivesString;
					int32_t InvalidSpawnTargetString;
					int32_t TargettedPlayerEnemiesNearbyString;
					int32_t TargettedPlayerUnfriendlyTeamString;
					int32_t TargettedPlayerIsDeadString;
					int32_t TargettedPlayerInCombatString;
					int32_t TargettedPlayerTooFarFromOwnedFlagString;
					int32_t NoAvailableNetpointsString;
					int32_t NetpointContestedString;

					struct GameWeapon
					{
						TagReference Weapon;
						float Unknown;
						float Unknown2;
						float Unknown3;
						float Unknown4;
					};
					TAG_STRUCT_SIZE_ASSERT(GameWeapon, 0x20);

					struct GameVehicle
					{
						TagReference Vehicle;
						float Unknown;
						float Unknown2;
						float Unknown3;
						float Unknown4;
					};
					TAG_STRUCT_SIZE_ASSERT(GameVehicle, 0x20);

					struct GameProjectile
					{
						TagReference Projectile;
						float Unknown;
						float Unknown2;
						float Unknown3;
					};
					TAG_STRUCT_SIZE_ASSERT(GameProjectile, 0x1C);

					struct GameEquipment
					{
						TagReference Equipment;
						float Unknown;
					};
					TAG_STRUCT_SIZE_ASSERT(GameEquipment, 0x14);
				};
				TAG_STRUCT_SIZE_ASSERT(MultiplayerConstant, 0x220);

				struct StateRespons
				{
					uint16_t Flags;
					int16_t Unknown;
					int16_t State;
					int16_t Unknown2;
					int32_t FreeForAllMessage;
					int32_t TeamMessage;
					TagReference Unknown3;
					uint32_t Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(StateRespons, 0x24);
			};
			TAG_STRUCT_SIZE_ASSERT(Runtime, 0x2A8);
		};
		TAG_STRUCT_SIZE_ASSERT(MultiplayerGlobals, 0x18);
	}
}
