#pragma once
#include "Item.hpp"

namespace Blam
{
	namespace Tags
	{
		enum class WeaponFlags : int32_t
		{
			None,
			VerticalHeatDisplay = 1 << 0,
			MutuallyExclusiveTriggers = 1 << 1,
			AttacksAutomaticallyOnBump = 1 << 2,
			MustBeReadied = 1 << 3,
			DoesNotCountTowardsMaximum = 1 << 4,
			AimAssistsOnlyWhenZoomed = 1 << 5,
			PreventsGrenadeThrowing = 1 << 6,
			MustBePickedUp = 1 << 7,
			HoldsTriggersWhenDropped = 1 << 8,
			PreventsMeleeAttack = 1 << 9,
			DetonatesWhenDropped = 1 << 10,
			CannotFireAtMaximumAge = 1 << 11,
			SecondaryTriggerOverridesGrenades = 1 << 12,
			DoesNotDepowerActiveCamoInMultiplayer = 1 << 13,
			EnabledIntegratedNightVision = 1 << 14,
			AIsUseWeaponMeleeDamage = 1 << 15,
			ForcesNoBinoculars = 1 << 16,
			LoopFirstPersonFiringAnimation = 1 << 17,
			PreventsSprinting = 1 << 18,
			CannotFireWhileBoosting = 1 << 19,
			PreventsDriving = 1 << 20,
			ThirdPersonCamera = 1 << 21,
			CanBeDualWielded = 1 << 22,
			CanOnlyBeDualWielded = 1 << 23,
			MeleeOnly = 1 << 24,
			CannotFireIfParentIsDead = 1 << 25,
			WeaponAgesWithEachKill = 1 << 26,
			WeaponUsesOldDualFireErrorCode = 1 << 27,
			PrimaryTriggerMeleeAttacks = 1 << 28,
			CannotBeUsedByPlayer = 1 << 29,
			Bit30 = 1 << 30,
			Bit31 = 1 << 31
		};
		TAG_STRUCT_SIZE_ASSERT(WeaponFlags, 0x4);

		struct Weapon : Item, Tag<'weap'>
		{
			struct TargetTracking;
			struct FirstPerson;
			struct PredictedResource;
			struct Magazine;
			struct Trigger;
			struct Barrel;

			WeaponFlags WeaponFlags : 32;
			uint32_t MoreFlags;
			int32_t Unknown6;
			int16_t SecondaryTriggerMode;
			int16_t MaximumAlternateShotsLoaded;
			float TurnOnTime;
			float ReadyTime;
			TagReference ReadyEffect;
			TagReference ReadyDamageEffect;
			uint32_t HeatRecoveryThreshold;
			uint32_t OverheatedThreshold;
			uint32_t HeatDetonationThreshold;
			uint32_t HeatDetonationFraction;
			uint32_t HeatLossPerSecond;
			uint32_t HeatIllumination;
			uint32_t OverheatedHeatLossPerSecond;
			TagReference Overheated;
			TagReference OverheatedDamageEffect;
			TagReference Detonation;
			TagReference DetonationDamageEffect2;
			TagReference PlayerMeleeDamage;
			TagReference PlayerMeleeResponse;
			float DamagePyramidAnglesY;
			float DamagePyramidAnglesP;
			float DamagePyramidDepth;
			TagReference _1stHitDamage;
			TagReference _1stHitResponse;
			TagReference _2ndHitDamage;
			TagReference _2ndHitResponse;
			TagReference _3rdHitDamage;
			TagReference _3rdHitResponse;
			TagReference LungeMeleeDamage;
			TagReference LungeMeleeResponse;
			TagReference GunGunClangDamage;
			TagReference GunGunClangResponse;
			TagReference GunSwordClangDamage;
			TagReference GunSwordClangResponse;
			TagReference ClashEffect;
			int8_t MeleeDamageReportingType;
			int8_t Unknown7;
			int16_t MagnificationLevels;
			float MagnificationRangeMin;
			float MagnificationRangeMax;
			uint32_t MagnificationFlags;
			float WeaponSwitchReadySpeed0Default;
			float AutoaimAngle;
			float AutoaimRangeLong;
			float AutoaimRangeShort;
			float AutoaimSafeRadius;
			float MagnetismAngle;
			float MagnetismRangeLong;
			float MagnetismRangeShort;
			float MagnetismSafeRadius;
			float DeviationAngle;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			TagBlock<TargetTracking> TargetTracking2;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			int16_t MovementPenalized;
			int16_t Unknown18;
			float ForwardsMovementPenalty;
			float SidewaysMovementPenalty;
			float AiScariness;
			float WeaponPowerOnTime;
			float WeaponPowerOffTime;
			TagReference WeaponPowerOnEffect;
			TagReference WeaponPowerOffEffect;
			float AgeHeatRecoveryPenalty;
			float AgeRateOfFirePenalty;
			float AgeMisfireStart;
			float AgeMisfireChance;
			TagReference PickupSound;
			TagReference ZoomInSound;
			TagReference ZoomOutSound;
			uint32_t ActiveCamoDing;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			int32_t WeaponClass;
			int32_t WeaponName;
			int16_t MultiplayerWeaponType;
			int16_t WeaponType;
			int16_t TrackingType;
			int16_t Unknown22;
			int32_t SpecialHudVersion;
			int32_t SpecialHudIcon;
			TagBlock<FirstPerson> FirstPerson2;
			TagReference HudInterface;
			TagBlock<PredictedResource> PredictedResources;
			TagBlock<Magazine> Magazines;
			TagBlock<Trigger> Triggers;
			TagBlock<Barrel> Barrels;
			uint32_t Unknown23;
			uint32_t Unknown24;
			float MaximumMovementAcceleration;
			float MaximumMovementVelocity;
			float MaximumTurningAcceleration;
			float MaximumTurningVelocity;
			TagReference DeployedVehicle;
			TagReference DeployedWeapon;
			TagReference AgeModel;
			TagReference AgeWeapon;
			TagReference AgedMaterialEffects;
			float HammerAgePerUseMp;
			float HammerAgePerUseSp;
			float FirstPersonWeaponOffsetI;
			float FirstPersonWeaponOffsetJ;
			float FirstPersonWeaponOffsetK;
			float FirstPersonScopeSizeI;
			float FirstPersonScopeSizeJ;
			float ThirdPersonPitchBoundsMin;
			float ThirdPersonPitchBoundsMax;
			float ZoomTransitionTime;
			float MeleeWeaponDelay;
			float ReadyAnimationDuration;
			int32_t WeaponHolsterMarker;

			struct TargetTracking
			{
				struct TrackingType;

				TagBlock<TrackingType> TrackingTypes;
				float AcquireTime;
				float GraceTime;
				float DecayTime;
				TagReference TrackingSound;
				TagReference LockedSound;

				struct TrackingType
				{
					int32_t Type;
				};
				TAG_STRUCT_SIZE_ASSERT(TrackingType, 0x4);
			};
			TAG_STRUCT_SIZE_ASSERT(TargetTracking, 0x38);

			struct FirstPerson
			{
				TagReference FirstPersonModel;
				TagReference FirstPersonAnimations;
			};
			TAG_STRUCT_SIZE_ASSERT(FirstPerson, 0x20);

			struct PredictedResource
			{
				int16_t Type;
				int16_t ResourceIndex;
				int32_t TagIndex;
			};
			TAG_STRUCT_SIZE_ASSERT(PredictedResource, 0x8);

			struct Magazine
			{
				struct MagazineEquipment;

				uint32_t Flags;
				int16_t RoundsRecharged;
				int16_t RoundsTotalInitial;
				int16_t RoundsTotalMaximum;
				int16_t RoundsTotalLoadedMaximum;
				int16_t MaximumRoundsHeld;
				int16_t Unknown;
				float ReloadTime;
				int16_t RoundsReloaded;
				int16_t Unknown2;
				float ChamberTime;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				TagReference ReloadingEffect;
				TagReference ReloadingDamageEffect;
				TagReference ChamberingEffect;
				TagReference ChamberingDamageEffect;
				TagBlock<MagazineEquipment> MagazineEquipment2;

				struct MagazineEquipment
				{
					int16_t Rounds0ForMax;
					int16_t Unknown;
					TagReference Equipment;
				};
				TAG_STRUCT_SIZE_ASSERT(MagazineEquipment, 0x14);
			};
			TAG_STRUCT_SIZE_ASSERT(Magazine, 0x80);

			struct Trigger
			{
				uint32_t Flags;
				int16_t ButtonUsed;
				int16_t Behavior;
				int16_t PrimaryBarrel;
				int16_t SecondaryBarrel;
				int16_t Prediction;
				int16_t Unknown;
				float AutofireTime;
				float AutofireThrow;
				int16_t SecondaryAction;
				int16_t PrimaryAction;
				float ChargingTime;
				float ChargedTime;
				int16_t OverchargeAction;
				uint16_t ChargeFlags;
				float ChargedIllumination;
				float SpewTime;
				TagReference ChargingEffect;
				TagReference ChargingDamageEffect;
				TagReference ChargingResponse;
				float ChargingAgeDegeneration;
				TagReference DischargingEffect;
				TagReference DischargingDamageEffect;
				float TargetTrackingDecayTime;
				uint32_t Unknown2;
				uint32_t Unknown3;
			};
			TAG_STRUCT_SIZE_ASSERT(Trigger, 0x90);

			struct Barrel
			{
				struct FiringPenaltyFunction;
				struct FiringCrouchedPenaltyFunction;
				struct MovingPenaltyFunction;
				struct TurningPenaltyFunction;
				struct DualFiringPenaltyFunction;
				struct DualFiringCrouchedPenaltyFunction;
				struct DualMovingPenaltyFunction;
				struct DualTurningPenaltyFunction;
				struct FirstPersonOffset;
				struct FiringEffects;

				uint32_t Flags;
				float RoundsPerSecondMin;
				float RoundsPerSecondMax;
				float AccelerationTime;
				float DecelerationTime;
				float BarrelSpinScale;
				float BlurredRateOfFire;
				int16_t ShotsPerFireMin;
				int16_t ShotsPerFireMax;
				float FireRecoveryTime;
				float SoftRecoveryFraction;
				int16_t Magazine;
				int16_t RoundsPerShot;
				int16_t MinimumRoundsLoaded;
				int16_t RoundsBetweenTracers;
				int32_t OptionalBarrelMarkerName;
				int16_t PredictionType;
				int16_t FiringNoise;
				float AccelerationTime2;
				float DecelerationTime2;
				float DamageErrorMin;
				float DamageErrorMax;
				float BaseTurningSpeed;
				float DynamicTurningSpeedMin;
				float DynamicTurningSpeedMax;
				float AccelerationTime3;
				float DecelerationTime3;
				float Unknown;
				float Unknown2;
				float MinimumError;
				float ErrorAngleMin;
				float ErrorAngleMax;
				float DualWieldDamageScale;
				int16_t DistributionFunction;
				int16_t ProjectilesPerShot;
				float DistributionAngle;
				float MinimumError2;
				float ErrorAngleMin2;
				float ErrorAngleMax2;
				float ReloadPenalty;
				float SwitchPenalty;
				float ZoomPenalty;
				float JumpPenalty;
				TagBlock<FiringPenaltyFunction> FiringPenaltyFunction2;
				TagBlock<FiringCrouchedPenaltyFunction> FiringCrouchedPenaltyFunction2;
				TagBlock<MovingPenaltyFunction> MovingPenaltyFunction2;
				TagBlock<TurningPenaltyFunction> TurningPenaltyFunction2;
				float ErrorAngleMaximumRotation;
				TagBlock<DualFiringPenaltyFunction> DualFiringPenaltyFunction2;
				TagBlock<DualFiringCrouchedPenaltyFunction> DualFiringCrouchedPenaltyFunction2;
				TagBlock<DualMovingPenaltyFunction> DualMovingPenaltyFunction2;
				TagBlock<DualTurningPenaltyFunction> DualTurningPenaltyFunction2;
				float DualErrorAngleMaximumRotation;
				TagBlock<FirstPersonOffset> FirstPersonOffsets;
				int8_t DamageReportingType;
				int8_t Unknown3;
				int16_t Unknown4;
				TagReference InitialProjectile;
				TagReference TrailingProjectile;
				TagReference DamageEffect;
				TagReference CrateProjectile;
				float CrateProjectileSpeed;
				float EjectionPortRecoveryTime;
				float IlluminationRecoveryTime;
				float HeatGeneratedPerRound;
				float AgeGeneratedPerRoundMp;
				float AgeGeneratedPerRoundSp;
				float OverloadTime;
				float AngleChangePerShotMin;
				float AngleChangePerShotMax;
				float AngleChangeAccelerationTime;
				float AngleChangeDecelerationTime;
				int16_t AngleChangeFunction;
				int16_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				float FiringEffectDecelerationTime;
				float Unknown8;
				float RateOfFireAccelerationTime;
				float RateOfFireDecelerationTime;
				float Unknown9;
				float BloomRateOfDecay;
				TagBlock<FiringEffects> FiringEffects;

				struct FiringPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(FiringPenaltyFunction, 0x14);

				struct FiringCrouchedPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(FiringCrouchedPenaltyFunction, 0x14);

				struct MovingPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(MovingPenaltyFunction, 0x14);

				struct TurningPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(TurningPenaltyFunction, 0x14);

				struct DualFiringPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(DualFiringPenaltyFunction, 0x14);

				struct DualFiringCrouchedPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(DualFiringCrouchedPenaltyFunction, 0x14);

				struct DualMovingPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(DualMovingPenaltyFunction, 0x14);

				struct DualTurningPenaltyFunction
				{
					DataReference<uint8_t> Function;
				};
				TAG_STRUCT_SIZE_ASSERT(DualTurningPenaltyFunction, 0x14);

				struct FirstPersonOffset
				{
					float FirstPersonOffsetX;
					float FirstPersonOffsetY;
					float FirstPersonOffsetZ;
				};
				TAG_STRUCT_SIZE_ASSERT(FirstPersonOffset, 0xC);

				struct FiringEffects
				{
					int16_t ShotCountLowerBound;
					int16_t ShotCountUpperBound;
					TagReference FiringEffect;
					TagReference MisfireEffect;
					TagReference EmptyEffect;
					TagReference UnknownEffect;
					TagReference FiringResponse;
					TagReference MisfireResponse;
					TagReference EmptyResponse;
					TagReference UnknownResponse;
					TagReference RiderFiringResponse;
					TagReference RiderMisfireResponse;
					TagReference RiderEmptyResponse;
					TagReference RiderUnknownResponse;
				};
				TAG_STRUCT_SIZE_ASSERT(struct FiringEffects, 0xC4);
			};
			TAG_STRUCT_SIZE_ASSERT(Barrel, 0x1AC);
		};
		TAG_STRUCT_SIZE_ASSERT(Weapon, 0x558);
	}
}
