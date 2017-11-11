#include "Blam\Tags\Items\DefinitionEquipment.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Items;
using namespace Definitions;

namespace Blam::Tags
{
	const StructDefinition CameraTrackStruct =
	{
		"CameraTrack", sizeof(Equipment::CameraTrack),
		{
			{ FieldType::TagReference, "Track" }
		}
	};

	const StructDefinition UnknownStruct =
	{
		"Unknown", sizeof(Equipment::UnknownBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::LongInteger, "Unknown6" },
			{ FieldType::LongInteger, "Unknown7" },
			{ FieldType::LongInteger, "Unknown8" },
			{ FieldType::LongInteger, "Unknown9" },
			{ FieldType::LongInteger, "Unknown10" },
			{ FieldType::LongInteger, "Unknown11" },
			{ FieldType::LongInteger, "Unknown12" },
			{ FieldType::LongInteger, "Unknown13" },
			{ FieldType::LongInteger, "Unknown14" },
			{ FieldType::LongInteger, "Unknown15" },
			{ FieldType::LongInteger, "Unknown16" },
			{ FieldType::LongInteger, "Unknown17" },
			{ FieldType::LongInteger, "Unknown18" },
			{ FieldType::LongInteger, "Unknown19" }
		}
	};

	const StructDefinition EquipmentCameraStruct =
	{
		"EquipmentCamera", sizeof(Equipment::EquipmentCameraBlock),
		{
			{ FieldType::ShortInteger, "Flags" },
			{ FieldType::ShortInteger, "Unknown" },
			{ FieldType::StringID, "CameraMarkerName" },
			{ FieldType::StringID, "CameraSubmergedMarkerName" },
			{ FieldType::Real, "PitchAutoLevel" },
			{ FieldType::Real, "PitchRangeMin" },
			{ FieldType::Real, "PitchRangeMax" },
			{ FieldType::Block, "CameraTrack", &CameraTrackStruct },
			{ FieldType::Real, "Unknown2" },
			{ FieldType::Real, "Unknown3" },
			{ FieldType::Real, "Unknown4" },
			{ FieldType::Block, "Unknown5", &UnknownStruct }
		}
	};

	const StructDefinition HealthPackStruct =
	{
		"PredictedBitmap", sizeof(Equipment::HealthPackBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::Real, "ShieldsGiven" },
			{ FieldType::TagReference, "Unknown3" },
			{ FieldType::TagReference, "Unknown4" },
			{ FieldType::TagReference, "Unknown5" },
		}
	};

	const EnumDefinition PowerupTraitSetValueEnum =
	{
		"PowerupTraitSetValue", EnumType::Long,
		{
			{ "Red", (long)Equipment::PowerupTraitSetValue::Red },
			{ "Blue", (long)Equipment::PowerupTraitSetValue::Blue },
			{ "Yellow", (long)Equipment::PowerupTraitSetValue::Yellow }
		}
	};

	const StructDefinition PowerupStruct =
	{
		"Powerup", sizeof(Equipment::PowerupBlock),
		{
			{ FieldType::LongEnum, "PowerupTraitSet", &PowerupTraitSetValueEnum }
		}
	};

	const StructDefinition ObjectCreationStruct =
	{
		"ObjectCreation", sizeof(Equipment::ObjectCreationBlock),
		{
			{ FieldType::TagReference, "Object" },
			{ FieldType::TagReference, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::Real, "ObjectForce" },
			{ FieldType::LongInteger, "Unknown5" }
		}
	};

	const StructDefinition DestructionStruct =
	{
		"Destruction", sizeof(Equipment::DestructionBlock),
		{
			{ FieldType::TagReference, "DestroyEffect" },
			{ FieldType::TagReference, "DestroyDamageEffect" },
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::Real, "SelfDestructionTime" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" }
		}
	};

	const StructDefinition RadarManipulationStruct =
	{
		"RadarManipulation", sizeof(Equipment::RadarManipulationBlock),
		{
			{ FieldType::LongInteger, "Bitmap" },
			{ FieldType::Real, "Bitmap" },
			{ FieldType::LongInteger, "Bitmap" },
			{ FieldType::LongInteger, "Bitmap" }
		}
	};

	const StructDefinition InvisibilityStruct =
	{
		"Invisibility", sizeof(Equipment::InvisibilityBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" }
		}
	};

	const StructDefinition InvincibilityStruct =
	{
		"Invincibility", sizeof(Equipment::InvincibilityBlock),
		{
			{ FieldType::StringID, "Bitmap" },
			{ FieldType::ShortInteger, "Bitmap" },
			{ FieldType::ShortInteger, "Bitmap" },
			{ FieldType::LongInteger, "Bitmap" },
			{ FieldType::TagReference, "Bitmap" },
			{ FieldType::TagReference, "Unknown2" }
		}
	};

	const StructDefinition RegeneratorStruct =
	{
		"Regenerator", sizeof(Equipment::RegeneratorBlock),
		{
			{ FieldType::TagReference, "RegeneratingEffect" }
		}
	};

	const StructDefinition ForcedReloadStruct =
	{
		"ForcedReload", sizeof(Equipment::ForcedReloadBlock),
		{
			{ FieldType::TagReference, "Effect" },
			{ FieldType::LongInteger, "Unknown" }
		}
	};

	const StructDefinition ConcussiveBlastStruct =
	{
		"ConcussiveBlast", sizeof(Equipment::ConcussiveBlastBlock),
		{
			{ FieldType::TagReference, "Unknown" },
			{ FieldType::TagReference, "Unknown2" }
		}
	};

	const StructDefinition TankModeStruct =
	{
		"TankMode", sizeof(Equipment::TankModeBlock),
		{
			{ FieldType::StringID, "NewPlayerMaterial" },
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::TagReference, "ActiveHud" }
		}
	};

	const StructDefinition MagPulseStruct =
	{
		"MagPulse", sizeof(Equipment::MagPulseBlock),
		{
			{ FieldType::TagReference, "Unknown" },
			{ FieldType::TagReference, "Unknown2" },
			{ FieldType::TagReference, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" }
		}
	};

	const StructDefinition HologramStruct =
	{
		"Hologram", sizeof(Equipment::HologramBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::TagReference, "ActiveEffect" },
			{ FieldType::TagReference, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::TagReference, "DeathEffect" },
			{ FieldType::LongInteger, "Unknown6" },
			{ FieldType::LongInteger, "Unknown7" },
			{ FieldType::Array, FieldType::CharInteger, "Function", 18 },
			{ FieldType::TagReference, "NavPointHud" }
		}
	};

	const StructDefinition ReactiveArmorStruct =
	{
		"ReactiveArmor", sizeof(Equipment::ReactiveArmorBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::TagReference, "Unknown4" },
			{ FieldType::TagReference, "Unknown5" },
			{ FieldType::TagReference, "Unknown6" },
			{ FieldType::TagReference, "Unknown7" }
		}
	};

	const StructDefinition BombRunStruct =
	{
		"BombRun", sizeof(Equipment::BombRunBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::TagReference, "Projectile" },
			{ FieldType::TagReference, "ThrowSound" }
		}
	};

	const StructDefinition ArmorLockStruct =
	{
		"ArmorLock", sizeof(Equipment::ArmorLockBlock),
		{
			{ FieldType::TagReference, "Unknown" },
			{ FieldType::TagReference, "Unknown2" }
		}
	};

	const StructDefinition AdrenalineStruct =
	{
		"Adrenaline", sizeof(Equipment::AdrenalineBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::TagReference, "Unknown2" },
			{ FieldType::TagReference, "Unknown3" }
		}
	};

	const StructDefinition LightningStrikeStruct =
	{
		"LightningStrike", sizeof(Equipment::LightningStrikeBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::TagReference, "Unknown2" }
		}
	};

	const StructDefinition ScramblerStruct =
	{
		"Scrambler", sizeof(Equipment::ScramblerBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::TagReference, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::LongInteger, "Unknown6" }
		}
	};

	const StructDefinition WeaponJammerStruct =
	{
		"WeaponJammer", sizeof(Equipment::WeaponJammerBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::TagReference, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::LongInteger, "Unknown6" }
		}
	};

	const StructDefinition WeaponStruct =
	{
		"Weapon", sizeof(Equipment::WeaponBlock),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::TagReference, "WeaponObject" },
			{ FieldType::LongInteger, "Unknown" }
		}
	};

	const StructDefinition AmmoPackStruct =
	{
		"AmmoPack", sizeof(Equipment::AmmoPackBlock),
		{
			{ FieldType::LongInteger, "Unknown" },
			{ FieldType::LongInteger, "Unknown2" },
			{ FieldType::LongInteger, "Unknown3" },
			{ FieldType::LongInteger, "Unknown4" },
			{ FieldType::LongInteger, "Unknown5" },
			{ FieldType::LongInteger, "Unknown6" },
			{ FieldType::Block, "Weapons", &WeaponStruct },
			{ FieldType::TagReference, "Unknown7" }
		}
	};

	const StructDefinition VisionStruct =
	{
		"PredictedBitmap", sizeof(Equipment::VisionBlock),
		{
			{ FieldType::TagReference, "ScreenEffect" },
			{ FieldType::TagReference, "Unknown" }
		}
	};

	const StructDefinition TagGroup<Equipment::GroupTag>::Definition =
	{
		"Equipment", sizeof(Equipment),
		{
			{ FieldType::Real, "UseDuration" },
			{ FieldType::LongInteger, "Unknown8" },
			{ FieldType::ShortInteger, "NumberOfUses" },
			{ FieldType::WordInteger, "Flags3" },
			{ FieldType::LongInteger, "Unknown9" },
			{ FieldType::LongInteger, "Unknown10" },
			{ FieldType::LongInteger, "Unknown11" },
			{ FieldType::Block, "EquipmentCamera", &EquipmentCameraStruct },
			{ FieldType::Block, "HealthPack", &HealthPackStruct },
			{ FieldType::Block, "Powerup", &PowerupStruct },
			{ FieldType::Block, "ObjectCreation", &ObjectCreationStruct },
			{ FieldType::Block, "Destruction", &DestructionStruct },
			{ FieldType::Block, "RadarManipulation", &RadarManipulationStruct },
			{ FieldType::LongInteger, "Unknown12" },
			{ FieldType::LongInteger, "Unknown13" },
			{ FieldType::LongInteger, "Unknown14" },
			{ FieldType::Block, "Invisibility", &InvisibilityStruct },
			{ FieldType::Block, "Invincibility", &InvincibilityStruct },
			{ FieldType::Block, "Regenerator", &RegeneratorStruct },
			{ FieldType::LongInteger, "Unknown15" },
			{ FieldType::LongInteger, "Unknown16" },
			{ FieldType::LongInteger, "Unknown17" },
			{ FieldType::Block, "ForcedReload", &ForcedReloadStruct },
			{ FieldType::Block, "ConcussiveBlast", &ConcussiveBlastStruct },
			{ FieldType::Block, "TankMode", &TankModeStruct },
			{ FieldType::Block, "MagPulse", &MagPulseStruct },
			{ FieldType::Block, "Hologram", &HologramStruct },
			{ FieldType::Block, "ReactiveArmor", &ReactiveArmorStruct },
			{ FieldType::Block, "BombRun", &BombRunStruct },
			{ FieldType::Block, "ArmorLock", &ArmorLockStruct },
			{ FieldType::Block, "Adrenaline", &AdrenalineStruct },
			{ FieldType::Block, "LightningStrike", &LightningStrikeStruct },
			{ FieldType::Block, "Scrambler", &ScramblerStruct },
			{ FieldType::Block, "WeaponJammer", &WeaponJammerStruct },
			{ FieldType::Block, "AmmoPack", &AmmoPackStruct },
			{ FieldType::Block, "Vision", &VisionStruct },
			{ FieldType::TagReference, "HudInterface" },
			{ FieldType::TagReference, "PickupSound" },
			{ FieldType::TagReference, "EmptySound" },
			{ FieldType::TagReference, "ActivationEffect" },
			{ FieldType::TagReference, "ActiveEffect" },
			{ FieldType::TagReference, "DeactivationEffect" },
			{ FieldType::StringID, "EnterAnimation" },
			{ FieldType::StringID, "IdleAnimation" },
			{ FieldType::StringID, "ExitAnimation" }
		}
	};
}