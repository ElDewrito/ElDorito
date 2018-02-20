#include "Blam\Tags\Objects\Unit.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Objects;
using namespace Definitions;

namespace Blam::Tags
{
	const EnumDefinition MetagamePropertiesUnitValuesEnum =
	{
		"UnitValue", EnumType::Char,
		{
			{ "Small", (char)Unit::MetagameProperty::UnitValue::Brute },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Grunt },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Jackal },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Marine },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Bugger },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Hunter },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::FloodInfection },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::FloodCarrier },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::FloodCombat },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::FloodPureform },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Sentinel },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Elite },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Turret },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Mongoose },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Warthog },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Scorpion },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Hornet },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Pelican },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Shade },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Watchtower },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Ghost },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Chopper },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Mauler },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Wraith },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Banshee },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Phantom },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Scarab },
			{ "Medium", (char)Unit::MetagameProperty::UnitValue::Guntower },
			{ "Engineer", (char)Unit::MetagameProperty::UnitValue::Engineer },
			{ "EngineerRechargeStation", (char)Unit::MetagameProperty::UnitValue::EngineerRechargeStation }
		}
	};

	const EnumDefinition MetagamePropertiesClassificationValuesEnum =
	{
		"ClassificationValue", EnumType::Char,
		{
			{ "Infantry", (char)Unit::MetagameProperty::ClassificationValue::Infantry },
			{ "Leader", (char)Unit::MetagameProperty::ClassificationValue::Leader },
			{ "Hero", (char)Unit::MetagameProperty::ClassificationValue::Hero },
			{ "Specialist", (char)Unit::MetagameProperty::ClassificationValue::Specialist },
			{ "LightVehicle", (char)Unit::MetagameProperty::ClassificationValue::LightVehicle },
			{ "HeavyVehicle", (char)Unit::MetagameProperty::ClassificationValue::HeavyVehicle },
			{ "GiantVehicle", (char)Unit::MetagameProperty::ClassificationValue::GiantVehicle },
			{ "StandardVehicle", (char)Unit::MetagameProperty::ClassificationValue::StandardVehicle }
		}
	};

	const StructDefinition MetagamePropertiesStruct =
	{
		"MetagameProperty", sizeof(Unit::MetagameProperty),
		{
			{ FieldType::CharInteger, "Flags" },
			{ FieldType::CharEnum, "Unit", &MetagamePropertiesUnitValuesEnum },
			{ FieldType::CharEnum, "Classification", &MetagamePropertiesClassificationValuesEnum },
			{ FieldType::CharInteger, "Unknown" },
			{ FieldType::ShortInteger, "Points" },
			{ FieldType::ShortInteger, "Unknown2" }
		}
	};

	const StructDefinition CameraTrackStruct =
	{
		"CameraTrackBlock", sizeof(Unit::CameraTrackBlock),
		{
			{ FieldType::TagReference, "CameraTrack" }
		}
	};

	const StructDefinition UnknownBlockStruct =
	{
		"UnknownBlock", sizeof(Unit::UnknownBlock),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::DwordInteger, "Unknown10" },
			{ FieldType::DwordInteger, "Unknown11" },
			{ FieldType::DwordInteger, "Unknown12" },
			{ FieldType::DwordInteger, "Unknown13" },
			{ FieldType::DwordInteger, "Unknown14" },
			{ FieldType::DwordInteger, "Unknown15" },
			{ FieldType::DwordInteger, "Unknown16" },
			{ FieldType::DwordInteger, "Unknown17" },
			{ FieldType::DwordInteger, "Unknown18" },
			{ FieldType::DwordInteger, "Unknown19" }
		}
	};

	const StructDefinition UnknownBlock2Struct =
	{
		"UnknownBlock2", sizeof(Unit::UnknownBlock2),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::DwordInteger, "Unknown10" },
			{ FieldType::DwordInteger, "Unknown11" },
			{ FieldType::DwordInteger, "Unknown12" },
			{ FieldType::DwordInteger, "Unknown13" },
			{ FieldType::DwordInteger, "Unknown14" },
			{ FieldType::DwordInteger, "Unknown15" },
			{ FieldType::DwordInteger, "Unknown16" },
			{ FieldType::DwordInteger, "Unknown17" },
			{ FieldType::DwordInteger, "Unknown18" },
			{ FieldType::DwordInteger, "Unknown19" }
		}
	};

	const StructDefinition PosturesStruct =
	{
		"Posture", sizeof(Unit::Posture),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::RealVector3D, "PillOffset" }
		}
	};

	const StructDefinition HudInterfacesStruct =
	{
		"HudInterface", sizeof(Unit::HudInterface),
		{
			{ FieldType::TagReference, "UnitHudInterface" }
		}
	};

	const StructDefinition DialogueVariantsStruct =
	{
		"DialogueVariant", sizeof(Unit::DialogueVariant),
		{
			{ FieldType::ShortInteger, "VariantNumber" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::TagReference, "Dialogue" }
		}
	};

	const StructDefinition PoweredSeatsStruct =
	{
		"PoweredSeat", sizeof(Unit::PoweredSeat),
		{
			{ FieldType::Real, "DriverPowerupTime" },
			{ FieldType::Real, "DriverPowerdownTime" }
		}
	};

	const StructDefinition WeaponsStruct =
	{
		"Weapon", sizeof(Unit::Weapon),
		{
			{ FieldType::TagReference, "Weapon2" }
		}
	};

	const StructDefinition TargetTrackingBlockTrackingTypesStruct =
	{
		"TargetTrackingBlock", sizeof(Unit::TargetTrackingBlock::TrackingType),
		{
			{ FieldType::StringID, "TrackingType2" }
		}
	};

	const StructDefinition TargetTrackingStruct =
	{
		"TargetTrackingBlock", sizeof(Unit::TargetTrackingBlock),
		{
			{ FieldType::Block, "TrackingTypes", &TargetTrackingBlockTrackingTypesStruct },
			{ FieldType::Real, "AcquireTime" },
			{ FieldType::Real, "GraceTime" },
			{ FieldType::Real, "DecayTime" },
			{ FieldType::TagReference, "TrackingSound" },
			{ FieldType::TagReference, "LockedSound" }
		}
	};

	const EnumDefinition SeatsAiSeatTypeValuesEnum =
	{
		"MotionSensorBlipSizeValue", EnumType::Short,
		{
			{ "Passenger", (short)Unit::Seat::AiSeatTypeValue::Passenger },
			{ "Gunner", (short)Unit::Seat::AiSeatTypeValue::Gunner },
			{ "SmallCargo", (short)Unit::Seat::AiSeatTypeValue::SmallCargo },
			{ "LargeCargo", (short)Unit::Seat::AiSeatTypeValue::LargeCargo },
			{ "Driver", (short)Unit::Seat::AiSeatTypeValue::Driver }
		}
	};

	const EnumDefinition SeatsCameraFlagsValuesEnum =
	{
		"ItemScaleValue", EnumType::Word,
		{
			{ "PitchBoundsAbsoluteSpace", (uint16_t)Unit::Seat::CameraFlagsValue::PitchBoundsAbsoluteSpace },
			{ "OnlyCollidesWithEnvironment", (uint16_t)Unit::Seat::CameraFlagsValue::OnlyCollidesWithEnvironment },
			{ "HidesPlayerUnitFromCamera", (uint16_t)Unit::Seat::CameraFlagsValue::HidesPlayerUnitFromCamera },
			{ "UseAimingVectorInsteadOfMarkerForward", (uint16_t)Unit::Seat::CameraFlagsValue::UseAimingVectorInsteadOfMarkerForward }
		}
	};

	const StructDefinition SeatsCameraAccelerationBlockStruct =
	{
		"Seat", sizeof(Unit::Seat::CameraAccelerationBlock),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::DwordInteger, "Unknown10" },
			{ FieldType::DwordInteger, "Unknown11" },
			{ FieldType::DwordInteger, "Unknown12" },
			{ FieldType::DwordInteger, "Unknown13" },
			{ FieldType::DwordInteger, "Unknown14" },
			{ FieldType::DwordInteger, "Unknown15" },
			{ FieldType::DwordInteger, "Unknown16" },
			{ FieldType::DwordInteger, "Unknown17" },
			{ FieldType::DwordInteger, "Unknown18" },
			{ FieldType::DwordInteger, "Unknown19" }
		}
	};

	const StructDefinition SeatsUnitHudInterfaceBlockStruct =
	{
		"Seat", sizeof(Unit::Seat::UnitHudInterfaceBlock),
		{
			{ FieldType::TagReference, "UnitHudInterface" }
		}
	};

	const StructDefinition SeatsStruct =
	{
		"Seat", sizeof(Unit::Seat),
		{
			{ FieldType::DwordInteger, "Flags" },
			{ FieldType::StringID, "SeatAnimation" },
			{ FieldType::StringID, "SeatMarkerName" },
			{ FieldType::StringID, "EntryMarkerSName" },
			{ FieldType::StringID, "BoardingGrenadeMarker" },
			{ FieldType::StringID, "BoardingGrenadeString" },
			{ FieldType::StringID, "BoardingMeleeString" },
			{ FieldType::StringID, "DetachWeaponString" },
			{ FieldType::Real, "PingScale" },
			{ FieldType::Real, "TurnoverTime" },
			{ FieldType::RealVector3D, "AccelerationRange" },
			{ FieldType::Real, "AccelerationActionScale" },
			{ FieldType::Real, "AccelerationAttachScale" },
			{ FieldType::Real, "AiScariness" },
			{ FieldType::ShortEnum, "AiSeatType", &SeatsAiSeatTypeValuesEnum },
			{ FieldType::ShortInteger, "BoardingSeat" },
			{ FieldType::Real, "ListenerInterpolationFactor" },

			{ FieldType::RealBounds, "YawRateBounds" },

			{ FieldType::RealBounds, "PitchRateBounds" },
			{ FieldType::Real, "PitchInterpolationTime" },

			{ FieldType::Real, "MinSpeedReference" },
			{ FieldType::Real, "MaxSpeedReference" },
			{ FieldType::Real, "SpeedExponent" },

			{ FieldType::WordFlags, "CameraFlags", &SeatsCameraFlagsValuesEnum },

			{ FieldType::Pad, 2 },


			{ FieldType::StringID, "CameraMarkerName" },
			{ FieldType::StringID, "CameraSubmergedMarkerName" },
			{ FieldType::AngleBounds, "PitchAutoLevel" },
			{ FieldType::AngleBounds, "PitchRange" },
			{ FieldType::Block, "CameraTracks", &CameraTrackStruct },
			{ FieldType::AngleBounds, "PitchSpringBounds" },
			{ FieldType::Angle, "SpringVelocity" },
			{ FieldType::Block, "Unknown7", &SeatsCameraAccelerationBlockStruct },
			{ FieldType::Block, "UnitHudInterface", &SeatsUnitHudInterfaceBlockStruct },
			{ FieldType::StringID, "EnterSeatString" },
			{ FieldType::AngleBounds, "YawRange" },
			{ FieldType::TagReference, "BuiltInGunner" },
			{ FieldType::Real, "EntryRadius" },
			{ FieldType::Angle, "EntryMarkerConeAngle" },
			{ FieldType::Angle, "EntryMarkerFacingAngle" },
			{ FieldType::Real, "MaximumRelativeVelocity" },
			{ FieldType::StringID, "InvisibleSeatRegion" },
			{ FieldType::LongInteger, "RuntimeInvisibleSeatRegionIndex" },
		}
	};

	const EnumDefinition DefaultTeamValuesEnum =
	{
		"DefaultTeamValue", EnumType::Short,
		{
			{ "Default", (short)Unit::DefaultTeamValue::Default },
			{ "Player", (short)Unit::DefaultTeamValue::Player },
			{ "Human", (short)Unit::DefaultTeamValue::Human },
			{ "Covenant", (short)Unit::DefaultTeamValue::Covenant },
			{ "Flood", (short)Unit::DefaultTeamValue::Flood },
			{ "Sentinel", (short)Unit::DefaultTeamValue::Sentinel },
			{ "Heretic", (short)Unit::DefaultTeamValue::Heretic },
			{ "Prophet", (short)Unit::DefaultTeamValue::Prophet },
			{ "Guilty", (short)Unit::DefaultTeamValue::Guilty },
			{ "Unused9", (short)Unit::DefaultTeamValue::Unused9 },
			{ "Unused10", (short)Unit::DefaultTeamValue::Unused10 },
			{ "Unused11", (short)Unit::DefaultTeamValue::Unused11 },
			{ "Unused12", (short)Unit::DefaultTeamValue::Unused12 },
			{ "Unused13", (short)Unit::DefaultTeamValue::Unused13 },
			{ "Unused14", (short)Unit::DefaultTeamValue::Unused14 },
			{ "Unused15", (short)Unit::DefaultTeamValue::Unused15 }
		}
	};

	const EnumDefinition MotionSensorBlipSizeValuesEnum =
	{
		"MotionSensorBlipSizeValue", EnumType::Short,
		{
			{ "Small", (short)Unit::MotionSensorBlipSizeValue::Small },
			{ "Medium", (short)Unit::MotionSensorBlipSizeValue::Medium },
			{ "Large", (short)Unit::MotionSensorBlipSizeValue::Large }
		}
	};

	const EnumDefinition ItemScaleValuesEnum =
	{
		"ItemScaleValue", EnumType::Short,
		{
			{ "Small", (short)Unit::ItemScaleValue::Small },
			{ "Medium", (short)Unit::ItemScaleValue::Medium },
			{ "Large", (short)Unit::ItemScaleValue::Large },
			{ "Huge", (short)Unit::ItemScaleValue::Huge }
		}
	};

	const EnumDefinition GrenadeTypeValuesEnum =
	{
		"GrenadeTypeValue", EnumType::Short,
		{
			{ "HumanFragmentation", (short)Unit::GrenadeTypeValue::HumanFragmentation },
			{ "CovenantPlasma", (short)Unit::GrenadeTypeValue::CovenantPlasma },
			{ "BruteSpike", (short)Unit::GrenadeTypeValue::BruteSpike },
			{ "Firebomb", (short)Unit::GrenadeTypeValue::Firebomb }
		}
	};

	const EnumDefinition ConstantSoundVolumeValuesEnum =
	{
		"ConstantSoundVolumeValue", EnumType::Short,
		{
			{ "Silent", (short)Unit::ConstantSoundVolumeValue::Silent },
			{ "Medium", (short)Unit::ConstantSoundVolumeValue::Medium },
			{ "Loud", (short)Unit::ConstantSoundVolumeValue::Loud },
			{ "Shout", (short)Unit::ConstantSoundVolumeValue::Shout },
			{ "WhenZoomedOrReloading", (short)Unit::ConstantSoundVolumeValue::Quiet }
		}
	};

	const StructDefinition TagGroup<Unit::GroupTag>::Definition =
	{
		"Unit", sizeof(Unit),
		{
			{ FieldType::DwordInteger, "FlagsWarningHalo4Values" },
			{ FieldType::ShortEnum, "DefaultTeam" },
			{ FieldType::ShortEnum, "ConstantSoundVolume" },
			{ FieldType::TagReference, "HologramUnit" },
			{ FieldType::Block, "MetagameProperties", &MetagamePropertiesStruct },
			{ FieldType::TagReference, "IntegratedLightToggle" },
			{ FieldType::Angle, "CameraFieldOfView" },
			{ FieldType::Real, "CameraStiffness" },
			{ FieldType::ShortInteger, "Flags2" },
			{ FieldType::ShortInteger, "Unknown6" },
			{ FieldType::StringID, "CameraMarkerName" },
			{ FieldType::StringID, "CameraSubmergedMarkerName" },
			{ FieldType::Angle, "PitchAutoLevel" },
			{ FieldType::Angle, "PitchRangeMin" },
			{ FieldType::Angle, "PitchRangeMax" },
			{ FieldType::Block, "CameraTracks", &CameraTrackStruct },
			{ FieldType::Angle, "Unknown7" },
			{ FieldType::Angle, "Unknown8" },
			{ FieldType::Angle, "Unknown9" },
			{ FieldType::Block, "Unknown10", &UnknownBlockStruct },
			{ FieldType::ShortInteger, "Flags3" },
			{ FieldType::ShortInteger, "Unknown11" },
			{ FieldType::StringID, "CameraMarkerName2" },
			{ FieldType::StringID, "CameraSubmergedMarkerName2" },
			{ FieldType::Angle, "PitchAutoLevel2" },
			{ FieldType::Angle, "PitchRangeMin2" },
			{ FieldType::Angle, "PitchRangeMax2" },
			{ FieldType::Block, "CameraTracks2", &CameraTrackStruct },
			{ FieldType::Angle, "Unknown12" },
			{ FieldType::Angle, "Unknown13" },
			{ FieldType::Angle, "Unknown14" },
			{ FieldType::Block, "Unknown5", &UnknownBlock2Struct },
			{ FieldType::TagReference, "AssassinationResponse" },
			{ FieldType::TagReference, "AssassinationWeapon" },
			{ FieldType::StringID, "AssasinationToolStowAnchor" },
			{ FieldType::StringID, "AssasinationToolHandMarker" },
			{ FieldType::StringID, "AssasinationToolMarker" },
			{ FieldType::RealVector3D, "AccelerationRange" },
			{ FieldType::Real, "AccelerationActionScale" },
			{ FieldType::Real, "AccelerationAttachScale" },
			{ FieldType::Real, "SoftPingThreshold" },
			{ FieldType::Real, "SoftPingInterruptTime" },
			{ FieldType::Real, "HardPingThreshold" },
			{ FieldType::Real, "HardPingInterruptTime" },
			{ FieldType::Real, "FeignDeathThreshold" },
			{ FieldType::Real, "FeignDeathTime" },
			{ FieldType::Real, "DistanceOfEvadeAnimation" },
			{ FieldType::Real, "DistanceOfDiveAnimation" },
			{ FieldType::Real, "StunnedMovementThreshold" },
			{ FieldType::Real, "FeignDeathChance" },
			{ FieldType::Real, "FeignRepeatChance" },
			{ FieldType::TagReference, "SpawnedTurretCharacter" },
			{ FieldType::ShortInteger, "SpawnedActorCountMin" },
			{ FieldType::ShortInteger, "SpawnedActorCountMax" },
			{ FieldType::Real, "SpawnedVelocity" },
			{ FieldType::Angle, "AimingVelocityMaximum" },
			{ FieldType::Angle, "AimingAccelerationMaximum" },
			{ FieldType::Real, "CasualAimingModifier" },
			{ FieldType::Angle, "LookingVelocityMaximum" },
			{ FieldType::Angle, "LookingAccelerationMaximum" },
			{ FieldType::StringID, "RightHandNode" },
			{ FieldType::StringID, "LeftHandNode" },
			{ FieldType::StringID, "PreferredGunNode" },
			{ FieldType::TagReference, "MeleeDamage" },
			{ FieldType::TagReference, "BoardingMeleeDamage" },
			{ FieldType::TagReference, "BoardingMeleeResponse" },
			{ FieldType::TagReference, "EjectionMeleeDamage" },
			{ FieldType::TagReference, "EjectionMeleeResponse" },
			{ FieldType::TagReference, "LandingMeleeDamage" },
			{ FieldType::TagReference, "FlurryMeleeDamage" },
			{ FieldType::TagReference, "ObstacleSmashMeleeDamage" },
			{ FieldType::TagReference, "ShieldPopDamage" },
			{ FieldType::TagReference, "AssassinationDamage" },
			{ FieldType::ShortEnum, "MotionSensorBlipSize", &MotionSensorBlipSizeValuesEnum },
			{ FieldType::ShortEnum, "ItemScale", &ItemScaleValuesEnum },
			{ FieldType::Block, "Postures", &PosturesStruct },
			{ FieldType::Block, "HudInterfaces", &HudInterfacesStruct },
			{ FieldType::Block, "DialogueVariants", &DialogueVariantsStruct },
			{ FieldType::DwordInteger, "Unknown16" },
			{ FieldType::DwordInteger, "Unknown17" },
			{ FieldType::DwordInteger, "Unknown18" },
			{ FieldType::DwordInteger, "Unknown19" },
			{ FieldType::Real, "GrenadeVelocity" },
			{ FieldType::ShortEnum, "GrenadeType", &GrenadeTypeValuesEnum },
			{ FieldType::ShortInteger, "GrenadeCount" },
			{ FieldType::Block, "PoweredSeats", &PoweredSeatsStruct },
			{ FieldType::Block, "Weapons", &WeaponsStruct },
			{ FieldType::Block, "TargetTracking", &TargetTrackingStruct },
			{ FieldType::Block, "Seats", &SeatsStruct },
			{ FieldType::Real, "EmpRadius" },
			{ FieldType::TagReference, "EmpEffect" },
			{ FieldType::TagReference, "BoostCollisionDamage" },
			{ FieldType::Real, "BoostPeakPower" },
			{ FieldType::Real, "BoostRisePower" },
			{ FieldType::Real, "BoostPeakTime" },
			{ FieldType::Real, "BoostFallPower" },
			{ FieldType::Real, "BoostDeadTime" },
			{ FieldType::Real, "LipsyncAttackWeight" },
			{ FieldType::Real, "LipsyncDecayWeight" },
			{ FieldType::TagReference, "DetachDamage" },
			{ FieldType::TagReference, "DetachedWeapon" },
		}
	};
}