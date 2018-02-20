#include "Blam\Tags\Objects\Projectile.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Objects;
using namespace Definitions;

namespace Blam::Tags
{
	const StructDefinition EarlyMoverPropertiesStruct =
	{
		"EarlyMoverProperty", sizeof(Projectile::EarlyMoverProperty),
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
		"AiProperty", sizeof(Projectile::AiProperty),
		{
			{ FieldType::LongInteger, "Flags" },
			{ FieldType::StringID, "AiTypeName" },
			{ FieldType::ShortInteger, "Size" },
			{ FieldType::ShortInteger, "LeapJumpSpeed" }
		}
	};

	const StructDefinition FunctionsStruct =
	{
		"Function", sizeof(Projectile::Function),
		{
			{ FieldType::LongInteger, "Flags" },
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
		"Attachment", sizeof(Projectile::Attachment),
		{
			{ FieldType::LongInteger, "AtlasFlags" },
			{ FieldType::TagReference, "Tag" },
			{ FieldType::StringID, "Marker" },
			{ FieldType::ShortInteger, "ChangeColor" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::StringID, "PrimaryScale" },
			{ FieldType::StringID, "SecondaryScale" }
		}
	};

	const StructDefinition WidgetsStruct =
	{
		"Widget", sizeof(Projectile::Widget),
		{
			{ FieldType::TagReference, "Type" }
		}
	};

	const StructDefinition ChangeColorsInitialPermutationsStruct =
	{
		"ChangeColor", sizeof(Projectile::ChangeColor::InitialPermutation),
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
		"ChangeColor", sizeof(Projectile::ChangeColor::Function),
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
		"ChangeColor", sizeof(Projectile::ChangeColor),
		{
			{ FieldType::Block, "InitialPermutations", &ChangeColorsInitialPermutationsStruct },
			{ FieldType::Block, "Functions", &ChangeColorsFunctionsStruct }
		}
	};

	const StructDefinition NodeMapsStruct =
	{
		"NodeMap", sizeof(Projectile::NodeMap),
		{
			{ FieldType::CharInteger, "TargetNode" }
		}
	};

	const StructDefinition MultiplayerObjectPropertiesStruct =
	{
		"MultiplayerObjectProperty", sizeof(Projectile::MultiplayerObjectProperty),
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

	const StructDefinition ModelObjectDatumStruct =
	{
		"ModelObjectDatum", sizeof(Projectile::ModelObjectDatum),
		{
			{ FieldType::ShortInteger, "Type" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::Real, "OffsetX" },
			{ FieldType::Real, "OffsetY" },
			{ FieldType::Real, "OffsetZ" },
			{ FieldType::Real, "Radius" }
		}
	};

	const StructDefinition MaterialResponsesStruct =
	{
		"MaterialResponse", sizeof(Projectile::MaterialResponse),
		{
			{ FieldType::WordInteger, "Flags" },
			{ FieldType::ShortInteger, "Response" },
			{ FieldType::StringID, "MaterialName" },
			{ FieldType::ShortInteger, "GlobalMaterialIndex" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::ShortInteger, "Response2" },
			{ FieldType::WordInteger, "Flags2" },
			{ FieldType::Real, "ChanceFraction" },
			{ FieldType::Real, "BetweenAngleMin" },
			{ FieldType::Real, "BetweenAngleMax" },
			{ FieldType::Real, "AndVelocityMin" },
			{ FieldType::Real, "AndVelocityMax" },
			{ FieldType::ShortInteger, "ScaleEffectsBy" },
			{ FieldType::ShortInteger, "Unknown2" },
			{ FieldType::Real, "AngularNoise" },
			{ FieldType::Real, "VelocityNoise" },
			{ FieldType::Real, "InitialFriction" },
			{ FieldType::Real, "MaximumDistance" },
			{ FieldType::Real, "ParallelFriction" },
			{ FieldType::Real, "PerpendicularFriction" }
		}
	};

	const StructDefinition ImpactPropertiesStruct =
	{
		"ImpactProperty", sizeof(Projectile::ImpactProperty),
		{
			{ FieldType::TagReference, "Tag" }
		}
	};

	const StructDefinition Unknown16Struct =
	{
		"Unknown16", sizeof(Projectile::Unknown16),
		{
			{ FieldType::DwordInteger, "Unknown" }
		}
	};

	const StructDefinition ShotgunPropertiesStruct =
	{
		"ShotgunProperty", sizeof(Projectile::ShotgunProperty),
		{
			{ FieldType::ShortInteger, "Amount" },
			{ FieldType::ShortInteger, "Distance" },
			{ FieldType::Real, "Accuracy" },
			{ FieldType::Real, "SpreadConeAngle" }
		}
	};

	const StructDefinition TagGroup<Projectile::GroupTag>::Definition =
	{
		"Projectile", sizeof(Projectile),
		{
			{ FieldType::ShortInteger, "ObjectType" },
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
			{ FieldType::Block, "EarlyMoverProperties", &EarlyMoverPropertiesStruct },
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
			{ FieldType::Block, "EarlyMoverProperties", &NodeMapsStruct },
			{ FieldType::Block, "MultiplayerObjectProperties", &MultiplayerObjectPropertiesStruct },
			{ FieldType::DwordInteger, "Unknown3" },
			{ FieldType::DwordInteger, "Unknown4" },
			{ FieldType::DwordInteger, "Unknown5" },
			{ FieldType::Block, "ModelObjectData", &ModelObjectDatumStruct },
			{ FieldType::LongInteger, "Flags2" },
			{ FieldType::ShortInteger, "DetonationTimerStarts" },
			{ FieldType::ShortInteger, "ImpactNoise" },
			{ FieldType::Real, "CollisionRadius" },
			{ FieldType::Real, "ArmingTime" },
			{ FieldType::Real, "DangerRadius" },
			{ FieldType::Real, "TimerMin" },
			{ FieldType::Real, "TimerMax" },
			{ FieldType::Real, "MinimumVelocity" },
			{ FieldType::Real, "MaximumRange" },
			{ FieldType::Real, "DetonationChargeTime" },
			{ FieldType::ShortInteger, "DetonationNoise" },
			{ FieldType::ShortInteger, "SuperDetonationProjectileCount" },
			{ FieldType::Real, "SuperDetonationDelay" },
			{ FieldType::TagReference, "DetonationStarted" },
			{ FieldType::TagReference, "AirborneDetonationEffect" },
			{ FieldType::TagReference, "GroundDetonationEffect" },
			{ FieldType::TagReference, "DetonationDamage" },
			{ FieldType::TagReference, "AttachedDetonationDamage" },
			{ FieldType::TagReference, "SuperDetonation" },
			{ FieldType::TagReference, "SuperDetonationDamage" },
			{ FieldType::TagReference, "DetonationSound" },
			{ FieldType::CharInteger, "DamageReportingType" },
			{ FieldType::CharInteger, "Unknown6" },
			{ FieldType::CharInteger, "Unknown7" },
			{ FieldType::CharInteger, "Unknown8" },
			{ FieldType::TagReference, "AttachedSuperDetonationDamage" },
			{ FieldType::Real, "MaterialEffectRadius" },
			{ FieldType::TagReference, "FlybySound" },
			{ FieldType::TagReference, "FlybyResponse" },
			{ FieldType::TagReference, "ImpactEffect" },
			{ FieldType::TagReference, "ImpactDamage" },
			{ FieldType::Real, "BoardingDetonationTime" },
			{ FieldType::TagReference, "BoardingDetonationDamage" },
			{ FieldType::TagReference, "BoardingAttachedDetonationDamage" },
			{ FieldType::Real, "AirGravityScale" },
			{ FieldType::Real, "AirDamageRangeMin" },
			{ FieldType::Real, "AirDamageRangeMax" },
			{ FieldType::Real, "WaterGravityScale" },
			{ FieldType::Real, "WaterDamageScaleMin" },
			{ FieldType::Real, "WaterDamageScaleMax" },
			{ FieldType::Real, "InitialVelocity" },
			{ FieldType::Real, "FinalVelocity" },
			{ FieldType::Real, "Unknown9" },
			{ FieldType::Real, "Unknown10" },
			{ FieldType::Real, "GuidedAngularVelocityLower" },
			{ FieldType::Real, "GuidedAngularVelocityUpper" },
			{ FieldType::Real, "Unknown11" },
			{ FieldType::Real, "AccelerationRangeMin" },
			{ FieldType::Real, "AccelerationRangeMax" },
			{ FieldType::Real, "Unknown12" },
			{ FieldType::DwordInteger, "Unknown13" },
			{ FieldType::Real, "TargetedLeadingFraction" },
			{ FieldType::DwordInteger, "Unknown14" },
			{ FieldType::DwordInteger, "Unknown15" },
			{ FieldType::Block, "MaterialResponses", &MaterialResponsesStruct },
			{ FieldType::Block, "ImpactProperties", &ImpactPropertiesStruct },
			{ FieldType::Block, "Unknown17", &Unknown16Struct },
			{ FieldType::Block, "ShotgunProperties", &ShotgunPropertiesStruct },
		}
	};
}