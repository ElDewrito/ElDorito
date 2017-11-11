#include "Blam\Tags\Objects\Biped.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Objects;
using namespace Definitions;

namespace Blam::Tags
{
	const EnumDefinition LockOnFlagsValuesEnum =
	{
		"LockOnFlagsValue", EnumType::Long,
		{
			{ "LockedByHumanTargeting", (long)Biped::LockOnFlagsValue::LockedByHumanTargeting },
			{ "LockedByPlasmaTargeting", (long)Biped::LockOnFlagsValue::LockedByPlasmaTargeting },
			{ "AlwaysLockedByHumanTargeting", (long)Biped::LockOnFlagsValue::AlwaysLockedByHumanTargeting },
			{ "Bit3", (long)Biped::LockOnFlagsValue::Bit3 },
			{ "Bit4", (long)Biped::LockOnFlagsValue::Bit4 },
			{ "Bit5", (long)Biped::LockOnFlagsValue::Bit5 },
			{ "Bit6", (long)Biped::LockOnFlagsValue::Bit6 },
			{ "Bit7", (long)Biped::LockOnFlagsValue::Bit7 },
			{ "Bit8", (long)Biped::LockOnFlagsValue::Bit8 },
			{ "Bit9", (long)Biped::LockOnFlagsValue::Bit9 },
			{ "Bit10", (long)Biped::LockOnFlagsValue::Bit10 },
			{ "Bit11", (long)Biped::LockOnFlagsValue::Bit11 },
			{ "Bit12", (long)Biped::LockOnFlagsValue::Bit12 },
			{ "Bit13", (long)Biped::LockOnFlagsValue::Bit13 },
			{ "Bit14", (long)Biped::LockOnFlagsValue::Bit14 },
			{ "Bit15", (long)Biped::LockOnFlagsValue::Bit15 },
			{ "Bit16", (long)Biped::LockOnFlagsValue::Bit16 },
			{ "Bit17", (long)Biped::LockOnFlagsValue::Bit17 },
			{ "Bit18", (long)Biped::LockOnFlagsValue::Bit18 },
			{ "Bit19", (long)Biped::LockOnFlagsValue::Bit19 },
			{ "Bit20", (long)Biped::LockOnFlagsValue::Bit20 },
			{ "Bit21", (long)Biped::LockOnFlagsValue::Bit21 },
			{ "Bit22", (long)Biped::LockOnFlagsValue::Bit22 },
			{ "Bit23", (long)Biped::LockOnFlagsValue::Bit23 },
			{ "Bit24", (long)Biped::LockOnFlagsValue::Bit24 },
			{ "Bit25", (long)Biped::LockOnFlagsValue::Bit25 },
			{ "Bit26", (long)Biped::LockOnFlagsValue::Bit26 },
			{ "Bit27", (long)Biped::LockOnFlagsValue::Bit27 },
			{ "Bit28", (long)Biped::LockOnFlagsValue::Bit28 },
			{ "Bit29", (long)Biped::LockOnFlagsValue::Bit29 },
			{ "Bit30", (long)Biped::LockOnFlagsValue::Bit30 },
			{ "Bit31", (long)Biped::LockOnFlagsValue::Bit31 }
		}
	};

	const StructDefinition WeaponCameraHeightStruct =
	{
		"WeaponCameraHeightBlock", sizeof(Biped::WeaponCameraHeightBlock),
		{
			{ FieldType::StringID, "Class" },
			{ FieldType::Real, "StandingHeightFraction" },
			{ FieldType::Real, "CrouchingHeightFraction" },
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" }
		}
	};

	const StructDefinition UnknownBlock3Struct =
	{
		"UnknownBlock3", sizeof(Biped::UnknownBlock3),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::Data, "Function" }
		}
	};

	const StructDefinition UnknownBlock4Struct =
	{
		"UnknownBlock4", sizeof(Biped::UnknownBlock4),
		{
			{ FieldType::DwordInteger, "Unknown" },
			{ FieldType::DwordInteger, "Unknown2" },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::Data, "Function" }
		}
	};

	const StructDefinition DeadSphereShapesStruct =
	{
		"DeadSphereShape", sizeof(Biped::DeadSphereShape),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::CharInteger, "MaterialIndex" },
			{ FieldType::CharInteger, "Unknown" },
			{ FieldType::ShortInteger, "GlobalMaterialIndex" },
			{ FieldType::Real, "RelativeMassScale" },
			{ FieldType::Real, "Friction" },
			{ FieldType::Real, "Restitution" },
			{ FieldType::Real, "Volume" },
			{ FieldType::Real, "Mass" },
			{ FieldType::ShortInteger, "OverallShapeIndex" },
			{ FieldType::CharInteger, "PhantomIndex" },
			{ FieldType::CharInteger, "InteractionUnknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::ShortInteger, "Size" },
			{ FieldType::ShortInteger, "Count" },
			{ FieldType::LongInteger, "Offset" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::Real, "Radius" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::LongInteger, "Unknown7" },
			{ FieldType::ShortInteger, "Size2" },
			{ FieldType::ShortInteger, "Count2" },
			{ FieldType::LongInteger, "Offset2" },
			{ FieldType::LongInteger, "Unknown8" },
			{ FieldType::Real, "Radius2" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::DwordInteger, "Unknown10" },
			{ FieldType::DwordInteger, "Unknown11" },
			{ FieldType::RealVector3D, "Translation" },
			{ FieldType::Real, "TranslationRadius" }
		}
	};

	const StructDefinition PillShapesStruct =
	{
		"PillShape", sizeof(Biped::PillShape),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::CharInteger, "MaterialIndex" },
			{ FieldType::CharInteger, "Unknown" },
			{ FieldType::ShortInteger, "GlobalMaterialIndex" },
			{ FieldType::Real, "RelativeMassScale" },
			{ FieldType::Real, "Friction" },
			{ FieldType::Real, "Restitution" },
			{ FieldType::Real, "Volume" },
			{ FieldType::Real, "Mass" },
			{ FieldType::ShortInteger, "OverallShapeIndex" },
			{ FieldType::CharInteger, "PhantomIndex" },
			{ FieldType::CharInteger, "InteractionUnknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::ShortInteger, "Size" },
			{ FieldType::ShortInteger, "Count" },
			{ FieldType::LongInteger, "Offset" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::Real, "Radius" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::RealVector3D, "Bottom" },
			{ FieldType::Real, "BottomRadius" },
			{ FieldType::RealVector3D, "Top" },
			{ FieldType::Real, "TopRadius" }
		}
	};

	const StructDefinition SphereShapesStruct =
	{
		"SphereShape", sizeof(Biped::SphereShape),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::CharInteger, "MaterialIndex" },
			{ FieldType::CharInteger, "Unknown" },
			{ FieldType::ShortInteger, "GlobalMaterialIndex" },
			{ FieldType::Real, "RelativeMassScale" },
			{ FieldType::Real, "Friction" },
			{ FieldType::Real, "Restitution" },
			{ FieldType::Real, "Volume" },
			{ FieldType::Real, "Mass" },
			{ FieldType::ShortInteger, "OverallShapeIndex" },
			{ FieldType::CharInteger, "PhantomIndex" },
			{ FieldType::CharInteger, "InteractionUnknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::ShortInteger, "Size" },
			{ FieldType::ShortInteger, "Count" },
			{ FieldType::LongInteger, "Offset" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::Real, "Radius" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::DwordInteger, "Unknown6" },
			{ FieldType::LongInteger, "Unknown7" },
			{ FieldType::ShortInteger, "Size2" },
			{ FieldType::ShortInteger, "Count2" },
			{ FieldType::LongInteger, "Offset2" },
			{ FieldType::LongInteger, "Unknown8" },
			{ FieldType::Real, "Radius2" },
			{ FieldType::DwordInteger, "Unknown9" },
			{ FieldType::DwordInteger, "Unknown10" },
			{ FieldType::DwordInteger, "Unknown11" },
			{ FieldType::RealVector3D, "Translation" },
			{ FieldType::Real, "TranslationRadius" }
		}
	};

	const StructDefinition ContactPointsStruct =
	{
		"ContactPoint", sizeof(Biped::ContactPoint),
		{
			{ FieldType::StringID, "MarkerName" }
		}
	};

	const StructDefinition TagGroup<Biped::GroupTag>::Definition =
	{
		"Biped", sizeof(Biped),
		{
			{ FieldType::Angle, "MovingTurningSpeed" },
			{ FieldType::DwordInteger, "Flags4" },
			{ FieldType::Angle, "StationaryTurningSpeed" },
			{ FieldType::DwordInteger, "Unknown20" },
			{ FieldType::StringID, "Unknown21" },
			{ FieldType::Real, "JumpVelocity" },
			{ FieldType::Real, "MaximumSoftLandingTime" },
			{ FieldType::Real, "MinimumHardLandingTime" },
			{ FieldType::Real, "MinimumSoftLandingVelocity" },
			{ FieldType::Real, "MinimumHardLandingVelocity" },
			{ FieldType::Real, "MaximumHardLandingVelocity" },
			{ FieldType::Real, "DeathHardLandingVelocity" },
			{ FieldType::Real, "StunDuration" },
			{ FieldType::Real, "StationaryStandingCameraHeight" },
			{ FieldType::Real, "MovingStandingCameraHeight" },
			{ FieldType::Real, "StationaryCrouchingCameraHeight" },
			{ FieldType::Real, "MovingCrouchingCameraHeight" },
			{ FieldType::Real, "CrouchTransitionTime" },
			{ FieldType::Data, "CrouchingCameraFunction" },
			{ FieldType::Block, "WeaponCameraHeight", &WeaponCameraHeightStruct },
			{ FieldType::Angle, "CameraInterpolationStart" },
			{ FieldType::Angle, "CameraInterpolationEnd" },
			{ FieldType::DwordInteger, "Unknown22" },
			{ FieldType::DwordInteger, "Unknown23" },
			{ FieldType::DwordInteger, "Unknown24" },
			{ FieldType::DwordInteger, "Unknown25" },
			{ FieldType::Real, "AutoaimWidth" },
			{ FieldType::ByteFlags, "LockonFlags", &LockOnFlagsValuesEnum },
			{ FieldType::DwordInteger, "PhysicsControlNodeIndex" },
			{ FieldType::ShortInteger, "PhysicsControlNodeIndex" },
			{ FieldType::ShortInteger, "Unknown29" },
			{ FieldType::DwordInteger, "Unknown30" },
			{ FieldType::DwordInteger, "Unknown31" },
			{ FieldType::DwordInteger, "Unknown32" },
			{ FieldType::ShortInteger, "PelvisNodeIndex" },
			{ FieldType::ShortInteger, "HeadNodeIndex" },
			{ FieldType::DwordInteger, "Unknown33" },
			{ FieldType::Real, "HeadshotAccelerationScale" },
			{ FieldType::TagReference, "AreaDamageEffect" },
			{ FieldType::Block, "Unknown34", &UnknownBlock3Struct },
			{ FieldType::Block, "Unknown35", &UnknownBlock4Struct },
			{ FieldType::DwordInteger, "Unknown36" },
			{ FieldType::DwordInteger, "Unknown37" },
			{ FieldType::DwordInteger, "Unknown38" },
			{ FieldType::DwordInteger, "Unknown39" },
			{ FieldType::DwordInteger, "Unknown40" },
			{ FieldType::DwordInteger, "Unknown41" },
			{ FieldType::DwordInteger, "Flags5" },
			{ FieldType::Real, "HeightStanding" },
			{ FieldType::Real, "HeightCrouching" },
			{ FieldType::Real, "Radius" },
			{ FieldType::Real, "Mass" },
			{ FieldType::StringID, "LivingMaterialName" },
			{ FieldType::StringID, "DeadMaterialName" },
			{ FieldType::ShortInteger, "LivingMaterialGlobalIndex" },
			{ FieldType::ShortInteger, "DeadMaterialGlobalIndex" },
			{ FieldType::Block, "DeadSphereShapes", &DeadSphereShapesStruct },
			{ FieldType::Block, "PillShapes", &PillShapesStruct },
			{ FieldType::Block, "SphereShapes", &SphereShapesStruct },
			{ FieldType::Angle, "MaximumSlopeAngle" },
			{ FieldType::Angle, "DownhillFalloffAngle" },
			{ FieldType::Angle, "DownhillCutoffAngle" },
			{ FieldType::Angle, "UphillFalloffAngle" },
			{ FieldType::Angle, "UphillCutoffAngle" },
			{ FieldType::Real, "DownhillVelocityScale" },
			{ FieldType::Real, "UphillVelocityScale" },
			{ FieldType::DwordInteger, "Unknown42" },
			{ FieldType::DwordInteger, "Unknown43" },
			{ FieldType::DwordInteger, "Unknown44" },
			{ FieldType::DwordInteger, "Unknown45" },
			{ FieldType::DwordInteger, "Unknown46" },
			{ FieldType::DwordInteger, "Unknown47" },
			{ FieldType::DwordInteger, "Unknown48" },
			{ FieldType::DwordInteger, "Unknown49" },
			{ FieldType::DwordInteger, "Unknown50" },
			{ FieldType::DwordInteger, "Unknown51" },
			{ FieldType::Angle, "BankAngle" },
			{ FieldType::Real, "BankApplyTime" },
			{ FieldType::Real, "BankDecayTime" },
			{ FieldType::Real, "PitchRatio" },
			{ FieldType::Real, "MaximumVelocity" },
			{ FieldType::Real, "MaximumSidestepVelocity" },
			{ FieldType::Real, "Acceleration" },
			{ FieldType::Real, "Deceleration" },
			{ FieldType::Angle, "AngularVelocityMaximum" },
			{ FieldType::Angle, "AngularAccelerationMaximum" },
			{ FieldType::Real, "CrouchVelocityModifier" },
			{ FieldType::Block, "ContactPoints", &ContactPointsStruct },
			{ FieldType::TagReference, "ReanimationCharacter" },
			{ FieldType::TagReference, "TransformationMuffin" },
			{ FieldType::TagReference, "DeathSpawnCharacter" },
			{ FieldType::ShortInteger, "DeathSpawnCount" },
			{ FieldType::ShortInteger, "Unknown52" },
			{ FieldType::Angle, "Unknown53" },
			{ FieldType::Angle, "Unknown54" },
			{ FieldType::Real, "Unknown55" },
			{ FieldType::Real, "Unknown56" },
			{ FieldType::Real, "Unknown57" },
			{ FieldType::Real, "Unknown58" },
			{ FieldType::Real, "Unknown59" },
			{ FieldType::Real, "Unknown60" },
			{ FieldType::Real, "Unknown61" },
			{ FieldType::Real, "Unknown62" },
			{ FieldType::Real, "Unknown63" },
			{ FieldType::Real, "Unknown64" },
			{ FieldType::Real, "Unknown65" },
			{ FieldType::Real, "Unknown66" },
			{ FieldType::Real, "Unknown67" },
			{ FieldType::Real, "Unknown68" },
			{ FieldType::Real, "Unknown69" },
			{ FieldType::Real, "Unknown70" },
			{ FieldType::Real, "Unknown71" },
			{ FieldType::Real, "Unknown72" },
			{ FieldType::Real, "Unknown73" },
			{ FieldType::Real, "Unknown74" },
			{ FieldType::Real, "Unknown75" },
			{ FieldType::Real, "Unknown76" },
			{ FieldType::DwordInteger, "Unknown77" }
		}
	};
}