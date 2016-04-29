#pragma once
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct ChudDefinition : Tag<'chdt'>
		{
			struct HudWidgetDefinition;

			TagBlock<HudWidgetDefinition> HudWidgets;
			int32_t LowClipCutoff;
			int32_t LowAmmoCutoff;
			int32_t AgeCutoff;

			enum class SpecialHudTypeDefinition : int16_t
			{
				Unspecial,
				Ammo,
				CrosshairAndScope,
				UnitShieldMeter,
				Grenades,
				Gametype,
				MotionSensor,
				SpikeGrenade,
				FirebombGrenade,
				Compass,
				Stamina,
				EnergyMeter,
				Consumable
			};

			enum class StateDataUnknownFlags : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataEngineFlags1 : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataEngineFlags2 : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataEngineFlags3 : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataResolutionFlags : uint16_t
			{
				None = 0,
				WideFull = 1 << 0,
				WideHalf = 1 << 1,
				Bit2 = 1 << 2,
				StandardFull = 1 << 3,
				WideQuarter = 1 << 4,
				StandardHalf = 1 << 5,
				Bit6 = 1 << 6,
				StandardQuarter = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataScoreboardFlags1 : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataScoreboardFlags2 : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataEditorFlags : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			enum class StateDataMetagameFlags : uint16_t
			{
				None = 0,
				Bit0 = 1 << 0,
				Bit1 = 1 << 1,
				Bit2 = 1 << 2,
				Bit3 = 1 << 3,
				Bit4 = 1 << 4,
				Bit5 = 1 << 5,
				Bit6 = 1 << 6,
				Bit7 = 1 << 7,
				Bit8 = 1 << 8,
				Bit9 = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				Bit14 = 1 << 14,
				Bit15 = 1 << 15
			};

			struct StateDataDefinition
			{
				StateDataEngineFlags1 EngineFlags1;
				StateDataEngineFlags2 EngineFlags2;
				StateDataEngineFlags3 EngineFlags3;
				StateDataResolutionFlags ResolutionFlags;
				StateDataScoreboardFlags1 ScoreboardFlags1;
				StateDataScoreboardFlags2 ScoreboardFlags2;
				StateDataUnknownFlags UnknownFlags1;
				StateDataUnknownFlags UnknownFlags2;
				StateDataEditorFlags EditorFlags;
				StateDataMetagameFlags MetagameFlags;
				StateDataUnknownFlags UnknownFlags3;
				StateDataUnknownFlags UnknownFlags4;
				StateDataUnknownFlags UnknownFlags5;
				StateDataUnknownFlags UnknownFlags6;
				StateDataUnknownFlags UnknownFlags7;
				StateDataUnknownFlags UnknownFlags8;
				StateDataUnknownFlags UnknownFlags9;
				StateDataUnknownFlags UnknownFlags10;
				StateDataUnknownFlags UnknownFlags11;
				StateDataUnknownFlags UnknownFlags12;
				StateDataUnknownFlags UnknownFlags13;
				StateDataUnknownFlags UnknownFlags14;
				StateDataUnknownFlags UnknownFlags15;
				StateDataUnknownFlags UnknownFlags16;
				StateDataUnknownFlags UnknownFlags17;
				StateDataUnknownFlags UnknownFlags18;
				StateDataUnknownFlags UnknownFlags19;
				StateDataUnknownFlags UnknownFlags20;
				StateDataUnknownFlags UnknownFlags21;
				StateDataUnknownFlags UnknownFlags22;
				StateDataUnknownFlags UnknownFlags23;
				StateDataUnknownFlags UnknownFlags24;
				StateDataUnknownFlags UnknownFlags25;
				StateDataUnknownFlags UnknownFlags26;
			};
			TAG_STRUCT_SIZE_ASSERT(StateDataDefinition, 0x44);

			enum class PlacementDataAnchorDefinition : int16_t
			{
				TopLeft,
				TopRight,
				BottomRight,
				BottomLeft,
				Center,
				TopEdge,
				GrenadeA,
				GrenadeB,
				GrenadeC,
				GrenadeD,
				ScoreboardFriendly,
				ScoreboardEnemy,
				HealthAndShield,
				BottomEdge,
				Unknown1,
				Equipment,
				Unknown2,
				Depreciated1,
				Depreciated2,
				Depreciated3,
				Depreciated4,
				Depreciated5,
				Unknown3,
				Gametype,
				Unknown4,
				StateRight,
				StateLeft,
				StateCenter,
				Unknown5,
				GametypeFriendly,
				GametypeEnemy,
				MetagameTop,
				MetagamePlayer1,
				MetagamePlayer2,
				MetagamePlayer3,
				MetagamePlayer4,
				Theater
			};

			struct PlacementDataDefinition
			{
				PlacementDataAnchorDefinition Anchor;
				PAD16;
				float MirrorOffsetX;
				float MirrorOffsetY;
				float OffsetX;
				float OffsetY;
				float ScaleX;
				float ScaleY;
			};
			TAG_STRUCT_SIZE_ASSERT(PlacementDataDefinition, 0x1C);

			struct AnimationDataDefinition
			{
				PAD32;
				TagReference Animation1;
				PAD32;
				PAD32;
				TagReference Animation2;
				PAD32;
				PAD32;
				TagReference Animation3;
				PAD32;
				PAD32;
				TagReference Animation4;
				PAD32;
				PAD32;
				TagReference Animation5;
				PAD32;
				PAD32;
				TagReference Animation6;
				PAD32;
			};
			TAG_STRUCT_SIZE_ASSERT(AnimationDataDefinition, 0x90);

			struct RenderDataDefinition
			{
				// TODO: Define this...
				int32_t Unknown0;
				int32_t Unknown4;
				int32_t Unknown8;
				int32_t UnknownC;
				int32_t Unknown10;
				int32_t Unknown14;
				int32_t Unknown18;
				int32_t Unknown1c;
				int32_t Unknown20;
				int32_t Unknown24;
				int32_t Unknown28;
				int32_t Unknown2c;
				int32_t Unknown30;
				int32_t Unknown34;
				int32_t Unknown38;
				int32_t Unknown3c;
				int32_t Unknown40;
				int32_t Unknown44;
			};
			TAG_STRUCT_SIZE_ASSERT(RenderDataDefinition, 0x48);

			enum class BitmapWidgetFlags : uint16_t
			{
				None = 0,
				MirrorHorizontally = 1 << 0,
				MirrorVertically = 1 << 1,
				StretchEdges = 1 << 2,
				EnableTextureCamera = 1 << 3,
				Looping = 1 << 4,
				Bit5 = 1 << 5,
				Player1Emblem = 1 << 6,
				Player2Emblem = 1 << 7,
				Player3Emblem = 1 << 8,
				Player4Emblem = 1 << 9,
				Bit10 = 1 << 10,
				Bit11 = 1 << 11,
				Bit12 = 1 << 12,
				Bit13 = 1 << 13,
				ConsumableControlsSpriteIndex = 1 << 14,
				WeaponControlsSpriteIndex = 1 << 15
			};

			struct BitmapWidgetDefinition
			{
				int32_t NameStringID;
				SpecialHudTypeDefinition SpecialHudType : 16;
				PAD16;
				TagBlock<StateDataDefinition> StateData;
				TagBlock<PlacementDataDefinition> PlacementData;
				TagBlock<AnimationDataDefinition> AnimationData;
				TagBlock<RenderDataDefinition> RenderData;
				int32_t WidgetIndex;
				BitmapWidgetFlags Flags : 16;
				PAD16;
				TagReference Bitmap;
				uint8_t BitmapSpriteIndex;
				PAD24;
			};
			TAG_STRUCT_SIZE_ASSERT(BitmapWidgetDefinition, 0x54);

			enum class TextWidgetFlags : int32_t
			{
				None = 0,
				StringIsANumber = 1 << 0,
				Force2Digit = 1 << 1,
				Force3Digit = 1 << 2,
				PlusSuffix = 1 << 3,
				MSuffix = 1 << 4,
				TenthsDecimal = 1 << 5,
				HundredthsDecimal = 1 << 6,
				ThousandthsDecimal = 1 << 7,
				HundredThousandthsDecimal = 1 << 8,
				OnlyANumber = 1 << 9,
				XSuffix = 1 << 10,
				InBrackets = 1 << 11,
				TimeFormat_S_MS = 1 << 12,
				TimeFormat_H_M_S = 1 << 13,
				MinusPrefix = 1 << 14,
				Bit15 = 1 << 15,
				Bit16 = 1 << 16,
				Bit17 = 1 << 17,
				Bit18 = 1 << 18,
				Bit19 = 1 << 19,
				Bit20 = 1 << 20,
				Bit21 = 1 << 21,
				Bit22 = 1 << 22,
				Bit23 = 1 << 23,
				Bit24 = 1 << 24,
				Bit25 = 1 << 25,
				Bit26 = 1 << 26,
				Bit27 = 1 << 27,
				Bit28 = 1 << 28,
				Bit29 = 1 << 29,
				Bit30 = 1 << 30,
				Bit31 = 1 << 31
			};

			enum class TextWidgetFont : int16_t
			{
				Conduit18,
				Agency16,
				Fixedsys9,
				Conduit14,
				Conduit32,
				Agency32,
				Conduit23,
				Agency18,
				Conduit18_2,
				Conduit16,
				Agency23
			};

			struct TextWidgetDefinition
			{
				int32_t NameStringID;
				SpecialHudTypeDefinition SpecialHudType : 16;
				PAD16;
				TagBlock<StateDataDefinition> StateData;
				TagBlock<PlacementDataDefinition> PlacementData;
				TagBlock<AnimationDataDefinition> AnimationData;
				TagBlock<RenderDataDefinition> RenderData;
				int32_t WidgetIndex;
				TextWidgetFlags Flags : 32;
				TextWidgetFont Font : 16;
				PAD16;
				int32_t TextStringID;
			};
			TAG_STRUCT_SIZE_ASSERT(TextWidgetDefinition, 0x48);

			struct HudWidgetDefinition
			{
				int32_t NameStringID;
				SpecialHudTypeDefinition SpecialHudType;
				PAD16;
				TagBlock<StateDataDefinition> StateData;
				TagBlock<PlacementDataDefinition> PlacementData;
				TagBlock<AnimationDataDefinition> AnimationData;
				TagBlock<RenderDataDefinition> RenderData;
				TagReference ParallaxData;
				TagBlock<BitmapWidgetDefinition> BitmapWidgets;
				TagBlock<TextWidgetDefinition> TextWidgets;
			};
			TAG_STRUCT_SIZE_ASSERT(HudWidgetDefinition, 0x60);
		};
		TAG_STRUCT_SIZE_ASSERT(ChudDefinition, 0x18);
	}
}
