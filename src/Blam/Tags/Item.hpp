#pragma once
#include "Object.hpp"

namespace Blam
{
	namespace Tags
	{
		struct Item : Object, Tag<'item'>
		{
			struct PredictedBitmap;

			uint32_t Flags2;
			int16_t OldMessageIndex;
			int16_t SortOrder;
			float OldMultiplayerOnGroundScale;
			float OldCampaignOnGroundScale;
			int32_t PickupMessage;
			int32_t SwapMessage;
			int32_t PickupOrDualWieldMessage;
			int32_t SwapOrDualWieldMessage;
			int32_t PickedUpMessage;
			int32_t SwitchToMessage;
			int32_t SwitchToFromAiMessage;
			int32_t AllWeaponsEmptyMessage;
			TagReference CollisionSound;
			TagBlock<PredictedBitmap> PredictedBitmaps;
			TagReference DetonationDamageEffect;
			float DetonationDelayMin;
			float DetonationDelayMax;
			TagReference DetonatingEffect;
			TagReference DetonationEffect;
			float CampaignGroundScale;
			float MultiplayerGroundScale;
			float SmallHoldScale;
			float SmallHolsterScale;
			float MediumHoldScale;
			float MediumHolsterScale;
			float LargeHoldScale;
			float LargeHolsterScale;
			float HugeHoldScale;
			float HugeHolsterScale;
			float GroundedFrictionLength;
			float GroundedFrictionUnknown;

			struct PredictedBitmap
			{
				TagReference Bitmap;
			};
			TAG_STRUCT_SIZE_ASSERT(PredictedBitmap, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(Item, 0x1D4);
	}
}
