#pragma once
#include "../Tags.hpp"
#include "../../Math/Angle.hpp"
#include "../../Math/Bounds.hpp"
#include "../../Math/RealEulerAngles2D.hpp"
#include "../../Math/RealPoint3D.hpp"
#include "../../Math/RealVector2D.hpp"
#include "../../Math/RealVector3D.hpp"
#include "../../Text/StringID.hpp"
#include "../Objects/Damage.hpp"
#include "../Sounds/Noise.hpp"
#include "Item.hpp"

namespace Blam::Tags::Items
{
	using Blam::Math::Angle;
	using Blam::Math::Bounds;
	using Blam::Math::RealEulerAngles2D;
	using Blam::Math::RealPoint3D;
	using Blam::Math::RealVector2D;
	using Blam::Math::RealVector3D;
	using Blam::Tags::Tag;
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;
	using Blam::Tags::Objects::DamageReportingType;
	using Blam::Tags::Sounds::NoiseLevel;

	struct Weapon : TagGroup<'weap'>
	{
		Item Item;

		enum class Flags1 : int32_t;
		enum class Flags2 : int32_t;
		enum class SecondaryTriggerMode : int16_t;
		enum class MagnificationFlags : int32_t;
		struct TargetTracking;
		enum class MovementPenalty : int16_t;
		enum class MultiplayerType : int16_t;
		enum class Type : int16_t;
		enum class TrackingType : int16_t;
		enum class SpecialHUDType : int32_t;
		struct FirstPersonResources;
		struct PredictedResource;
		struct Magazine;
		struct Trigger;
		struct Barrel;

		Weapon::Flags1 WeaponFlags1 : 32;
		Weapon::Flags2 WeaponFlags2 : 32;
		StringID Unknown6;
		Weapon::SecondaryTriggerMode SecondaryTriggerMode : 16;
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
		RealEulerAngles2D DamagePyramidAngles;
		float DamagePyramidDepth;
		TagReference FirstHitDamage;
		TagReference FirstHitResponse;
		TagReference SecondHitDamage;
		TagReference SecondHitResponse;
		TagReference ThirdHitDamage;
		TagReference ThirdHitResponse;
		TagReference LungeMeleeDamage;
		TagReference LungeMeleeResponse;
		TagReference GunOnGunClangDamage;
		TagReference GunOnGunClangResponse;
		TagReference GunOnSwordClangDamage;
		TagReference GunOnSwordClangResponse;
		TagReference ClashEffect;
		DamageReportingType MeleeDamageReportingType : 8;
		int8_t Unknown7;
		int16_t MagnificationLevels;
		Bounds<float> MagnificationRange;
		Weapon::MagnificationFlags MagnificationFlags : 32;
		float WeaponSwitchReadySpeed0Default;
		Angle AutoaimAngle;
		float AutoaimRangeLong;
		float AutoaimRangeShort;
		float AutoaimSafeRadius;
		Angle MagnetismAngle;
		float MagnetismRangeLong;
		float MagnetismRangeShort;
		float MagnetismSafeRadius;
		Angle DeviationAngle;
		uint32_t Unknown8;
		uint32_t Unknown9;
		uint32_t Unknown10;
		uint32_t Unknown11;
		uint32_t Unknown12;
		uint32_t Unknown13;
		TagBlock<Weapon::TargetTracking> TargetTracking;
		uint32_t Unknown14;
		uint32_t Unknown15;
		uint32_t Unknown16;
		uint32_t Unknown17;
		Weapon::MovementPenalty MovementPenalized : 16;
		int16_t Unknown18;
		float ForwardsMovementPenalty;
		float SidewaysMovementPenalty;
		float AIScariness;
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
		StringID WeaponClass;
		StringID WeaponName;
		Weapon::MultiplayerType MultiplayerWeaponType : 16;
		Weapon::Type WeaponType : 16;
		Weapon::TrackingType TrackingType : 16;
		int16_t Unknown22;
		Weapon::SpecialHUDType SpecialHUDType : 32;
		int32_t SpecialHUDIcon;
		TagBlock<Weapon::FirstPersonResources> FirstPersonResources;
		TagReference HUDInterface;
		TagBlock<Weapon::PredictedResource> PredictedResources;
		TagBlock<Weapon::Magazine> Magazines;
		TagBlock<Weapon::Trigger> Triggers;
		TagBlock<Weapon::Barrel> Barrels;
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
		float HammerAgePerUseMP;
		float HammerAgePerUseSP;
		RealVector3D FirstPersonWeaponOffset;
		RealVector2D FirstPersonScopeSize;
		Bounds<float> ThirdPersonPitchBounds;
		float ZoomTransitionTime;
		float MeleeWeaponDelay;
		float ReadyAnimationDuration;
		int32_t WeaponHolsterMarker;

		enum class Weapon::Flags1 : int32_t
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
			CannotBeUsedByPlayer = 1 << 29
		};

		enum class Weapon::Flags2 : int32_t
		{
			None
		};

		struct Weapon::TargetTracking
		{
			struct TrackingType;

			TagBlock<Weapon::TargetTracking::TrackingType> TrackingTypes;
			float AcquireTime;
			float GraceTime;
			float DecayTime;
			TagReference TrackingSound;
			TagReference LockedSound;

			struct Weapon::TargetTracking::TrackingType
			{
				StringID Type;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Weapon::TargetTracking::TrackingType, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Weapon::TargetTracking, 0x38);

		enum class Weapon::MovementPenalty : int16_t
		{
			Always,
			WhenZoomed,
			WhenZoomedOrReloading
		};

		enum class Weapon::MultiplayerType : int16_t
		{
			None,
			CTFFlag,
			OddballBall,
			HeadhunterHead,
			JuggernautPowerup
		};

		enum class Weapon::Type : int16_t
		{
			Undefined,
			Shotgun,
			Needler,
			PlasmaPistol,
			PlasmaRifle,
			RocketLauncher,
			EnergySword,
			SpartanLaser,
			AssaultRifle,
			BattleRifle,
			DMR,
			Magnum,
			SniperRifle,
			SMG
		};

		enum class Weapon::TrackingType : int16_t
		{
			NoTracking,
			HumanTracking,
			PlasmaTracking
		};

		enum class Weapon::SpecialHUDType : int32_t
		{
			NoOutline = -28,
			Default = 0,
			Ammo,
			Damage,
			Accuracy,
			RateOfFire,
			Range,
			Power
		};

		struct Weapon::FirstPersonResources
		{
			TagReference Model;
			TagReference Animations;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Weapon::FirstPersonResources, 0x20);

		struct Weapon::PredictedResource
		{
			int16_t Type;
			int16_t ResourceIndex;
			int32_t TagIndex;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Weapon::PredictedResource, 0x8);

		struct Weapon::Magazine
		{
			enum class Flags : int32_t;
			struct Equipment;

			Weapon::Magazine::Flags Flags : 32;
			int16_t RoundsRecharged;
			int16_t RoundsTotalInitial;
			int16_t RoundsTotalMaximum;
			int16_t RoundsTotalLoadedMaximum;
			int16_t MaximumRoundsHeld;
			int16_t Unknown1;
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
			TagBlock<Weapon::Magazine::Equipment> Equipment;

			enum class Weapon::Magazine::Flags : int32_t
			{
				None,
				WastesRoundsWhenReloaded = 1 << 0,
				EveryRoundMustBeChambered = 1 << 1
			};

			struct Weapon::Magazine::Equipment
			{
				int16_t Rounds;
				int16_t Unknown;
				TagReference Reference;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Weapon::Magazine::Equipment, 0x14);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Weapon::Magazine, 0x80);

		struct Weapon::Trigger
		{
			enum class Flags : int32_t;
			enum class Button : int16_t;
			enum class Behavior : int16_t;
			enum class Prediction : int16_t;
			enum class Action : int16_t;
			enum class OverchargeAction : int16_t;
			enum class ChargeFlags : uint16_t;

			Weapon::Trigger::Flags Flags : 32;
			Weapon::Trigger::Button ButtonUsed : 16;
			Weapon::Trigger::Behavior Behavior : 16;
			int16_t PrimaryBarrel;
			int16_t SecondaryBarrel;
			Weapon::Trigger::Prediction Prediction : 16;
			int16_t Unknown;
			float AutofireTime;
			float AutofireThrow;
			Weapon::Trigger::Action SecondaryAction : 16;
			Weapon::Trigger::Action PrimaryAction : 16;
			float ChargingTime;
			float ChargedTime;
			Weapon::Trigger::OverchargeAction OverchargeAction : 16;
			Weapon::Trigger::ChargeFlags ChargeFlags : 16;
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

			enum class Weapon::Trigger::Flags : int32_t
			{
				None,
				AutofireSingleActionOnly = 1 << 0
			};

			enum class Weapon::Trigger::Button : int16_t
			{
				RightTrigger,
				LeftTrigger,
				MeleeAttack,
				AutomatedFire,
				RightBumper
			};

			enum class Weapon::Trigger::Behavior : int16_t
			{
				Spew,
				Latch,
				LatchAutofire,
				Charge,
				LatchZoom,
				LatchRocketLauncher,
				SpewCharge
			};

			enum class Weapon::Trigger::Prediction : int16_t
			{
				None,
				Spew,
				Charge
			};

			enum class Weapon::Trigger::Action : int16_t
			{
				Fire,
				Charge,
				Track,
				FireOther
			};

			enum class OverchargeAction : int16_t
			{
				None,
				Explode,
				Discharge
			};

			enum class ChargeFlags : uint16_t
			{
				None
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct Trigger, 0x90);

		struct Barrel
		{
			enum class Flags : int32_t;
			enum class PredictionType : int16_t;
			enum class DistributionFunction : int16_t;
			struct Function;
			struct FirstPersonOffset;
			struct FiringEffects;

			Weapon::Barrel::Flags Flags : 32;
			Bounds<float> RoundsPerSecondBounds;
			float AccelerationTime;
			float DecelerationTime;
			float BarrelSpinScale;
			float BlurredRateOfFire;
			Bounds<int16_t> ShotsPerFireBounds;
			float FireRecoveryTime;
			float SoftRecoveryFraction;
			int16_t Magazine;
			int16_t RoundsPerShot;
			int16_t MinimumRoundsLoaded;
			int16_t RoundsBetweenTracers;
			StringID OptionalBarrelMarkerName;
			Weapon::Barrel::PredictionType PredictionType : 16;
			NoiseLevel FiringNoise : 16;
			float ErrorAccelerationTime;
			float ErrorDecelerationTime;
			Bounds<float> DamageErrorBounds;
			Angle BaseTurningSpeed;
			Bounds<Angle> DynamicTurningSpeedBounds;
			float DualErrorAccelerationTime;
			float DualErrorDecelerationTime;
			float Unknown1;
			float Unknown2;
			Angle MinimumErrorAngle;
			Bounds<Angle> ErrorAngleBounds;
			float DualWieldDamageScale;
			Weapon::Barrel::DistributionFunction DistributionFunction;
			int16_t ProjectilesPerShot;
			Angle DistributionAngle;
			Angle MinimumProjectileErrorAngle;
			Bounds<Angle> ProjectileErrorAngleBounds;
			float ReloadPenalty;
			float SwitchPenalty;
			float ZoomPenalty;
			float JumpPenalty;
			TagBlock<Weapon::Barrel::Function> FiringPenaltyFunction;
			TagBlock<Weapon::Barrel::Function> FiringCrouchedPenaltyFunction;
			TagBlock<Weapon::Barrel::Function> MovingPenaltyFunction;
			TagBlock<Weapon::Barrel::Function> TurningPenaltyFunction;
			float ErrorAngleMaximumRotation;
			TagBlock<Weapon::Barrel::Function> DualFiringPenaltyFunction;
			TagBlock<Weapon::Barrel::Function> DualFiringCrouchedPenaltyFunction;
			TagBlock<Weapon::Barrel::Function> DualMovingPenaltyFunction;
			TagBlock<Weapon::Barrel::Function> DualTurningPenaltyFunction;
			float DualErrorAngleMaximumRotation;
			TagBlock<Weapon::Barrel::FirstPersonOffset> FirstPersonOffsets;
			DamageReportingType DamageReportingType : 8;
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
			float AgeGeneratedPerRoundMP;
			float AgeGeneratedPerRoundSP;
			float OverloadTime;
			Bounds<float> AngleChangePerShotBounds;
			float AngleChangeAccelerationTime;
			float AngleChangeDecelerationTime;
			int16_t AngleChangeFunction;
			int16_t Unknown5;
			float Unknown6;
			float Unknown7;
			float FiringEffectDecelerationTime;
			float Unknown8;
			float RateOfFireAccelerationTime;
			float RateOfFireDecelerationTime;
			float Unknown9;
			float BloomRateOfDecay;
			TagBlock<Weapon::Barrel::FiringEffects> FiringEffects;

			struct Weapon::Barrel::Function
			{
				TagData<uint8_t> Data;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Weapon::Barrel::Function, 0x14);

			struct Weapon::Barrel::FirstPersonOffset
			{
				RealPoint3D Offset;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Weapon::Barrel::FirstPersonOffset, 0xC);

			struct Weapon::Barrel::FiringEffects
			{
				Bounds<int16_t> ShotCountBounds;
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
			TAG_STRUCT_SIZE_ASSERT(struct Weapon::Barrel::FiringEffects, 0xC4);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Weapon::Barrel, 0x1AC);
	};
	TAG_STRUCT_SIZE_ASSERT(struct Weapon, 0x558);
}
