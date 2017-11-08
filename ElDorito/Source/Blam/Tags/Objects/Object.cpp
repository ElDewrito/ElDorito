#include "Blam\Tags\Objects\Object.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Objects;
using namespace Definitions;

namespace Blam::Tags
{
	const EnumDefinition ObjectTypeEnum =
	{
		"ObjectType", EnumType::Short,
		{
			{ "Biped", (short)Object::TypeValue::Biped },
			{ "Vehicle", (short)Object::TypeValue::Vehicle },
			{ "Weapon", (short)Object::TypeValue::Weapon },
			{ "Equipment", (short)Object::TypeValue::Equipment },
			{ "ARGDevice", (short)Object::TypeValue::ARGDevice },
			{ "Terminal", (short)Object::TypeValue::Terminal },
			{ "Projectile", (short)Object::TypeValue::Projectile },
			{ "Scenery", (short)Object::TypeValue::Scenery },
			{ "Control", (short)Object::TypeValue::Control },
			{ "SoundScenery", (short)Object::TypeValue::SoundScenery },
			{ "Crate", (short)Object::TypeValue::Crate },
			{ "Creature", (short)Object::TypeValue::Creature },
			{ "Giant", (short)Object::TypeValue::Giant },
			{ "EffectScenery", (short)Object::TypeValue::EffectScenery }
		}
	};

	const EnumDefinition ObjectFlagsEnum =
	{
		"ObjectFlags", EnumType::Word,
		{
			{ "DoesNotCastShadow", (unsigned short)Object::FlagsValue::DoesNotCastShadow },
			{ "SearchCardinalDirectionLightmaps", (unsigned short)Object::FlagsValue::SearchCardinalDirectionLightmaps },
			{ "NotAPathfindingObstacle", (unsigned short)Object::FlagsValue::NotAPathfindingObstacle },
			{ "ExtensionOfParent", (unsigned short)Object::FlagsValue::ExtensionOfParent },
			{ "DoesNotCauseCollisionDamage", (unsigned short)Object::FlagsValue::DoesNotCauseCollisionDamage },
			{ "EarlyMover", (unsigned short)Object::FlagsValue::EarlyMover },
			{ "EarlyMoverLocalizedPhysics", (unsigned short)Object::FlagsValue::EarlyMoverLocalizedPhysics },
			{ "UseStaticMassiveLightmapSample", (unsigned short)Object::FlagsValue::UseStaticMassiveLightmapSample },
			{ "ObjectScalesAttachments", (unsigned short)Object::FlagsValue::ObjectScalesAttachments },
			{ "InheritsPlayersAppearance", (unsigned short)Object::FlagsValue::InheritsPlayersAppearance },
			{ "DeadBipedsCannotLocalize", (unsigned short)Object::FlagsValue::DeadBipedsCannotLocalize },
			{ "AttachToClustersByDynamicSphere", (unsigned short)Object::FlagsValue::AttachToClustersByDynamicSphere },
			{ "EffectsDoNotSpawnObjectsInMP", (unsigned short)Object::FlagsValue::EffectDoNotSpawnObjectsInMP }
		}
	};

	const EnumDefinition LightmapShadowModeEnum =
	{
		"LightmapShadowMode", EnumType::Short,
		{
			{ "Default", (short)Object::LightmapShadowModeValue::Default },
			{ "Never", (short)Object::LightmapShadowModeValue::Never },
			{ "Always", (short)Object::LightmapShadowModeValue::Always },
			{ "Blur", (short)Object::LightmapShadowModeValue::Blur }
		}
	};

	const EnumDefinition SweetenerSizeEnum =
	{
		"SweetenerSize", EnumType::Char,
		{
			{ "Small", (char)Object::SweetenerSizeValue::Small },
			{ "Medium", (char)Object::SweetenerSizeValue::Medium },
			{ "Large", (char)Object::SweetenerSizeValue::Large }
		}
	};

	const EnumDefinition WaterDensityEnum =
	{
		"WaterDensity", EnumType::Char,
		{
			{ "Default", (char)Object::WaterDensityValue::Default },
			{ "Least", (char)Object::WaterDensityValue::Least },
			{ "Some", (char)Object::WaterDensityValue::Some },
			{ "Equal", (char)Object::WaterDensityValue::Equal },
			{ "More", (char)Object::WaterDensityValue::More },
			{ "MoreStill", (char)Object::WaterDensityValue::MoreStill },
			{ "LotsMore", (char)Object::WaterDensityValue::LotsMore }
		}
	};

	const StructDefinition EarlyMoverPropertiesStruct =
	{
		"EarlyMoverProperties", sizeof(Object::EarlyMoverProperty),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::DwordInteger, "Unknown1" },
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

	const EnumDefinition AIPropertyFlagsEnum =
	{
		"AIPropertyFlags", EnumType::Long,
		{
			{ "DestroyableCover", (long)Object::AIProperty::Flags::DestroyableCover },
			{ "PathfindingIgnoreWhenDead", (long)Object::AIProperty::Flags::PathfindingIgnoreWhenDead },
			{ "DynamicCover", (long)Object::AIProperty::Flags::DynamicCover }
		}
	};

	const EnumDefinition AIPropertySizeEnum =
	{
		"AIPropertySize", EnumType::Short,
		{
			{ "Default", (short)Object::AIProperty::Size::Default },
			{ "Tiny", (short)Object::AIProperty::Size::Tiny },
			{ "Small", (short)Object::AIProperty::Size::Small },
			{ "Medium", (short)Object::AIProperty::Size::Medium },
			{ "Large", (short)Object::AIProperty::Size::Large },
			{ "Huge", (short)Object::AIProperty::Size::Huge },
			{ "Immobile", (short)Object::AIProperty::Size::Immobile }
		}
	};

	const EnumDefinition AIPropertyLeapJumpSpeedEnum =
	{
		"AIPropertyLeapJumpSpeed", EnumType::Short,
		{
			{ "None", (short)Object::AIProperty::LeapJumpSpeed::None },
			{ "Down", (short)Object::AIProperty::LeapJumpSpeed::Down },
			{ "Step", (short)Object::AIProperty::LeapJumpSpeed::Step },
			{ "Crouch", (short)Object::AIProperty::LeapJumpSpeed::Stand },
			{ "Storey", (short)Object::AIProperty::LeapJumpSpeed::Storey },
			{ "Tower", (short)Object::AIProperty::LeapJumpSpeed::Tower },
			{ "Infinite", (short)Object::AIProperty::LeapJumpSpeed::Infinite }
		}
	};

	const StructDefinition AIPropertiesStruct =
	{
		"AIProperties", sizeof(Object::AIProperty),
		{
			{ FieldType::LongFlags, "Flags", &AIPropertyFlagsEnum },
			{ FieldType::StringID, "AITypeName" },
			{ FieldType::ShortEnum, "Size", &AIPropertySizeEnum },
			{ FieldType::ShortEnum, "LeapJumpSpeed", &AIPropertyLeapJumpSpeedEnum }
		}
	};

	const EnumDefinition FunctionFlagsEnum =
	{
		"FunctionFlags", EnumType::Long,
		{
			{ "Invert", (long)Object::Function::Flags::Invert },
			{ "MappingDoesNotControlsActive", (long)Object::Function::Flags::MappingDoesNotControlsActive },
			{ "AlwaysActive", (long)Object::Function::Flags::AlwaysActive },
			{ "RandomTimeOffset", (long)Object::Function::Flags::RandomTimeOffset }
		}
	};

	const StructDefinition FunctionsStruct =
	{
		"Functions", sizeof(Object::Function),
		{
			{ FieldType::LongFlags, "Flags", &FunctionFlagsEnum },
			{ FieldType::StringID, "ImportName" },
			{ FieldType::StringID, "ExportName" },
			{ FieldType::StringID, "TurnOffWith" },
			{ FieldType::Real, "MinimumValue" },
			{ FieldType::Data, "DefaultFunction" },
			{ FieldType::StringID, "ScaleBy" }
		}
	};

	const EnumDefinition AttachmentFlagsEnum =
	{
		"AttachmentFlags", EnumType::Long,
		{
			{ "GameplayVisionMode", (long)Object::Attachment::Flags::GameplayVisionMode },
			{ "TheaterVisionMode", (long)Object::Attachment::Flags::TheaterVisionMode }
		}
	};

	const EnumDefinition AttachmentChangeColorEnum =
	{
		"AttachmentChangeColor", EnumType::Short,
		{
			{ "None", (short)Object::Attachment::ChangeColor::None },
			{ "Primary", (short)Object::Attachment::ChangeColor::Primary },
			{ "Secondary", (short)Object::Attachment::ChangeColor::Secondary },
			{ "Tertiary", (short)Object::Attachment::ChangeColor::Tertiary },
			{ "Quaternary", (short)Object::Attachment::ChangeColor::Quaternary }
		}
	};

	const StructDefinition AttachmentsStruct =
	{
		"Attachments", sizeof(Object::Attachment),
		{
			{ FieldType::LongFlags, "Flags", &AttachmentFlagsEnum },
			{ FieldType::TagReference, "Attached" },
			{ FieldType::StringID, "Marker" },
			{ FieldType::ShortEnum, "ChangeColor", &AttachmentChangeColorEnum },
			{ FieldType::Pad, 2 },
			{ FieldType::StringID, "PrimaryScale" },
			{ FieldType::StringID, "SecondaryScale" }
		}
	};

	const StructDefinition WidgetsStruct =
	{
		"Widgets", sizeof(Object::Widget),
		{
			{ FieldType::TagReference, "Type" }
		}
	};

	const StructDefinition ChangeColorInitialPermutationsStruct =
	{
		"ChangeColorInitialPermutations", sizeof(Object::ChangeColor::InitialPermutation),
		{
			{ FieldType::Real, "Weight" },
			{ FieldType::RealRgbColor, "ColorLowerBound" },
			{ FieldType::RealRgbColor, "ColorUpperBound" },
			{ FieldType::StringID, "VariantName" }
		}
	};

	const EnumDefinition ChangeColorFunctionScaleFlagsEnum =
	{
		"ChangeColorFunctionScaleFlags", EnumType::Long,
		{
			{ "BlendInHSV", (long)Object::ChangeColor::Function::ScaleFlags::BlendInHSV },
			{ "MoreColors", (long)Object::ChangeColor::Function::ScaleFlags::MoreColors }
		}
	};

	const StructDefinition ChangeColorFunctionsStruct =
	{
		"ChangeColorFunctions", sizeof(Object::ChangeColor::Function),
		{
			{ FieldType::LongFlags, "ScaleFlags", &ChangeColorFunctionScaleFlagsEnum },
			{ FieldType::RealRgbColor, "ColorLowerBound" },
			{ FieldType::RealRgbColor, "ColorUpperBound" },
			{ FieldType::StringID, "DarkenBy" },
			{ FieldType::StringID, "ScaleBy" }
		}
	};

	const StructDefinition ChangeColorsStruct =
	{
		"ChangeColors", sizeof(Object::ChangeColor),
		{
			{ FieldType::Block, "InitialPermutations", &ChangeColorInitialPermutationsStruct },
			{ FieldType::Block, "Functions", &ChangeColorFunctionsStruct }
		}
	};

	const StructDefinition NodeMapsStruct =
	{
		"NodeMaps", sizeof(Object::NodeMap),
		{
			{ FieldType::CharInteger, "TargetNode" }
		}
	};

	const EnumDefinition MultiplayerPropertyEngineFlagsEnum =
	{
		"MultiplayerPropertyEngineFlags", EnumType::Word,
		{
			{ "CTF", (unsigned short)Object::MultiplayerProperty::EngineFlags::CTF },
			{ "Slayer", (unsigned short)Object::MultiplayerProperty::EngineFlags::Slayer },
			{ "Oddball", (unsigned short)Object::MultiplayerProperty::EngineFlags::Oddball },
			{ "KOTH", (unsigned short)Object::MultiplayerProperty::EngineFlags::KOTH },
			{ "Juggernaut", (unsigned short)Object::MultiplayerProperty::EngineFlags::Juggernaut },
			{ "Territories", (unsigned short)Object::MultiplayerProperty::EngineFlags::Territories },
			{ "Assault", (unsigned short)Object::MultiplayerProperty::EngineFlags::Assault },
			{ "VIP", (unsigned short)Object::MultiplayerProperty::EngineFlags::VIP },
			{ "Infection", (unsigned short)Object::MultiplayerProperty::EngineFlags::Infection }
		}
	};

	const EnumDefinition MultiplayerPropertyObjectTypeEnum =
	{
		"MultiplayerPropertyObjectType", EnumType::Char,
		{
			{ "Ordinary", (char)Object::MultiplayerProperty::ObjectType::Ordinary },
			{ "Weapon", (char)Object::MultiplayerProperty::ObjectType::Weapon },
			{ "Grenade", (char)Object::MultiplayerProperty::ObjectType::Grenade },
			{ "Projectile", (char)Object::MultiplayerProperty::ObjectType::Projectile },
			{ "Powerup", (char)Object::MultiplayerProperty::ObjectType::Powerup },
			{ "Equipment", (char)Object::MultiplayerProperty::ObjectType::Equipment },
			{ "LightLandVehicle", (char)Object::MultiplayerProperty::ObjectType::LightLandVehicle },
			{ "HeavyLandVehicle", (char)Object::MultiplayerProperty::ObjectType::HeavyLandVehicle },
			{ "FlyingVehicle", (char)Object::MultiplayerProperty::ObjectType::FlyingVehicle },
			{ "TeleporterTwoWay", (char)Object::MultiplayerProperty::ObjectType::TeleporterTwoWay },
			{ "TeleporterSender", (char)Object::MultiplayerProperty::ObjectType::TeleporterSender },
			{ "TeleporterReceiver", (char)Object::MultiplayerProperty::ObjectType::TeleporterReceiver },
			{ "PlayerSpawnLocation", (char)Object::MultiplayerProperty::ObjectType::PlayerSpawnLocation },
			{ "PlayerRespawnZone", (char)Object::MultiplayerProperty::ObjectType::PlayerRespawnZone },
			{ "HoldSpawnObjective", (char)Object::MultiplayerProperty::ObjectType::HoldSpawnObjective },
			{ "CaptureSpawnObjective", (char)Object::MultiplayerProperty::ObjectType::CaptureSpawnObjective },
			{ "HoldDestinationObjective", (char)Object::MultiplayerProperty::ObjectType::HoldDestinationObjective },
			{ "CaptureDestinationObjective", (char)Object::MultiplayerProperty::ObjectType::CaptureDestinationObjective },
			{ "HillObjective", (char)Object::MultiplayerProperty::ObjectType::HillObjective },
			{ "InfectionHavenObjective", (char)Object::MultiplayerProperty::ObjectType::InfectionHavenObjective },
			{ "TerritoryObjective", (char)Object::MultiplayerProperty::ObjectType::TerritoryObjective },
			{ "VIPBoundaryObjective", (char)Object::MultiplayerProperty::ObjectType::VIPBoundaryObjective },
			{ "VIPDestinationObjective", (char)Object::MultiplayerProperty::ObjectType::VIPDestinationObjective },
			{ "JuggernautDestinationObjective", (char)Object::MultiplayerProperty::ObjectType::JuggernautDestinationObjective }
		}
	};

	const EnumDefinition MultiplayerPropertyTeleporterFlagsEnum =
	{
		"MultiplayerPropertyTeleporterFlags", EnumType::Byte,
		{
			{ "DisallowsPlayers", (char)Object::MultiplayerProperty::TeleporterFlags::DisallowsPlayers },
			{ "AllowsLandVehicles", (char)Object::MultiplayerProperty::TeleporterFlags::AllowsLandVehicles },
			{ "AllowsHeavyVehicles", (char)Object::MultiplayerProperty::TeleporterFlags::AllowsHeavyVehicles },
			{ "AllowsFlyingVehicles", (char)Object::MultiplayerProperty::TeleporterFlags::AllowsFlyingVehicles },
			{ "AllowsProjectiles", (char)Object::MultiplayerProperty::TeleporterFlags::AllowsProjectiles }
		}
	};

	const EnumDefinition MultiplayerPropertyFlagsEnum =
	{
		"MultiplayerPropertyFlags", EnumType::Word,
		{
			{ "EditorOnly", (unsigned short)Object::MultiplayerProperty::Flags::EditorOnly }
		}
	};

	const EnumDefinition MultiplayerPropertyShapeEnum =
	{
		"MultiplayerPropertyShape", EnumType::Char,
		{
			{ "None", (char)Object::MultiplayerProperty::Shape::None },
			{ "Sphere", (char)Object::MultiplayerProperty::Shape::Sphere },
			{ "Cylinder", (char)Object::MultiplayerProperty::Shape::Cylinder },
			{ "Box", (char)Object::MultiplayerProperty::Shape::Box }
		}
	};

	const EnumDefinition MultiplayerPropertySpawnTimerModeEnum =
	{
		"MultiplayerPropertySpawnTimerMode", EnumType::Char,
		{
			{ "OnDeath", (char)Object::MultiplayerProperty::SpawnTimerMode::OnDeath },
			{ "OnDisturbance", (char)Object::MultiplayerProperty::SpawnTimerMode::OnDisturbance }
		}
	};

	const StructDefinition MultiplayerPropertiesStruct =
	{
		"MultiplayerProperties", sizeof(Object::MultiplayerProperty),
		{
			{ FieldType::WordFlags, "EngineFlags", &MultiplayerPropertyEngineFlagsEnum },
			{ FieldType::CharEnum, "ObjectType", &MultiplayerPropertyObjectTypeEnum },
			{ FieldType::ByteFlags, "TeleporterFlags", &MultiplayerPropertyTeleporterFlagsEnum },
			{ FieldType::WordFlags, "Flags", &MultiplayerPropertyFlagsEnum },
			{ FieldType::CharEnum, "Shape", &MultiplayerPropertyShapeEnum },
			{ FieldType::CharEnum, "SpawnTimerMode", &MultiplayerPropertySpawnTimerModeEnum },
			{ FieldType::ShortInteger, "SpawnTime" },
			{ FieldType::ShortInteger, "AbandonTime" },
			{ FieldType::Real, "RadiusWidth" },
			{ FieldType::Real, "Length" },
			{ FieldType::Real, "Top" },
			{ FieldType::Real, "Bottom" },
			{ FieldType::Real, "Unknown1" },
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

	const EnumDefinition ModelObjectDatumTypeEnum =
	{
		"ModelObjectDatumType", EnumType::Short,
		{
			{ "NotSet", (short)Object::ModelObjectDatum::Type::NotSet },
			{ "UserDefined", (short)Object::ModelObjectDatum::Type::UserDefined },
			{ "AutoGenerated", (short)Object::ModelObjectDatum::Type::AutoGenerated }
		}
	};

	const StructDefinition ModelObjectDataStruct =
	{
		"ModelObjectData", sizeof(Object::ModelObjectDatum),
		{
			{ FieldType::ShortEnum, "Type", &ModelObjectDatumTypeEnum },
			{ FieldType::Pad, 2 },
			{ FieldType::RealPoint3D, "Offset" },
			{ FieldType::Real, "Radius" }
		}
	};

	const StructDefinition TagGroup<Object::GroupTag>::Definition =
	{
		"Object", sizeof(Object),
		{
			{ FieldType::ShortEnum, "Type", &ObjectTypeEnum },
			{ FieldType::WordFlags, "Flags", &ObjectFlagsEnum },
			{ FieldType::Real, "BoundingRadius" },
			{ FieldType::RealPoint3D, "BoundingOffset" },
			{ FieldType::Real, "AccelerationScale" },
			{ FieldType::ShortEnum, "LightmapShadowMode", &LightmapShadowModeEnum },
			{ FieldType::CharEnum, "SweetenerSize", &SweetenerSizeEnum },
			{ FieldType::CharEnum, "WaterDensity", &WaterDensityEnum },
			{ FieldType::LongInteger, "RuntimeFlags" },
			{ FieldType::Real, "DynamicLightSphereRadius" },
			{ FieldType::RealPoint3D, "DynamicLightSphereOffset" },
			{ FieldType::StringID, "DefaultModelVariant" },
			{ FieldType::TagReference, "Model" },
			{ FieldType::TagReference, "CrateObject" },
			{ FieldType::TagReference, "CollisionDamage" },
			{ FieldType::Block, "EarlyMoverProperties", &EarlyMoverPropertiesStruct },
			{ FieldType::TagReference, "CreationEffect" },
			{ FieldType::TagReference, "MaterialEffects" },
			{ FieldType::TagReference, "ArmorSounds" },
			{ FieldType::TagReference, "MeleeImpact" },
			{ FieldType::Block, "AIProperties", &AIPropertiesStruct },
			{ FieldType::Block, "Functions", &FunctionsStruct },
			{ FieldType::ShortInteger, "HUDTextMessageIndex" },
			{ FieldType::Pad, 2 },
			{ FieldType::Block, "Attachments", &AttachmentsStruct },
			{ FieldType::Block, "Widgets", &WidgetsStruct },
			{ FieldType::Block, "ChangeColors", &ChangeColorsStruct },
			{ FieldType::Block, "NodeMaps", &NodeMapsStruct },
			{ FieldType::Block, "MultiplayerProperties", &MultiplayerPropertiesStruct },
			{ FieldType::Pad, 12 },
			{ FieldType::Block, "ModelObjectData", &ModelObjectDataStruct }
		}
	};
}