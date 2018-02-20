#include "Blam\Tags\Objects\Vehicle.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Objects;
using namespace Definitions;

namespace Blam::Tags
{
	const StructDefinition EarlyMoverPropertiesStruct =
	{
		"EarlyMoverProperty", sizeof(Object::EarlyMoverProperty),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" }
		}
	};

	const StructDefinition AiPropertiesStruct =
	{
		"AiProperty", sizeof(Object::AIProperty),
		{
			{ FieldType::LongInteger, "Flags" },
			{ FieldType::StringID, "AiTypeName" },
			{ FieldType::ShortInteger, "Size" },
			{ FieldType::ShortInteger, "LeapJumpSpeed" }
		}
	};

	const StructDefinition FunctionsStruct =
	{
		"Function", sizeof(Object::Function),
		{
			{ FieldType::StringID, "Flags" },
			{ FieldType::StringID, "ImportName" },
			{ FieldType::StringID, "ExportName" },
			{ FieldType::StringID, "TurnOffWith" },
			{ FieldType::Real, "MinimumValue" },
			{ FieldType::Data, "DefaultFunction" },
			{ FieldType::StringID, "ScaleBy" }
		}
	};

	const StructDefinition AttachmentsStruct =
	{
		"Attachment", sizeof(Object::Attachment),
		{
			{ FieldType::LongInteger, "AtlasFlags" },
			{ FieldType::TagReference, "Attachment_1" },
			{ FieldType::StringID, "Marker" },
			{ FieldType::ShortInteger, "ChangeColor" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::StringID, "PrimaryScale" },
			{ FieldType::StringID, "SecondaryScale" }
		}
	};

	const StructDefinition WidgetsStruct =
	{
		"Widget", sizeof(Object::Widget),
		{
			{ FieldType::TagReference, "Type" }
		}
	};

	const StructDefinition ChangeColorsInitialPermutationStruct =
	{
		"InitialPermutation", sizeof(Object::ChangeColor::InitialPermutation),
		{
			{ FieldType::DwordInteger, "Weight" },
			{ FieldType::Real, "ColorLowerBoundR" },
			{ FieldType::Real, "ColorLowerBoundG" },
			{ FieldType::Real, "ColorLowerBoundB" },
			{ FieldType::Real, "ColorUpperBoundR" },
			{ FieldType::Real, "ColorUpperBoundG" },
			{ FieldType::Real, "ColorUpperBoundB" },
			{ FieldType::StringID, "VariantName" }
		}
	};

	const StructDefinition ChangeColorsFunctionsStruct =
	{
		"Functions", sizeof(Object::ChangeColor::Function),
		{
			{ FieldType::DwordInteger, "ScaleFlags" },
			{ FieldType::Real, "ColorLowerBoundR" },
			{ FieldType::Real, "ColorLowerBoundG" },
			{ FieldType::Real, "ColorLowerBoundB" },
			{ FieldType::Real, "ColorUpperBoundR" },
			{ FieldType::Real, "ColorUpperBoundG" },
			{ FieldType::Real, "ColorUpperBoundB" },
			{ FieldType::StringID, "DarkenBy" },
			{ FieldType::StringID, "ScaleBy" }
		}
	};

	const StructDefinition ChangeColorsStruct =
	{
		"ChangeColor", sizeof(Object::ChangeColor),
		{
			{ FieldType::Block, "DialogueVariants", &ChangeColorsInitialPermutationStruct },
			{ FieldType::Block, "DialogueVariants", &ChangeColorsFunctionsStruct }
		}
	};

	const StructDefinition NodeMapsStruct =
	{
		"NodeMap", sizeof(Object::NodeMap),
		{
			{ FieldType::CharInteger, "TargetNode" }
		}
	};

	const StructDefinition MultiplayerObjectPropertiesStruct =
	{
		"MultiplayerObjectProperty", sizeof(Object::MultiplayerProperties),
		{
			{ FieldType::WordInteger, "EngineFlags" },
			{ FieldType::CharInteger, "ObjectType" },
			{ FieldType::ByteInteger, "TeleporterFlags" },
			{ FieldType::WordInteger, "Flags" },
			{ FieldType::CharInteger, "Shape" },
			{ FieldType::CharInteger, "SpawnTimerMode" },
			{ FieldType::ShortInteger, "SpawnTime" },
			{ FieldType::ShortInteger, "AbandonTime" },
			{ FieldType::Real, "RadiusWidth" },
			{ FieldType::Real, "Length" },
			{ FieldType::Real, "Top" },
			{ FieldType::Real, "Bottom" },
			{ FieldType::Real, "Unknown" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Real, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::TagReference, "ChildObject" },
			{ FieldType::LongInteger, "Unknown6" },
			{ FieldType::TagReference, "ShapeShader" },
			{ FieldType::TagReference, "UnknownShader" },
			{ FieldType::TagReference, "Unknown7" },
			{ FieldType::TagReference, "Unknown8" },
			{ FieldType::TagReference, "Unknown9" },
			{ FieldType::TagReference, "Unknown10" },
			{ FieldType::TagReference, "Unknown11" },
			{ FieldType::TagReference, "Unknown12" }
		}
	};

	const StructDefinition ModelObjectDataStruct =
	{
		"ModelObjectDatum", sizeof(Object::ModelObjectDatum),
		{
			{ FieldType::ShortInteger, "Type" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::Real, "OffsetX" },
			{ FieldType::Real, "OffsetY" },
			{ FieldType::Real, "OffsetZ" },
			{ FieldType::Real, "Radius" }
		}
	};

	const StructDefinition MetagamePropertiesStruct =
	{
		"MetagameProperty", sizeof(Unit::MetagameProperty),
		{
			{ FieldType::ByteInteger, "Flags" },
			{ FieldType::CharInteger, "Unit" },
			{ FieldType::CharInteger, "Classification" },
			{ FieldType::CharInteger, "Unknown" },
			{ FieldType::ShortInteger, "Points" },
			{ FieldType::ShortInteger, "Unknown2" }
		}
	};

	const StructDefinition CameraTracksStruct =
	{
		"CameraTrack", sizeof(Unit::CameraTrackBlock),
		{
			{ FieldType::TagReference, "Track" }
		}
	};

	const StructDefinition Unknown10Struct =
	{
		"Unknown10", sizeof(Vehicle::Unknown10),
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
			{ FieldType::DwordInteger, "Unknown10_1" },
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

	const StructDefinition Unknown16Struct =
	{
		"Unknown16", sizeof(Vehicle::Unknown16),
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
			{ FieldType::DwordInteger, "Unknown16_1" },
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
			{ FieldType::Real, "PillOffsetI" },
			{ FieldType::Real, "PillOffsetJ" },
			{ FieldType::Real, "PillOffsetK" }
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
			{ FieldType::TagReference, "Weapon_1" }
		}
	};

	const StructDefinition TrackingTypeStruct =
	{
		"TrackingType", sizeof(Unit::TargetTrackingBlock::TrackingType),
		{
			{ FieldType::StringID, "TrackingType_1" }
		}
	};

	const StructDefinition TargetTrackingStruct =
	{
		"TargetTracking", sizeof(Unit::TargetTrackingBlock),
		{
			{ FieldType::Block, "TrackingTypes", &TrackingTypeStruct },
			{ FieldType::Real, "AcquireTime" },
			{ FieldType::Real, "GraceTime" },
			{ FieldType::Real, "DecayTime" },
			{ FieldType::TagReference, "TrackingSound" },
			{ FieldType::TagReference, "LockedSound" }
		}
	};

	const StructDefinition SeatsUnknown7Struct =
	{
		"Unknown7", sizeof(Unit::Seat::Unknown7),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7_1" },
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

	const StructDefinition SeatsUnitHudInterfaceStruct =
	{
		"UnitHudInterface", sizeof(Unit::Seat::UnitHudInterface),
		{
			{ FieldType::TagReference, "UnitHudInterface_1" }
		}
	};

	const StructDefinition SeatsStruct =
	{
		"Seat", sizeof(Unit::Seat),
		{
			{ FieldType::LongInteger, "UnitHudInterface_1" },
			{ FieldType::StringID, "SeatAnimation" },
			{ FieldType::StringID, "SeatMarkerName" },
			{ FieldType::StringID, "EntryMarkerSName" },
			{ FieldType::StringID, "BoardingGrenadeMarker" },
			{ FieldType::StringID, "BoardingGrenadeString" },
			{ FieldType::StringID, "BoardingMeleeString" },
			{ FieldType::StringID, "DetachWeaponString" },
			{ FieldType::Real, "PingScale" },
			{ FieldType::Real, "TurnoverTime" },
			{ FieldType::Real, "AccelerationRangeI" },
			{ FieldType::Real, "AccelerationRangeJ" },
			{ FieldType::Real, "AccelerationRangeK" },
			{ FieldType::Real, "AccelerationActionScale" },
			{ FieldType::Real, "AccelerationAttachScale" },
			{ FieldType::Real, "AiScariness" },
			{ FieldType::ShortInteger, "AiSeatType" },
			{ FieldType::ShortInteger, "BoardingSeat" },
			{ FieldType::Real, "ListenerInterpolationFactor" },
			{ FieldType::Real, "YawRateBoundsMin" },
			{ FieldType::Real, "YawRateBoundsMax" },
			{ FieldType::Real, "PitchRateBoundsMin" },
			{ FieldType::Real, "PitchRateBoundsMax" },
			{ FieldType::Real, "Unknown" },
			{ FieldType::Real, "MinimumSpeedReference" },
			{ FieldType::Real, "MaximumSpeedReference" },
			{ FieldType::Real, "SpeedExponent" },
			{ FieldType::ShortInteger, "Unknown2" },
			{ FieldType::ShortInteger, "Unknown3" },
			{ FieldType::StringID, "CameraMarkerName" },
			{ FieldType::StringID, "CameraSubmergedMarkerName" },
			{ FieldType::Real, "PitchAutoLevel" },
			{ FieldType::Real, "PitchRangeMin" },
			{ FieldType::Real, "PitchRangeMax" },
			{ FieldType::Block, "CameraTracks", &CameraTracksStruct },
			{ FieldType::Real, "Unknown4" },
			{ FieldType::Real, "Unknown5" },
			{ FieldType::Real, "Unknown6" },
			{ FieldType::Block, "Unknown8", &SeatsUnknown7Struct },
			{ FieldType::Block, "UnitHudInterface2", &SeatsUnitHudInterfaceStruct },
			{ FieldType::StringID, "EnterSeatString" },
			{ FieldType::Real, "YawRangeMin" },
			{ FieldType::Real, "YawRangeMax" },
			{ FieldType::TagReference, "BuiltInGunner" },
			{ FieldType::Real, "EntryRadius" },
			{ FieldType::Real, "EntryMarkerConeAngle" },
			{ FieldType::Real, "EntryMarkerFacingAngle" },
			{ FieldType::Real, "MaximumRelativeVelocity" },
			{ FieldType::StringID, "InvisibleSeatRegion" },
			{ FieldType::LongInteger, "RuntimeInvisibleSeatRegionIndex" },
		}
	};

	const StructDefinition TankEngineMotionPropertyGearStruct =
	{
		"Gear", sizeof(Vehicle::TankEngineMotionProperty::Gear),
		{
			{ FieldType::Real, "MinTorque" },
			{ FieldType::Real, "MaxTorque" },
			{ FieldType::Real, "PeakTorqueScale" },
			{ FieldType::Real, "PastPeakTorqueExponent" },
			{ FieldType::Real, "TorqueAtMaxAngularVelovity" },
			{ FieldType::Real, "TorqueAt2xMaxAngularVelocity" },
			{ FieldType::Real, "MinTorque2" },
			{ FieldType::Real, "MaxTorque2" },
			{ FieldType::Real, "PeakTorqueScale2" },
			{ FieldType::Real, "PastPeakTorqueExponent2" },
			{ FieldType::Real, "TorqueAtMaxAngularVelovity2" },
			{ FieldType::Real, "TorqueAt2xMaxAngularVelocity2" },
			{ FieldType::Real, "MinTimeToUpshift" },
			{ FieldType::Real, "EngineUpshiftScale" },
			{ FieldType::Real, "GearRatio" },
			{ FieldType::Real, "MinTimeToDownshift" },
			{ FieldType::Real, "EngineDownshiftScale" }
		}
	};

	const StructDefinition TankEngineMotionPropertiesStruct =
	{
		"TankEngineMotionProperty", sizeof(Vehicle::TankEngineMotionProperty),
		{
			{ FieldType::Real, "SteeringOverdampenCuspAngle" },
			{ FieldType::Real, "SteeringOverdamenExponent" },
			{ FieldType::Real, "Unknown" },
			{ FieldType::Real, "SpeedLeft" },
			{ FieldType::Real, "SpeedRight" },
			{ FieldType::Real, "TurningSpeedLeft" },
			{ FieldType::Real, "TurningSpeedRight" },
			{ FieldType::Real, "SpeedLeft2" },
			{ FieldType::Real, "SpeedRight2" },
			{ FieldType::Real, "TurningSpeedLeft2" },
			{ FieldType::Real, "TurningSpeedRight2" },
			{ FieldType::Real, "EngineMomentum" },
			{ FieldType::Real, "EngineMaximumAngularVelocity" },
			{ FieldType::Block, "Gears", &TankEngineMotionPropertyGearStruct },
			{ FieldType::TagReference, "ChangeGearSound" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Real, "Unknown3" }
		}
	};

	const StructDefinition EngineMotionPropertyGearStruct =
	{
		"Gear", sizeof(Vehicle::EngineMotionProperty::Gear),
		{
			{ FieldType::Real, "MinTorque" },
			{ FieldType::Real, "MaxTorque" },
			{ FieldType::Real, "PeakTorqueScale" },
			{ FieldType::Real, "PastPeakTorqueExponent" },
			{ FieldType::Real, "TorqueAtMaxAngularVelovity" },
			{ FieldType::Real, "TorqueAt2xMaxAngularVelocity" },
			{ FieldType::Real, "MinTorque2" },
			{ FieldType::Real, "MaxTorque2" },
			{ FieldType::Real, "PeakTorqueScale2" },
			{ FieldType::Real, "PastPeakTorqueExponent2" },
			{ FieldType::Real, "TorqueAtMaxAngularVelovity2" },
			{ FieldType::Real, "TorqueAt2xMaxAngularVelocity2" },
			{ FieldType::Real, "MinTimeToUpshift" },
			{ FieldType::Real, "EngineUpshiftScale" },
			{ FieldType::Real, "GearRatio" },
			{ FieldType::Real, "MinTimeToDownshift" },
			{ FieldType::Real, "EngineDownshiftScale" }
		}
	};

	const StructDefinition EngineMotionPropertiesStruct =
	{
		"EngineMotionProperty", sizeof(Vehicle::EngineMotionProperty),
		{
			{ FieldType::Real, "SteeringOverdampenCuspAngle" },
			{ FieldType::Real, "SteeringOverdamenExponent" },
			{ FieldType::Real, "MaximumLeftTurn" },
			{ FieldType::Real, "MaximumRightTurnNegative" },
			{ FieldType::Real, "TurnRate" },
			{ FieldType::Real, "EngineMomentum" },
			{ FieldType::Real, "EngineMaximumAngularVelocity" },
			{ FieldType::Block, "Gears", &EngineMotionPropertyGearStruct },
			{ FieldType::TagReference, "ChangeGearSound" },
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" }
		}
	};

	const StructDefinition DropshipMotionPropertiesStruct =
	{
		"DropshipMotionProperty", sizeof(Vehicle::DropshipMotionProperty),
		{
			{ FieldType::Real, "ForwardAcceleration" },
			{ FieldType::Real, "BackwardAcceleration" },
			{ FieldType::Real, "Unknown" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Real, "LeftStrafeAcceleration" },
			{ FieldType::Real, "RightStrafeAcceleration" },
			{ FieldType::Real, "Unknown3" },
			{ FieldType::Real, "Unknown4" },
			{ FieldType::Real, "LiftAcceleration" },
			{ FieldType::Real, "DropAcceleration" },
			{ FieldType::Real, "Unknown5" },
			{ FieldType::Real, "Unknown6" },
			{ FieldType::Real, "Unknown7" },
			{ FieldType::Real, "Unknown8" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::Real, "Unknown11" },
			{ FieldType::Real, "Unknown12" },
			{ FieldType::Real, "Unknown13" }
		}
	};

	const StructDefinition AntigravityMotionPropertiesStruct =
	{
		"AntigravityMotionProperty", sizeof(Vehicle::AntigravityMotionProperty),
		{
			{ FieldType::Real, "SteeringOverdampenCuspAngle" },
			{ FieldType::Real, "SteeringOverdamenExponent" },
			{ FieldType::Real, "MaximumForwardSpeed" },
			{ FieldType::Real, "MaximumReverseSpeed" },
			{ FieldType::Real, "SpeedAcceleration" },
			{ FieldType::Real, "SpeedDeceleration" },
			{ FieldType::Real, "MaximumLeftSlide" },
			{ FieldType::Real, "MaximumRightSlide" },
			{ FieldType::Real, "SlideAcceleration" },
			{ FieldType::Real, "SlideDeceleration" },
			{ FieldType::CharInteger, "Unknown" },
			{ FieldType::CharInteger, "Unknown2" },
			{ FieldType::CharInteger, "Unknown3" },
			{ FieldType::CharInteger, "Unknown4" },
			{ FieldType::Real, "Traction" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::Real, "TurningRate" },
			{ FieldType::StringID, "Unknown6" },
			{ FieldType::Real, "Unknown7" },
			{ FieldType::Real, "Unknown8" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::StringID, "Unknown11" },
			{ FieldType::Real, "Unknown12" },
			{ FieldType::Real, "Unknown13" },
			{ FieldType::Real, "Unknown14" },
			{ FieldType::Real, "Unknown15" },
			{ FieldType::Real, "Unknown16" },
			{ FieldType::Real, "Unknown17" },
			{ FieldType::Real, "Unknown18" },
			{ FieldType::Real, "Unknown19" }
		}
	};

	const StructDefinition JetEngineMotionPropertiesStruct =
	{
		"JetEngineMotionProperty", sizeof(Vehicle::JetEngineMotionProperty),
		{
			{ FieldType::Real, "SteeringOverdampenCuspAngle" },
			{ FieldType::Real, "SteeringOverdamenExponent" },
			{ FieldType::Real, "MaximumLeftTurn" },
			{ FieldType::Real, "MaximumRightTurnNegative" },
			{ FieldType::Real, "TurnRate" },
			{ FieldType::Real, "FlyingSpeed" },
			{ FieldType::Real, "Acceleration" },
			{ FieldType::Real, "SpeedAcceleration" },
			{ FieldType::Real, "SpeedDeceleration" },
			{ FieldType::Real, "PitchLeftSpeed" },
			{ FieldType::Real, "PitchRightSpeed" },
			{ FieldType::Real, "PitchRate" },
			{ FieldType::Real, "UnpitchRate" },
			{ FieldType::Real, "FlightStability" },
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::Real, "NoseAngle" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Real, "Unknown3" },
			{ FieldType::Real, "Unknown4" },
			{ FieldType::Real, "FallingSpeed" },
			{ FieldType::Real, "FallingSpeed2" },
			{ FieldType::Real, "Unknown5" },
			{ FieldType::Real, "Unknown6" },
			{ FieldType::Real, "IdleRise" },
			{ FieldType::Real, "IdleForward" },
			{ FieldType::DwordInteger, "Unknown7" }
		}
	};

	const StructDefinition TurretPropertiesStruct =
	{
		"TurretProperty", sizeof(Vehicle::TurretProperty),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" }
		}
	};

	const StructDefinition HelicopterMotionPropertiesStruct =
	{
		"HelicopterMotionProperty", sizeof(Vehicle::HelicopterMotionProperty),
		{
			{ FieldType::Real, "MaximumLeftTurn" },
			{ FieldType::Real, "MaximumRightTurnNegative" },
			{ FieldType::Real, "Unknown" },
			{ FieldType::StringID, "ThrustFrontLeft" },
			{ FieldType::StringID, "ThrustFrontRight" },
			{ FieldType::StringID, "Thrust" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Real, "Unknown3" },
			{ FieldType::Real, "Unknown4" },
			{ FieldType::Real, "Unknown5" },
			{ FieldType::Real, "Unknown6" },
			{ FieldType::Real, "Unknown7" },
			{ FieldType::Real, "Unknown8" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::Real, "Unknown11" },
			{ FieldType::Real, "Unknown12" },
			{ FieldType::Real, "Unknown13" },
			{ FieldType::Real, "Unknown14" },
			{ FieldType::Real, "Unknown15" },
			{ FieldType::Real, "Unknown16" },
			{ FieldType::Real, "Unknown17" },
			{ FieldType::Real, "Unknown18" },
			{ FieldType::Real, "Unknown19" },
			{ FieldType::Real, "Unknown20" },
			{ FieldType::Real, "Unknown21" },
			{ FieldType::Real, "Unknown22" },
			{ FieldType::Real, "Unknown23" },
			{ FieldType::Real, "Unknown24" }
		}
	};

	const StructDefinition AntigravityEngineMotionPropertyGearStruct =
	{
		"Gear", sizeof(Vehicle::AntigravityEngineMotionProperty::Gear),
		{
			{ FieldType::Real, "MinTorque" },
			{ FieldType::Real, "MaxTorque" },
			{ FieldType::Real, "PeakTorqueScale" },
			{ FieldType::Real, "PastPeakTorqueExponent" },
			{ FieldType::Real, "TorqueAtMaxAngularVelovity" },
			{ FieldType::Real, "TorqueAt2xMaxAngularVelocity" },
			{ FieldType::Real, "MinTorque2" },
			{ FieldType::Real, "MaxTorque2" },
			{ FieldType::Real, "PeakTorqueScale2" },
			{ FieldType::Real, "PastPeakTorqueExponent2" },
			{ FieldType::Real, "TorqueAtMaxAngularVelovity2" },
			{ FieldType::Real, "TorqueAt2xMaxAngularVelocity2" },
			{ FieldType::Real, "MinTimeToUpshift" },
			{ FieldType::Real, "EngineUpshiftScale" },
			{ FieldType::Real, "GearRatio" },
			{ FieldType::Real, "MinTimeToDownshift" },
			{ FieldType::Real, "EngineDownshiftScale" }
		}
	};

	const StructDefinition AntigravityEngineMotionPropertiesStruct =
	{
		"AntigravityEngineMotionProperty", sizeof(Vehicle::AntigravityEngineMotionProperty),
		{
			{ FieldType::Real, "SteeringOverdampenCuspAngle" },
			{ FieldType::Real, "SteeringOverdamenExponent" },
			{ FieldType::Real, "MaximumLeftTurn" },
			{ FieldType::Real, "MaximumRightTurnNegative" },
			{ FieldType::Real, "TurnRate" },
			{ FieldType::Real, "EngineMomentum" },
			{ FieldType::Real, "EngineMaximumAngularVelocity" },
			{ FieldType::Block, "Gears", &AntigravityEngineMotionPropertyGearStruct },
			{ FieldType::TagReference, "ChangeGearSound" },
			{ FieldType::Real, "Unknown" },
			{ FieldType::StringID, "Unknown2" },
			{ FieldType::Real, "Unknown3" },
			{ FieldType::Real, "Unknown4" },
			{ FieldType::Real, "Unknown5" },
			{ FieldType::Real, "Unknown6" },
			{ FieldType::Real, "Unknown7" },
			{ FieldType::Real, "Unknown8" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::Real, "Unknown11" },
			{ FieldType::Real, "Unknown12" },
			{ FieldType::Real, "Unknown13" },
			{ FieldType::Real, "Unknown14" }
		}
	};

	const StructDefinition AutoturretEquipmentStruct =
	{
		"AutoturretEquipment", sizeof(Vehicle::AutoturretEquipment),
		{
			{ FieldType::Real, "Unknown" },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::Real, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::Real, "Unknown11" },
			{ FieldType::Real, "Unknown12" }
		}
	};

	const StructDefinition AntiGravityPointsStruct =
	{
		"AntiGravityPoint", sizeof(Vehicle::AntiGravityPoint),
		{
			{ FieldType::StringID, "MarkerName" },
			{ FieldType::LongInteger, "Flags" },
			{ FieldType::Real, "AntigravStrength" },
			{ FieldType::Real, "AntigravOffset" },
			{ FieldType::Real, "AntigravHeight" },
			{ FieldType::Real, "AntigravDumpFactor" },
			{ FieldType::Real, "AntigravNormalK1" },
			{ FieldType::Real, "AntigravNormalK0" },
			{ FieldType::Real, "Radius" },
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::ShortInteger, "Unknown4" },
			{ FieldType::ShortInteger, "DamageSourceRegionIndex" },
			{ FieldType::StringID, "DamageSourceRegionName" },
			{ FieldType::Real, "DefaultStateError" },
			{ FieldType::Real, "MinorDamageError" },
			{ FieldType::Real, "MediumDamageError" },
			{ FieldType::Real, "MajorDamageError" },
			{ FieldType::Real, "DestroyedStateError" }
		}
	};

	const StructDefinition FrictionPointsStruct =
	{
		"FrictionPoint", sizeof(Vehicle::FrictionPoint),
		{
			{ FieldType::StringID, "MarkerName" },
			{ FieldType::LongInteger, "Flags" },
			{ FieldType::Real, "FractionOfTotalMass" },
			{ FieldType::Real, "Radius" },
			{ FieldType::Real, "DamagedRadius" },
			{ FieldType::ShortInteger, "FrictionType" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::Real, "MovingFrictionVelocityDiff" },
			{ FieldType::Real, "EBrakeMovingFriction" },
			{ FieldType::Real, "EBrakeFriction" },
			{ FieldType::Real, "EBrakeMovingFrictionVelocityDiff" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::StringID, "CollisionMaterialName" },
			{ FieldType::ShortInteger, "CollisionGlobalMaterialIndex" },
			{ FieldType::ShortInteger, "ModelStateDestroyed" },
			{ FieldType::StringID, "RegionName" },
			{ FieldType::LongInteger, "RegionIndex" }
		}
	};

	const StructDefinition PhantomShapeSphereStruct =
	{
		"PhantomShapeSphere", sizeof(Vehicle::PhantomShape::PhantomShapePart::PhantomShapeSphere),
		{
			{ FieldType::RealPoint3D, "Position" },
			{ FieldType::Real, "Radius" }
		}
	};

	const StructDefinition PhantomShapePartStruct =
	{
		"PhantomShapePart", sizeof(Vehicle::PhantomShape::PhantomShapePart),
		{
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::ShortInteger, "Unknown5" },
			{ FieldType::ShortInteger, "Unknown6" },
			{ FieldType::LongInteger, "CollisionMaterialName" },
			{ FieldType::LongInteger, "CollisionGlobalMaterialIndex" },
			{ FieldType::LongInteger, "ModelStateDestroyed" },
			{ FieldType::DwordInteger, "RegionName" },
			{ FieldType::DwordInteger, "RegionName" },
			{ FieldType::DwordInteger, "RegionName" },
			{ FieldType::Array, FieldType::Struct, "Spheres", 8, &PhantomShapeSphereStruct }
		}
	};

	const StructDefinition PhantomShapesStruct =
	{
		"PhantomShape", sizeof(Vehicle::PhantomShape),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::ShortInteger, "Size" },
			{ FieldType::ShortInteger, "Count" },
			{ FieldType::LongInteger, "OverallShapeIndex" },
			{ FieldType::LongInteger, "Offset" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::DwordInteger, "Unknown7" },
			{ FieldType::DwordInteger, "Unknown8" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::Real, "Unknown11" },
			{ FieldType::Real, "Unknown12" },
			{ FieldType::Real, "Unknown13" },
			{ FieldType::Real, "Unknown14" },
			{ FieldType::Real, "Unknown15" },
			{ FieldType::Real, "Unknown16" },
			{ FieldType::DwordInteger, "Unknown17" },
			{ FieldType::LongInteger, "MultisphereCount" },
			{ FieldType::LongInteger, "Flags" },
			{ FieldType::Real, "X0" },
			{ FieldType::Real, "X1" },
			{ FieldType::Real, "Y0" },
			{ FieldType::Real, "Y1" },
			{ FieldType::Real, "Z0" },
			{ FieldType::Real, "Z1" },
			{ FieldType::Array, FieldType::Struct, "Parts", 5, &PhantomShapePartStruct },
			{ FieldType::DwordInteger, "Unknown34" },
			{ FieldType::DwordInteger, "Unknown35" },
			{ FieldType::DwordInteger, "Unknown36" },
			{ FieldType::DwordInteger, "Unknown37" },
			{ FieldType::DwordInteger, "Unknown38" },
			{ FieldType::DwordInteger, "Unknown39" },
			{ FieldType::DwordInteger, "Unknown40" },
			{ FieldType::DwordInteger, "Unknown41" },
			{ FieldType::DwordInteger, "Unknown42" },
			{ FieldType::DwordInteger, "Unknown43" },
			{ FieldType::DwordInteger, "Unknown44" },
			{ FieldType::DwordInteger, "Unknown45" },
			{ FieldType::DwordInteger, "Unknown46" },
			{ FieldType::DwordInteger, "Unknown47" },
			{ FieldType::DwordInteger, "Unknown48" },
			{ FieldType::DwordInteger, "Unknown49" },
		}
	};

	const StructDefinition TagGroup<Vehicle::GroupTag>::Definition =
	{
		"Vehicle", sizeof(Vehicle),
		{
			{ FieldType::ShortInteger, "MaximumAlternateShotsLoaded" },
			{ FieldType::WordInteger, "Flags" },
			{ FieldType::Real, "BoundingRadius" },
			{ FieldType::Real, "BoundingOffsetX" },
			{ FieldType::Real, "BoundingOffsetY" },
			{ FieldType::Real, "BoundingOffsetZ" },
			{ FieldType::Real, "AccelerationScale" },
			{ FieldType::ShortInteger, "LightmapShadowModeSize" },
			{ FieldType::CharInteger, "SweetenerSize" },
			{ FieldType::CharInteger, "WaterDensity" },
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::Real, "DynamicLightSphereRadius" },
			{ FieldType::Real, "DynamicLightSphereOffsetX" },
			{ FieldType::Real, "DynamicLightSphereOffsetY" },
			{ FieldType::Real, "DynamicLightSphereOffsetZ" },
			{ FieldType::StringID, "DefaultModelVariant" },
			{ FieldType::TagReference, "Model" },
			{ FieldType::TagReference, "CrateObject" },
			{ FieldType::TagReference, "CollisionDamage" },
			{ FieldType::Block, "Overheated", &EarlyMoverPropertiesStruct },
			{ FieldType::TagReference, "CreationEffect" },
			{ FieldType::TagReference, "MaterialEffects" },
			{ FieldType::TagReference, "ArmorSounds" },
			{ FieldType::TagReference, "MeleeImpact" },
			{ FieldType::Block, "AiProperties", &AiPropertiesStruct },
			{ FieldType::Block, "Functions", &FunctionsStruct },
			{ FieldType::ShortInteger, "HudTextMessageIndex" },
			{ FieldType::ShortInteger, "Unknown2" },
			{ FieldType::Block, "Attachments", &AttachmentsStruct },
			{ FieldType::Block, "Widgets", &WidgetsStruct },
			{ FieldType::Block, "ChangeColors", &ChangeColorsStruct },
			{ FieldType::Block, "NodeMaps", &NodeMapsStruct },
			{ FieldType::Block, "MultiplayerObjectProperties", &MultiplayerObjectPropertiesStruct },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::Block, "ModelObjectData", &ModelObjectDataStruct },
			{ FieldType::LongInteger, "Flags2" },
			{ FieldType::ShortInteger, "DefaultTeam" },
			{ FieldType::ShortInteger, "ConstantSoundVolume" },
			{ FieldType::TagReference, "HologramUnit" },
			{ FieldType::Block, "MetagameProperties", &MetagamePropertiesStruct },
			{ FieldType::TagReference, "IntegratedLightToggle" },
			{ FieldType::Real, "CameraFieldOfView" },
			{ FieldType::Real, "CameraStiffness" },
			{ FieldType::ShortInteger, "Flags3" },
			{ FieldType::ShortInteger, "Unknown6" },
			{ FieldType::StringID, "CameraMarkerName" },
			{ FieldType::StringID, "CameraSubmergedMarkerName" },
			{ FieldType::Real, "PitchAutoLevel" },
			{ FieldType::Real, "PitchRangeMin" },
			{ FieldType::Real, "PitchRangeMax" },
			{ FieldType::Block, "CameraTracks", &CameraTracksStruct },
			{ FieldType::Real, "Unknown7" },
			{ FieldType::Real, "Unknown8" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Block, "Unknown11", &Unknown10Struct },
			{ FieldType::ShortInteger, "Flags4" },
			{ FieldType::ShortInteger, "Unknown12" },
			{ FieldType::StringID, "CameraMarkerName2" },
			{ FieldType::StringID, "CameraSubmergedMarkerName2" },
			{ FieldType::Real, "PitchAutoLevel2" },
			{ FieldType::Real, "PitchRangeMin2" },
			{ FieldType::Real, "PitchRangeMax2" },
			{ FieldType::Block, "CameraTracks2", &CameraTracksStruct },
			{ FieldType::Real, "Unknown13" },
			{ FieldType::Real, "Unknown14" },
			{ FieldType::Real, "Unknown15" },
			{ FieldType::Block, "Unknown17", &Unknown16Struct },
			{ FieldType::TagReference, "AssassinationResponse" },
			{ FieldType::TagReference, "AssassinationWeapon" },
			{ FieldType::StringID, "AssasinationToolStowAnchor" },
			{ FieldType::StringID, "AssasinationToolHandMarker" },
			{ FieldType::StringID, "AssasinationToolMarker" },
			{ FieldType::Real, "AccelerationRangeI" },
			{ FieldType::Real, "AccelerationRangeJ" },
			{ FieldType::Real, "AccelerationRangeK" },
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
			{ FieldType::Real, "AimingVelocityMaximum" },
			{ FieldType::Real, "AimingAccelerationMaximum" },
			{ FieldType::Real, "CasualAimingModifier" },
			{ FieldType::Real, "LookingVelocityMaximum" },
			{ FieldType::Real, "LookingAccelerationMaximum" },
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
			{ FieldType::ShortInteger, "MotionSensorBlipSize" },
			{ FieldType::ShortInteger, "ItemScale" },
			{ FieldType::Block, "Postures", &PosturesStruct },
			{ FieldType::Block, "HudInterfaces", &HudInterfacesStruct },
			{ FieldType::Block, "DialogueVariants", &DialogueVariantsStruct },
			{ FieldType::Real, "Unknown18" },
			{ FieldType::Real, "Unknown19" },
			{ FieldType::Real, "Unknown20" },
			{ FieldType::Real, "Unknown21" },
			{ FieldType::Real, "GrenadeVelocity" },
			{ FieldType::ShortInteger, "GrenadeType" },
			{ FieldType::ShortInteger, "GrenadeCount" },
			{ FieldType::Block, "PoweredSeats", &PoweredSeatsStruct },
			{ FieldType::Block, "Weapons", &WeaponsStruct },
			{ FieldType::Block, "TargetTracking2", &TargetTrackingStruct },
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
			{ FieldType::LongInteger, "Flags5" },
			{ FieldType::Block, "TankEngineMotionProperties", &TankEngineMotionPropertiesStruct },
			{ FieldType::Block, "EngineMotionProperties", &EngineMotionPropertiesStruct },
			{ FieldType::Block, "DropshipMotionProperties", &DropshipMotionPropertiesStruct },
			{ FieldType::Block, "AntigravityMotionProperties", &AntigravityMotionPropertiesStruct },
			{ FieldType::Block, "JetEngineMotionProperties", &JetEngineMotionPropertiesStruct },
			{ FieldType::Block, "TurretProperties", &TurretPropertiesStruct },
			{ FieldType::DwordInteger, "Unknown22" },
			{ FieldType::DwordInteger, "Unknown23" },
			{ FieldType::DwordInteger, "Unknown24" },
			{ FieldType::Block, "HelicopterMotionProperties", &HelicopterMotionPropertiesStruct },
			{ FieldType::Block, "AntigravityEngineMotionProperties", &AntigravityEngineMotionPropertiesStruct },
			{ FieldType::Block, "AutoturretEquipment2", &AutoturretEquipmentStruct },
			{ FieldType::LongInteger, "Flags6" },
			{ FieldType::Real, "GroundFriction" },
			{ FieldType::Real, "GroundDepth" },
			{ FieldType::Real, "GroundDampFactor" },
			{ FieldType::Real, "GroundMovingFriction" },
			{ FieldType::Real, "GroundMaximumSlope0" },
			{ FieldType::Real, "GroundMaximumSlope1LessThanSlope0" },
			{ FieldType::Real, "Unknown25" },
			{ FieldType::Real, "AntiGravityBankLift" },
			{ FieldType::Real, "SteeringBankReactionScale" },
			{ FieldType::Real, "GravityScale" },
			{ FieldType::Real, "Radius" },
			{ FieldType::Real, "Unknown26" },
			{ FieldType::Real, "Unknown27" },
			{ FieldType::Real, "Unknown28" },
			{ FieldType::Block, "AntiGravityPoints", &AntiGravityPointsStruct },
			{ FieldType::Block, "FrictionPoints", &FrictionPointsStruct },
			{ FieldType::Block, "PhantomShapes", &PhantomShapesStruct },
			{ FieldType::CharInteger, "PlayerTrainingVehicleType" },
			{ FieldType::CharInteger, "VehicleSize" },
			{ FieldType::CharInteger, "Unknown29" },
			{ FieldType::CharInteger, "Unknown30" },
			{ FieldType::Real, "MinimumFlippingAngularVelocity" },
			{ FieldType::Real, "MaximumFlippingAngularVelocity" },
			{ FieldType::DwordInteger, "Unknown31" },
			{ FieldType::DwordInteger, "Unknown32" },
			{ FieldType::Real, "SeatEntranceAccelerationScale" },
			{ FieldType::Real, "SeatExitAccelerationScale" },
			{ FieldType::Real, "FlipTime" },
			{ FieldType::StringID, "FlipOverMessage" },
			{ FieldType::TagReference, "SuspensionSound" },
			{ FieldType::TagReference, "RunningEffect" },
			{ FieldType::TagReference, "UnknownResponse" },
			{ FieldType::TagReference, "UnknownResponse2" },
			{ FieldType::DwordInteger, "Unknown33" },
			{ FieldType::DwordInteger, "Unknown34" }
		}
	};
}