#include "Blam\Tags\Items\DefinitionWeapon.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Items;
using namespace Blam::Tags;
using namespace Definitions;

namespace Blam::Tags
{
	// Sounds/Noise
	const EnumDefinition SoundsNoiseLevelEnum =
	{
		"NoiseLevel", EnumType::Short,
		{
			{ "SentinelRPG", (short)NoiseLevel::Silent },
			{ "Teleporter", (short)NoiseLevel::Medium },
			{ "Tripmine", (short)NoiseLevel::Loud },
			{ "DMR", (short)NoiseLevel::Shout },
			{ "DamageCount", (short)NoiseLevel::Quiet }
		}
	};

	// Objects/Damage
	const EnumDefinition DamageReportingTypeEnum =
	{
		"DamageReportingType", EnumType::Char,
		{
			{ "GuardiansUnknown", (char)DamageReportingType::GuardiansUnknown },
			{ "Guardians", (char)DamageReportingType::Guardians },
			{ "FallingDamage", (char)DamageReportingType::FallingDamage },
			{ "GenericCollision", (char)DamageReportingType::GenericCollision },
			{ "ArmorLockCrush", (char)DamageReportingType::ArmorLockCrush },
			{ "GenericMelee", (char)DamageReportingType::GenericMelee },
			{ "GenericExplosion", (char)DamageReportingType::GenericExplosion },
			{ "Magnum", (char)DamageReportingType::Magnum },
			{ "PlasmaPistol", (char)DamageReportingType::PlasmaPistol },
			{ "Needler", (char)DamageReportingType::Needler },
			{ "Mauler", (char)DamageReportingType::Mauler },
			{ "SMG", (char)DamageReportingType::SMG },
			{ "PlasmaRifle", (char)DamageReportingType::PlasmaRifle },
			{ "BattleRifle", (char)DamageReportingType::BattleRifle },
			{ "CovenantCarbine", (char)DamageReportingType::CovenantCarbine },
			{ "Shotgun", (char)DamageReportingType::Shotgun },
			{ "SniperRifle", (char)DamageReportingType::SniperRifle },
			{ "BeamRifle", (char)DamageReportingType::BeamRifle },
			{ "AssaultRifle", (char)DamageReportingType::AssaultRifle },
			{ "Spiker", (char)DamageReportingType::Spiker },
			{ "FuelRodCannon", (char)DamageReportingType::FuelRodCannon },
			{ "MissilePod", (char)DamageReportingType::MissilePod },
			{ "RocketLauncher", (char)DamageReportingType::RocketLauncher },
			{ "SpartanLaser", (char)DamageReportingType::SpartanLaser },
			{ "BruteShot", (char)DamageReportingType::BruteShot },
			{ "Flamethrower", (char)DamageReportingType::Flamethrower },
			{ "SentinelGun", (char)DamageReportingType::SentinelGun },
			{ "EnergySword", (char)DamageReportingType::EnergySword },
			{ "GravityHammer", (char)DamageReportingType::GravityHammer },
			{ "FragGrenade", (char)DamageReportingType::FragGrenade },
			{ "PlasmaGrenade", (char)DamageReportingType::PlasmaGrenade },
			{ "SpikeGrenade", (char)DamageReportingType::SpikeGrenade },
			{ "FirebombGrenade", (char)DamageReportingType::FirebombGrenade },
			{ "Flag", (char)DamageReportingType::Flag },
			{ "Bomb", (char)DamageReportingType::Bomb },
			{ "BombExplosion", (char)DamageReportingType::BombExplosion },
			{ "Ball", (char)DamageReportingType::Ball },
			{ "MachinegunTurret", (char)DamageReportingType::MachinegunTurret },
			{ "PlasmaCannon", (char)DamageReportingType::PlasmaCannon },
			{ "PlasmaMortar", (char)DamageReportingType::PlasmaMortar },
			{ "PlasmaTurret", (char)DamageReportingType::PlasmaTurret },
			{ "ShadeTurret", (char)DamageReportingType::ShadeTurret },
			{ "Banshee", (char)DamageReportingType::Banshee },
			{ "Ghost", (char)DamageReportingType::Ghost },
			{ "Mongoose", (char)DamageReportingType::Mongoose },
			{ "Scorpion", (char)DamageReportingType::Scorpion },
			{ "ScorpionGunner", (char)DamageReportingType::ScorpionGunner },
			{ "Spectre", (char)DamageReportingType::Spectre },
			{ "SpectreGunner", (char)DamageReportingType::SpectreGunner },
			{ "Warthog", (char)DamageReportingType::Warthog },
			{ "WarthogGunner", (char)DamageReportingType::WarthogGunner },
			{ "WarthogGaussTurret", (char)DamageReportingType::WarthogGaussTurret },
			{ "Wraith", (char)DamageReportingType::Wraith },
			{ "WraithGunner", (char)DamageReportingType::WraithGunner },
			{ "Tank", (char)DamageReportingType::Tank },
			{ "Chopper", (char)DamageReportingType::Chopper },
			{ "Hornet", (char)DamageReportingType::Hornet },
			{ "Mantis", (char)DamageReportingType::Mantis },
			{ "Prowler", (char)DamageReportingType::Prowler },
			{ "SentinelBeam", (char)DamageReportingType::SentinelBeam },
			{ "SentinelRPG", (char)DamageReportingType::SentinelRPG },
			{ "Teleporter", (char)DamageReportingType::Teleporter },
			{ "Tripmine", (char)DamageReportingType::Tripmine },
			{ "DMR", (char)DamageReportingType::DMR },
			{ "DamageCount", (char)DamageReportingType::DamageCount }
		}
	};

	const EnumDefinition Flags1Enum =
	{
		"Flags1", EnumType::Long,
		{
			{ "VerticalHeatDisplay", (long)Weapon::Flags1::VerticalHeatDisplay },
			{ "MutuallyExclusiveTriggers", (long)Weapon::Flags1::MutuallyExclusiveTriggers },
			{ "AttacksAutomaticallyOnBump", (long)Weapon::Flags1::AttacksAutomaticallyOnBump },
			{ "MustBeReadied", (long)Weapon::Flags1::MustBeReadied },
			{ "DoesNotCountTowardsMaximum", (long)Weapon::Flags1::DoesNotCountTowardsMaximum },
			{ "AimAssistsOnlyWhenZoomed", (long)Weapon::Flags1::AimAssistsOnlyWhenZoomed },
			{ "PreventsGrenadeThrowing", (long)Weapon::Flags1::PreventsGrenadeThrowing },
			{ "MustBePickedUp", (long)Weapon::Flags1::MustBePickedUp },
			{ "HoldsTriggersWhenDropped", (long)Weapon::Flags1::HoldsTriggersWhenDropped },
			{ "PreventsMeleeAttack", (long)Weapon::Flags1::PreventsMeleeAttack },
			{ "DetonatesWhenDropped", (long)Weapon::Flags1::DetonatesWhenDropped },
			{ "CannotFireAtMaximumAge", (long)Weapon::Flags1::CannotFireAtMaximumAge },
			{ "SecondaryTriggerOverridesGrenades", (long)Weapon::Flags1::SecondaryTriggerOverridesGrenades },
			{ "DoesNotDepowerActiveCamoInMultiplayer", (long)Weapon::Flags1::DoesNotDepowerActiveCamoInMultiplayer },
			{ "EnabledIntegratedNightVision", (long)Weapon::Flags1::EnabledIntegratedNightVision },
			{ "AIsUseWeaponMeleeDamage", (long)Weapon::Flags1::AIsUseWeaponMeleeDamage },
			{ "ForcesNoBinoculars", (long)Weapon::Flags1::ForcesNoBinoculars },
			{ "LoopFirstPersonFiringAnimation", (long)Weapon::Flags1::LoopFirstPersonFiringAnimation },
			{ "PreventsSprinting", (long)Weapon::Flags1::PreventsSprinting },
			{ "CannotFireWhileBoosting", (long)Weapon::Flags1::CannotFireWhileBoosting },
			{ "PreventsDriving", (long)Weapon::Flags1::PreventsDriving },
			{ "ThirdPersonCamera", (long)Weapon::Flags1::ThirdPersonCamera },
			{ "CanBeDualWielded", (long)Weapon::Flags1::CanBeDualWielded },
			{ "CanOnlyBeDualWielded", (long)Weapon::Flags1::CanOnlyBeDualWielded },
			{ "MeleeOnly", (long)Weapon::Flags1::MeleeOnly },
			{ "CannotFireIfParentIsDead", (long)Weapon::Flags1::CannotFireIfParentIsDead },
			{ "WeaponAgesWithEachKill", (long)Weapon::Flags1::WeaponAgesWithEachKill },
			{ "WeaponUsesOldDualFireErrorCode", (long)Weapon::Flags1::WeaponUsesOldDualFireErrorCode },
			{ "PrimaryTriggerMeleeAttacks", (long)Weapon::Flags1::PrimaryTriggerMeleeAttacks },
			{ "CannotBeUsedByPlayer", (long)Weapon::Flags1::CannotBeUsedByPlayer }
		}
	};

	const EnumDefinition Flags2Enum =
	{
		"Flags2", EnumType::DWord,
		{
		}
	};

	const EnumDefinition SecondaryTriggerModeEnum =
	{
		"SecondaryTriggerMode", EnumType::Short,
		{
		}
	};

	const EnumDefinition MagnificationFlagsEnum =
	{
		"MagnificationFlags", EnumType::Short,
		{
		}
	};

	const StructDefinition TrackingTypeStruct =
	{
		"TrackingType", sizeof(Weapon::TargetTracking::TrackingType),
		{
			{ FieldType::StringID, "Type" }
		}
	};

	const StructDefinition TargetTrackingStruct =
	{
		"TargetTracking", sizeof(Weapon::TargetTracking),
		{
			{ FieldType::Block, "TrackingTypes", &TrackingTypeStruct },
			{ FieldType::Real, "AcquireTime" },
			{ FieldType::Real, "GraceTime" },
			{ FieldType::Real, "DecayTime" },
			{ FieldType::TagReference, "TrackingSound" },
			{ FieldType::TagReference, "LockedSound" }
		}
	};

	const EnumDefinition MovementPenaltyEnum =
	{
		"MovementPenalty", EnumType::Short,
		{
			{ "Always", (short)Weapon::MovementPenalty::Always },
			{ "WhenZoomed", (short)Weapon::MovementPenalty::WhenZoomed },
			{ "WhenZoomedOrReloading", (short)Weapon::MovementPenalty::WhenZoomedOrReloading }
		}
	};

	const EnumDefinition MultiplayerTypeEnum =
	{
		"MultiplayerType", EnumType::Short,
		{
			{ "CTFFlag", (short)Weapon::MultiplayerType::CTFFlag },
			{ "OddballBall", (short)Weapon::MultiplayerType::OddballBall },
			{ "HeadhunterHead", (short)Weapon::MultiplayerType::HeadhunterHead },
			{ "JuggernautPowerup", (short)Weapon::MultiplayerType::JuggernautPowerup }
		}
	};

	const EnumDefinition TypeEnum =
	{
		"Type", EnumType::Short,
		{
			{ "Undefined", (short)Weapon::Type::Undefined },
			{ "Shotgun", (short)Weapon::Type::Shotgun },
			{ "Needler", (short)Weapon::Type::Needler },
			{ "PlasmaPistol", (short)Weapon::Type::PlasmaPistol },
			{ "PlasmaRifle", (short)Weapon::Type::PlasmaRifle },
			{ "RocketLauncher", (short)Weapon::Type::RocketLauncher },
			{ "EnergySword", (short)Weapon::Type::EnergySword },
			{ "SpartanLaser", (short)Weapon::Type::SpartanLaser },
			{ "AssaultRifle", (short)Weapon::Type::AssaultRifle },
			{ "BattleRifle", (short)Weapon::Type::BattleRifle },
			{ "DMR", (short)Weapon::Type::DMR },
			{ "Magnum", (short)Weapon::Type::Magnum },
			{ "SniperRifle", (short)Weapon::Type::SniperRifle },
			{ "SMG", (short)Weapon::Type::SMG }
		}
	};

	const EnumDefinition TrackingTypeEnum =
	{
		"TrackingType", EnumType::Short,
		{
			{ "NoTracking", (short)Weapon::MovementPenalty::Always },
			{ "HumanTracking", (short)Weapon::MovementPenalty::WhenZoomed },
			{ "PlasmaTracking", (short)Weapon::MovementPenalty::WhenZoomedOrReloading }
		}
	};

	const EnumDefinition SpecialHUDTypeEnum =
	{
		"SpecialHUDType", EnumType::DWord,
		{
			{ "NoOutline", (short)Weapon::Type::Undefined },
			{ "Default", (short)Weapon::Type::Shotgun },
			{ "Ammo", (short)Weapon::Type::Needler },
			{ "Damage", (short)Weapon::Type::PlasmaPistol },
			{ "Accuracy", (short)Weapon::Type::PlasmaRifle },
			{ "RateOfFire", (short)Weapon::Type::RocketLauncher },
			{ "Range", (short)Weapon::Type::EnergySword },
			{ "Power", (short)Weapon::Type::SpartanLaser }
		}
	};

	const StructDefinition FirstPersonResourcesStruct =
	{
		"FirstPersonResources", sizeof(Weapon::FirstPersonResources),
		{
			{ FieldType::TagReference, "Model" },
			{ FieldType::TagReference, "Animations" }
		}
	};

	const StructDefinition PredictedResourceStruct =
	{
		"PredictedResource", sizeof(Weapon::PredictedResource),
		{
			{ FieldType::ShortInteger, "Type" },
			{ FieldType::ShortInteger, "ResourceIndex" },
			{ FieldType::DwordInteger, "TagIndex" }
		}
	};

	const EnumDefinition MagazineFlagsEnum =
	{
		" MagazineFlags", EnumType::Long,
		{
			{ "WastesRoundsWhenReloaded", (long)Weapon::Magazine::Flags::WastesRoundsWhenReloaded },
			{ "EveryRoundMustBeChambered", (long)Weapon::Magazine::Flags::EveryRoundMustBeChambered }
		}
	};

	const StructDefinition MagazineEquipmentStruct =
	{
		"PredictedResource", sizeof(Weapon::Magazine::Equipment),
		{
			{ FieldType::ShortInteger, "Rounds" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::TagReference, "Reference" }
		}
	};

	const StructDefinition MagazineStruct =
	{
		"Magazine", sizeof(Weapon::Magazine),
		{
			{ FieldType::ShortEnum, "Flags", &MagazineFlagsEnum },
			{ FieldType::ShortInteger, "RoundsRecharged" },
			{ FieldType::ShortInteger, "RoundsTotalInitial" },
			{ FieldType::ShortInteger, "RoundsTotalMaximum" },
			{ FieldType::ShortInteger, "RoundsTotalLoadedMaximum" },
			{ FieldType::ShortInteger, "MaximumRoundsHeld" },
			{ FieldType::ShortInteger, "Unknown1" },
			{ FieldType::Real, "ReloadTime" },
			{ FieldType::ShortInteger, "RoundsReloaded" },
			{ FieldType::ShortInteger, "Unknown2" },
			{ FieldType::Real, "ChamberTime" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::TagReference, "ReloadingEffect" },
			{ FieldType::TagReference, "ReloadingDamageEffect" },
			{ FieldType::TagReference, "ChamberingEffect" },
			{ FieldType::TagReference, "ChamberingDamageEffect" },
			{ FieldType::Block, "Equipment", &MagazineEquipmentStruct }
		}
	};

	const EnumDefinition TriggerFlagsEnum =
	{
		"TriggerFlags", EnumType::Long,
		{
			{ "AutofireSingleActionOnly", (long)Weapon::Trigger::Flags::AutofireSingleActionOnly }
		}
	};

	const EnumDefinition TriggerButtonEnum =
	{
		"TriggerButton", EnumType::Short,
		{
			{ "RightTrigger", (short)Weapon::Trigger::Button::RightTrigger },
			{ "LeftTrigger", (short)Weapon::Trigger::Button::LeftTrigger },
			{ "MeleeAttack", (short)Weapon::Trigger::Button::MeleeAttack },
			{ "AutomatedFire", (short)Weapon::Trigger::Button::AutomatedFire },
			{ "RightBumper", (short)Weapon::Trigger::Button::RightBumper }
		}
	};

	const EnumDefinition TriggerBehaviorEnum =
	{
		"TriggerBehavior", EnumType::Short,
		{
			{ "Spew", (short)Weapon::Trigger::Behavior::Spew },
			{ "Latch", (short)Weapon::Trigger::Behavior::Latch },
			{ "LatchAutofire", (short)Weapon::Trigger::Behavior::LatchAutofire },
			{ "Charge", (short)Weapon::Trigger::Behavior::Charge },
			{ "LatchZoom", (short)Weapon::Trigger::Behavior::LatchZoom },
			{ "LatchRocketLauncher", (short)Weapon::Trigger::Behavior::LatchRocketLauncher },
			{ "SpewCharge", (short)Weapon::Trigger::Behavior::SpewCharge }
		}
	};

	const EnumDefinition TriggerPredictionEnum =
	{
		"TriggerPrediction", EnumType::Short,
		{
			{ "Explode", (short)Weapon::Trigger::Prediction::Spew },
			{ "Discharge", (short)Weapon::Trigger::Prediction::Charge }
		}
	};

	const EnumDefinition TriggerActionEnum =
	{
		"TriggerAction", EnumType::Short,
		{
			{ "Fire", (short)Weapon::Trigger::Action::Fire },
			{ "Charge", (short)Weapon::Trigger::Action::Charge },
			{ "Track", (short)Weapon::Trigger::Action::Track },
			{ "FireOther", (short)Weapon::Trigger::Action::FireOther }
		}
	};

	const EnumDefinition TriggerOverchargeActionEnum =
	{
		"TriggerOverchargeAction", EnumType::Short,
		{
			{ "Explode", (short)Weapon::Trigger::OverchargeAction::Discharge },
			{ "Discharge", (short)Weapon::Trigger::OverchargeAction::Explode }
		}
	};

	const EnumDefinition TriggerChargeFlagsEnum =
	{
		"TriggerChargeFlags", EnumType::Word,
		{
		}
	};

	const StructDefinition TriggerStruct =
	{
		"Trigger", sizeof(Weapon::Trigger),
		{
			{ FieldType::ShortEnum, "Flags", &TriggerFlagsEnum },
			{ FieldType::ShortEnum, "ButtonUsed", &TriggerButtonEnum },
			{ FieldType::ShortEnum, "Behavior", &TriggerBehaviorEnum },
			{ FieldType::ShortInteger, "PrimaryBarrel" },
			{ FieldType::ShortInteger, "SecondaryBarrel" },
			{ FieldType::ShortEnum, "Behavior", &TriggerPredictionEnum },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::Real, "AutofireTime" },
			{ FieldType::Real, "AutofireThrow" },
			{ FieldType::ShortEnum, "SecondaryAction", &TriggerActionEnum },
			{ FieldType::ShortEnum, "PrimaryAction", &TriggerActionEnum },
			{ FieldType::Real, "ChargingTime" },
			{ FieldType::Real, "ChargedTime" },
			{ FieldType::ShortEnum, "OverchargeAction", &TriggerOverchargeActionEnum },
			{ FieldType::ShortEnum, "ChargeFlags", &TriggerChargeFlagsEnum },
			{ FieldType::Real, "ChargedIllumination" },
			{ FieldType::Real, "SpewTime" },
			{ FieldType::TagReference, "ChargingEffect" },
			{ FieldType::TagReference, "ChargingDamageEffect" },
			{ FieldType::TagReference, "ChargingResponse" },
			{ FieldType::Real, "ChargingAgeDegeneration" },
			{ FieldType::TagReference, "DischargingEffect" },
			{ FieldType::TagReference, "DischargingDamageEffect" },
			{ FieldType::Real, "TargetTrackingDecayTime" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
		}
	};

	const EnumDefinition BarrelFlagsEnum =
	{
		"BarrelFlags", EnumType::DWord,
		{
		}
	};

	const EnumDefinition BarrelPredictionTypeEnum =
	{
		"PredictionType", EnumType::Short,
		{
		}
	};

	const EnumDefinition BarrelDistributionFunctionEnum =
	{
		"DistributionFunction", EnumType::Short,
		{
		}
	};

	const EnumDefinition BarrelDamageReportingTypeEnum =
	{
		"DistributionFunction", EnumType::Short,
		{
		}
	};

	const StructDefinition BarrelFunctionStruct =
	{
		"PredictedResource", sizeof(Weapon::Barrel::Function),
		{
			{ FieldType::Data, "Data" }
		}
	};

	const StructDefinition BarrelFirstPersonOffsetStruct =
	{
		"PredictedResource", sizeof(Weapon::Barrel::FirstPersonOffset),
		{
			{ FieldType::RealPoint3D, "Offset" }
		}
	};

	const StructDefinition BarrelFiringEffectsStruct =
	{
		"PredictedResource", sizeof(Weapon::Barrel::FiringEffects),
		{
			{ FieldType::ShortInteger, "ShotCountLowerBound" },
			{ FieldType::ShortInteger, "ShotCountUpperBound" },
			{ FieldType::TagReference, "FiringEffect" },
			{ FieldType::TagReference, "MisfireEffect" },
			{ FieldType::TagReference, "EmptyEffect" },
			{ FieldType::TagReference, "UnknownEffect" },
			{ FieldType::TagReference, "FiringResponse" },
			{ FieldType::TagReference, "MisfireResponse" },
			{ FieldType::TagReference, "EmptyResponse" },
			{ FieldType::TagReference, "UnknownResponse" },
			{ FieldType::TagReference, "RiderFiringResponse" },
			{ FieldType::TagReference, "RiderMisfireResponse" },
			{ FieldType::TagReference, "RiderEmptyResponse" },
			{ FieldType::TagReference, "RiderUnknownResponse" }
		}
	};

	const StructDefinition BarrelStruct =
	{
		"Barrel", sizeof(Weapon::Barrel),
		{
			{ FieldType::ShortEnum, "Flags", &BarrelFlagsEnum },
			{ FieldType::RealBounds, "RoundsPerSecondBounds" },
			{ FieldType::Real, "AccelerationTime" },
			{ FieldType::Real, "DecelerationTime" },
			{ FieldType::Real, "BarrelSpinScale" },
			{ FieldType::Real, "BlurredRateOfFire" },
			{ FieldType::ShortIntegerBounds, "ShotsPerFireBounds" },
			{ FieldType::Real, "FireRecoveryTime" },
			{ FieldType::Real, "SoftRecoveryFraction" },
			{ FieldType::ShortInteger, "Magazine" },
			{ FieldType::ShortInteger, "RoundsPerShot" },
			{ FieldType::ShortInteger, "MinimumRoundsLoaded" },
			{ FieldType::ShortInteger, "RoundsBetweenTracers" },
			{ FieldType::StringID, "OptionalBarrelMarkerName" },
			{ FieldType::ShortEnum, "PredictionType", &BarrelPredictionTypeEnum },
			{ FieldType::ShortEnum, "FiringNoise", &SoundsNoiseLevelEnum },
			{ FieldType::Real, "ErrorAccelerationTime" },
			{ FieldType::Real, "ErrorDecelerationTime" },
			{ FieldType::RealBounds, "DamageErrorBounds" },
			{ FieldType::Angle, "BaseTurningSpeed" },
			{ FieldType::AngleBounds, "DynamicTurningSpeedBounds" },
			{ FieldType::Real, "DualErrorAccelerationTime" },
			{ FieldType::Real, "DualErrorDecelerationTime" },
			{ FieldType::Real, "Unknown1" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Angle, "MinimumErrorAngle" },
			{ FieldType::AngleBounds, "ErrorAngleBounds" },
			{ FieldType::Real, "DualWieldDamageScale" },
			{ FieldType::ShortEnum, "DistributionFunction", &BarrelDistributionFunctionEnum },
			{ FieldType::ShortInteger, "ProjectilesPerShot" },
			{ FieldType::Angle, "DistributionAngle" },
			{ FieldType::Angle, "MinimumProjectileErrorAngle" },
			{ FieldType::RealBounds, "ProjectileErrorAngleBounds" },
			{ FieldType::Real, "ReloadPenalty" },
			{ FieldType::Real, "SwitchPenalty" },
			{ FieldType::Real, "ZoomPenalty" },
			{ FieldType::Real, "JumpPenalty" },
			{ FieldType::Block, "FiringPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Block, "FiringCrouchedPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Block, "MovingPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Block, "TurningPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::ShortInteger, "ErrorAngleMaximumRotation" },
			{ FieldType::Block, "DualFiringPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Block, "DualFiringCrouchedPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Block, "DualMovingPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Block, "DualTurningPenaltyFunction", &BarrelFunctionStruct },
			{ FieldType::Real, "DualErrorAngleMaximumRotation" },
			{ FieldType::Block, "FirstPersonOffsets", &BarrelFirstPersonOffsetStruct },
			{ FieldType::ShortEnum, "DamageReportingType", &BarrelDamageReportingTypeEnum },
			{ FieldType::CharInteger, "Unknown3" },
			{ FieldType::ShortInteger, "Unknown4" },
			{ FieldType::TagReference, "InitialProjectile" },
			{ FieldType::TagReference, "TrailingProjectile" },
			{ FieldType::TagReference, "DamageEffect" },
			{ FieldType::TagReference, "CrateProjectile" },
			{ FieldType::Real, "CrateProjectileSpeed" },
			{ FieldType::Real, "EjectionPortRecoveryTime" },
			{ FieldType::Real, "IlluminationRecoveryTime" },
			{ FieldType::Real, "HeatGeneratedPerRound" },
			{ FieldType::Real, "AgeGeneratedPerRoundMP" },
			{ FieldType::Real, "AgeGeneratedPerRoundSP" },
			{ FieldType::Real, "OverloadTime" },
			{ FieldType::RealBounds, "AngleChangePerShotBounds" },
			{ FieldType::Real, "AngleChangeAccelerationTime" },
			{ FieldType::Real, "AngleChangeDecelerationTime" },
			{ FieldType::ShortInteger, "AngleChangeFunction" },
			{ FieldType::ShortInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::Real, "FiringEffectDecelerationTime" },
			{ FieldType::Real, "Unknown8" },
			{ FieldType::Real, "RateOfFireAccelerationTime" },
			{ FieldType::Real, "RateOfFireDecelerationTime" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Real, "BloomRateOfDecay" },
			{ FieldType::Struct, "FiringEffects", &BarrelFiringEffectsStruct }
		}
	};

	const StructDefinition TagGroup<Weapon::GroupTag>::Definition =
	{
		"Weapon", sizeof(Weapon),
		{
			{ FieldType::ShortEnum, "WeaponFlags1", &Flags1Enum },
			{ FieldType::WordFlags, "WeaponFlags2", &Flags2Enum },
			{ FieldType::StringID, "Unknown6" },
			{ FieldType::ShortEnum, "SecondaryTriggerMode", &SecondaryTriggerModeEnum },
			{ FieldType::ShortInteger, "MaximumAlternateShotsLoaded" },
			{ FieldType::Real, "TurnOnTime" },
			{ FieldType::Real, "ReadyTime" },
			{ FieldType::TagReference, "ReadyEffect" },
			{ FieldType::TagReference, "ReadyDamageEffect" },
			{ FieldType::DwordInteger, "HeatRecoveryThreshold" },
			{ FieldType::DwordInteger, "OverheatedThreshold" },
			{ FieldType::DwordInteger, "HeatDetonationThreshold" },
			{ FieldType::DwordInteger, "HeatDetonationFraction" },
			{ FieldType::DwordInteger, "HeatLossPerSecond" },
			{ FieldType::DwordInteger, "HeatIllumination" },
			{ FieldType::DwordInteger, "OverheatedHeatLossPerSecond" },
			{ FieldType::TagReference, "Overheated" },
			{ FieldType::TagReference, "OverheatedDamageEffect" },
			{ FieldType::TagReference, "Detonation" },
			{ FieldType::TagReference, "DetonationDamageEffect2" },
			{ FieldType::TagReference, "PlayerMeleeDamage" },
			{ FieldType::TagReference, "PlayerMeleeResponse" },
			{ FieldType::RealEulerAngles2D, "DamagePyramidAngles" },
			{ FieldType::Real, "DamagePyramidDepth" },
			{ FieldType::TagReference, "FirstHitDamage" },
			{ FieldType::TagReference, "FirstHitResponse" },
			{ FieldType::TagReference, "SecondHitDamage" },
			{ FieldType::TagReference, "SecondHitResponse" },
			{ FieldType::TagReference, "ThirdHitDamage" },
			{ FieldType::TagReference, "ThirdHitResponse" },
			{ FieldType::TagReference, "LungeMeleeDamage" },
			{ FieldType::TagReference, "LungeMeleeResponse" },
			{ FieldType::TagReference, "GunOnGunClangDamage" },
			{ FieldType::TagReference, "GunOnGunClangResponse" },
			{ FieldType::TagReference, "GunOnSwordClangDamage" },
			{ FieldType::TagReference, "GunOnSwordClangResponse" },
			{ FieldType::TagReference, "ClashEffect" },
			{ FieldType::ShortEnum, "MeleeDamageReportingType", &DamageReportingTypeEnum },
			{ FieldType::CharInteger, "Unknown7" },
			{ FieldType::ShortInteger, "MagnificationLevels" },
			{ FieldType::Real, "MagnificationRangeLowerBound" },
			{ FieldType::Real, "MagnificationRangeUpperBound" },
			{ FieldType::ShortEnum, "MagnificationFlags", &MagnificationFlagsEnum },
			{ FieldType::Real, "WeaponSwitchReadySpeed0Default" },
			{ FieldType::Angle, "AutoaimAngle" },
			{ FieldType::Real, "AutoaimRangeLong" },
			{ FieldType::Real, "AutoaimRangeShort" },
			{ FieldType::Real, "AutoaimSafeRadius" },
			{ FieldType::Angle, "MagnetismAngle" },
			{ FieldType::Real, "MagnetismRangeLong" },
			{ FieldType::Real, "MagnetismRangeShort" },
			{ FieldType::Real, "MagnetismSafeRadius" },
			{ FieldType::Angle, "DeviationAngle" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::DwordInteger, "Unknown10" },
			{ FieldType::DwordInteger, "Unknown11" },
			{ FieldType::DwordInteger, "Unknown12" },
			{ FieldType::DwordInteger, "Unknown13" },
			{ FieldType::Block, "TargetTracking", &TargetTrackingStruct },
			{ FieldType::DwordInteger, "Unknown14" },
			{ FieldType::DwordInteger, "Unknown15" },
			{ FieldType::DwordInteger, "Unknown16" },
			{ FieldType::DwordInteger, "Unknown17" },
			{ FieldType::ShortEnum, "MovementPenalized", &MovementPenaltyEnum },
			{ FieldType::ShortInteger, "Unknown18" },
			{ FieldType::Real, "ForwardsMovementPenalty" },
			{ FieldType::Real, "SidewaysMovementPenalty" },
			{ FieldType::Real, "AIScariness" },
			{ FieldType::Real, "WeaponPowerOnTime" },
			{ FieldType::Real, "WeaponPowerOffTime" },
			{ FieldType::TagReference, "WeaponPowerOnEffect" },
			{ FieldType::TagReference, "WeaponPowerOffEffect" },
			{ FieldType::Real, "AgeHeatRecoveryPenalty" },
			{ FieldType::Real, "AgeRateOfFirePenalty" },
			{ FieldType::Real, "AgeMisfireStart" },
			{ FieldType::Real, "AgeMisfireChance" },
			{ FieldType::TagReference, "PickupSound" },
			{ FieldType::TagReference, "ZoomInSound" },
			{ FieldType::TagReference, "ZoomOutSound" },
			{ FieldType::DwordInteger, "ActiveCamoDing" },
			{ FieldType::DwordInteger, "Unknown19" },
			{ FieldType::DwordInteger, "Unknown20" },
			{ FieldType::DwordInteger, "Unknown21" },
			{ FieldType::StringID, "WeaponClass" },
			{ FieldType::StringID, "WeaponName" },
			{ FieldType::ShortEnum, "MultiplayerWeaponType", &MultiplayerTypeEnum },
			{ FieldType::ShortEnum, "WeaponType", &TypeEnum },
			{ FieldType::ShortEnum, "TrackingType", &TrackingTypeEnum },
			{ FieldType::ShortInteger, "Unknown22" },
			{ FieldType::ShortEnum, "SpecialHUDType", &SpecialHUDTypeEnum },
			{ FieldType::DwordInteger, "SpecialHUDIcon" },
			{ FieldType::Block, "FirstPersonResources", &FirstPersonResourcesStruct },
			{ FieldType::TagReference, "HUDInterface" },
			{ FieldType::Block, "PredictedResources", &PredictedResourceStruct },
			{ FieldType::Block, "Magazines", &MagazineStruct },
			{ FieldType::Block, "Triggers", &TriggerStruct },
			{ FieldType::Block, "Barrels", &BarrelStruct },
			{ FieldType::DwordInteger, "Unknown23" },
			{ FieldType::DwordInteger, "Unknown24" },
			{ FieldType::Real, "MaximumMovementAcceleration" },
			{ FieldType::Real, "MaximumMovementVelocity" },
			{ FieldType::Real, "MaximumTurningAcceleration" },
			{ FieldType::Real, "MaximumTurningVelocity" },
			{ FieldType::TagReference, "DeployedVehicle" },
			{ FieldType::TagReference, "DeployedWeapon" },
			{ FieldType::TagReference, "AgeModel" },
			{ FieldType::TagReference, "AgeWeapon" },
			{ FieldType::TagReference, "AgedMaterialEffects" },
			{ FieldType::Real, "HammerAgePerUseMP" },
			{ FieldType::Real, "HammerAgePerUseSP" },
			{ FieldType::RealVector3D, "FirstPersonWeaponOffset" },
			{ FieldType::RealVector2D, "FirstPersonScopeSize" },
			{ FieldType::Real, "ThirdPersonPitchLowerBound" },
			{ FieldType::Real, "ThirdPersonPitchUpperBound" },
			{ FieldType::Real, "ZoomTransitionTime" },
			{ FieldType::Real, "MeleeWeaponDelay" },
			{ FieldType::Real, "ReadyAnimationDuration" },
			{ FieldType::DwordInteger, "WeaponHolsterMarker" }
		}
	};
}