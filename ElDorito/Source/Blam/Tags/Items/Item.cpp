#include "Blam\Tags\Items\Item.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Items;
using namespace Definitions;

namespace Blam::Tags
{
	const EnumDefinition ItemFlagsEnum =
	{
		"ItemFlags", EnumType::Long,
		{
			{ "AlwaysMaintainsZUp", (long)Item::Flags::AlwaysMaintainsZUp },
			{ "DestroyedByExplosions", (long)Item::Flags::DestroyedByExplosions },
			{ "UnaffectedByGravity", (long)Item::Flags::UnaffectedByGravity }
		}
	};

	const StructDefinition PredictedBitmapStruct =
	{
		"PredictedBitmap", sizeof(Item::PredictedBitmap),
		{
			{ FieldType::TagReference, "Bitmap" },
		}
	};

	const StructDefinition TagGroup<Item::GroupTag>::Definition =
	{
		"Item", sizeof(Item),
		{
			{ FieldType::LongFlags, "ItemFlags", &ItemFlagsEnum },
			{ FieldType::Pad, 2 },
			{ FieldType::ShortInteger, "SortOrder" },
			{ FieldType::Pad, 8 },
			{ FieldType::StringID, "PickupMessage" },
			{ FieldType::StringID, "SwapMessage" },
			{ FieldType::StringID, "PickupOrDualWieldMessage" },
			{ FieldType::StringID, "SwapOrDualWieldMessage" },
			{ FieldType::StringID, "PickedUpMessage" },
			{ FieldType::StringID, "SwitchToMessage" },
			{ FieldType::StringID, "SwitchToFromAIMessage" },
			{ FieldType::StringID, "AllWeaponsEmptyMessage" },
			{ FieldType::TagReference, "CollisionSound" },
			{ FieldType::Block, "PredictedBitmaps", &PredictedBitmapStruct },
			{ FieldType::TagReference, "DetonationDamageEffect" },
			{ FieldType::Real, "DetonationDelayLowerBound" },
			{ FieldType::Real, "DetonationDelayUpperBound" },
			{ FieldType::TagReference, "DetonatingEffect" },
			{ FieldType::TagReference, "DetonationEffect" },
			{ FieldType::Real, "GroundScaleSP" },
			{ FieldType::Real, "GroundScaleMP" },
			{ FieldType::Real, "SmallHoldScale" },
			{ FieldType::Real, "SmallHolsterScale" },
			{ FieldType::Real, "MediumHoldScale" },
			{ FieldType::Real, "MediumHolsterScale" },
			{ FieldType::Real, "LargeHoldScale" },
			{ FieldType::Real, "LargeHolsterScale" },
			{ FieldType::Real, "HugeHoldScale" },
			{ FieldType::Real, "HugeHolsterScale" },
			{ FieldType::Real, "GroundedFrictionLength" },
			{ FieldType::Real, "GroundedFrictionUnknown" }
		}
	};
}