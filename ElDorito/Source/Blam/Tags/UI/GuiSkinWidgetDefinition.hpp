#pragma once
#include <cstdint>
#include <string>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::UI
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct GuiScreenWidgetDefinition : TagGroup<'scn3'>
	{
		struct DataSource;
		struct GroupWidget;
		struct ButtonKeyLegend;

		enum class UnknownFlagValue : int32_t
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

		UnknownFlagValue Flags;
		StringID Name;
		int16_t Unknown1;
		int16_t Layer;
		int16_t WidescreenYBoundsMin;
		int16_t WidescreenXBoundsMin;
		int16_t WidescreenYBoundsMax;
		int16_t WidescreenXBoundsMax;
		int16_t StandardYBoundsMin;
		int16_t StandardXBoundsMin;
		int16_t StandardYBoundsMax;
		int16_t StandardXBoundsMax;
		TagReference Animation;
		TagReference Strings;
		TagReference Parent;
		StringID DefaultKeyLegendString;
		TagBlock<DataSource> DataSources;
		TagBlock<GroupWidget> GroupWidgets;
		TagBlock<ButtonKeyLegend> ButtonKeyLegends;
		TagReference UISounds;
		PAD32; // There's an ascii string here.
		PAD32; // But I'm not sure on the syntax
		PAD32; // to add it to this definition.
		PAD32; // And I don't need it anyway.
		PAD32; //- Alex231
		PAD32;
		PAD32;
		PAD32;
		int16_t ScriptIndex;
		int16_t Unknown2;

		struct DataSource
		{
			TagReference DataSourceReference;
		};
		TAG_STRUCT_SIZE_ASSERT(DataSource, 0x10);

		struct GroupWidget
		{
			struct ListWidget;
			struct TextWidget;
			struct BitmapWidget;
			struct ModelWidget;

			TagReference Parent;
			UnknownFlagValue Flags;
			StringID Name;
			int16_t Unknown;
			int16_t Layer;
			int16_t WidescreenYOffset;
			int16_t WidescreenXOffset;
			int16_t WidescreenYUnknown;
			int16_t WidescreenXUnknown;
			int16_t StandardXOffset;
			int16_t StandardYOffset;
			int16_t StandardXUnknown;
			int16_t StandardYUnknown;
			TagReference Animation;
			TagBlock<ListWidget> ListWidgets;
			TagBlock<TextWidget> TextWidgets;
			TagBlock<BitmapWidget> BitmapWidgets;
			TagBlock<ModelWidget> ModelWidgets;

			struct ListWidget
			{
				struct ListWidgetItem;

				TagReference Parent;
				UnknownFlagValue Flags;
				StringID Name;
				int16_t Unknown1;
				int16_t Layer;
				int16_t WidescreenYOffset;
				int16_t WidescreenXOffset;
				int16_t WidescreenYUnknown;
				int16_t WidescreenXUnknown;
				int16_t StandardXOffset;
				int16_t StandardYOffset;
				int16_t StandardXUnknown;
				int16_t StandardYUnknown;
				TagReference Animation;
				StringID DataSourceName;
				TagReference Skin;
				int32_t Unknown2;
				TagBlock<ListWidgetItem> ListWidgets;
				TagReference UpArrowBitmap;
				TagReference DownArrowBitmap;

				struct ListWidgetItem
				{
					UnknownFlagValue Flags;
					StringID Name;
					int16_t Unknown1;
					int16_t Layer;
					int16_t WidescreenYOffset;
					int16_t WidescreenXOffset;
					int16_t WidescreenYUnknown;
					int16_t WidescreenXUnknown;
					int16_t StandardXOffset;
					int16_t StandardYOffset;
					int16_t StandardXUnknown;
					int16_t StandardYUnknown;
					TagReference Animation;
					StringID Target;
				};
				TAG_STRUCT_SIZE_ASSERT(ListWidgetItem, 0x30);

			};
			TAG_STRUCT_SIZE_ASSERT(ListWidget, 0x80);

			struct TextWidget
			{
				TagReference Parent;
				UnknownFlagValue Flags;
				StringID Name;
				int16_t Unknown1;
				int16_t Layer;
				int16_t WidescreenYOffset;
				int16_t WidescreenXOffset;
				int16_t WidescreenYUnknown;
				int16_t WidescreenXUnknown;
				int16_t StandardXOffset;
				int16_t StandardYOffset;
				int16_t StandardXUnknown;
				int16_t StandardYUnknown;
				TagReference Animation;
				StringID DataSourceName;
				StringID TextString;
				StringID TextColor;
				int16_t TextFont;
				int16_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(TextWidget, 0x4C);

			struct BitmapWidget
			{
				enum class BlendMethodValue : uint16_t
				{
					Standard,
					Unknown1,
					Unknown2,
					Alpha,
					Overlay,
					Unknown3,
					LighterColor,
					Unknown4,
					Unknown5,
					Unknown6,
					InvertedAlpha,
					Unknown7,
					Unknown8,
					Unknown9
				};

				TagReference Parent;
				UnknownFlagValue Flags;
				StringID Name;
				int16_t Unknown1;
				int16_t Layer;
				int16_t WidescreenYOffset;
				int16_t WidescreenXOffset;
				int16_t WidescreenYUnknown;
				int16_t WidescreenXUnknown;
				int16_t StandardXOffset;
				int16_t StandardYOffset;
				int16_t StandardXUnknown;
				int16_t StandardYUnknown;
				TagReference Animation;
				TagReference Bitmap;
				TagReference Unknown2;
				BlendMethodValue BlendMethod;
				int16_t Unknown3;
				int16_t SpriteIndex;
				int16_t Unknown4;
				StringID DataSourceName;
				StringID SpriteDataSourceName;

			};
			TAG_STRUCT_SIZE_ASSERT(BitmapWidget, 0x6C);

			struct ModelWidget
			{
				struct CameraRefinement;

				TagReference Parent;
				UnknownFlagValue Flags;
				StringID Name;
				int16_t Unknown1;
				int16_t Layer;
				int16_t WidescreenYOffset;
				int16_t WidescreenXOffset;
				int16_t WidescreenYUnknown;
				int16_t WidescreenXUnknown;
				int16_t StandardXOffset;
				int16_t StandardYOffset;
				int16_t StandardXUnknown;
				int16_t StandardYUnknown;
				TagReference Animation;
				TagBlock<CameraRefinement> CameraRefinements;

				struct CameraRefinement
				{
					struct ZoomData;

					StringID Biped;
					float Unknown1;
					float Unknown2;
					float Unknown3;
					float Unknown4;
					float Unknown5;
					float Unknown6;
					float Unknown7;
					float Unknown8;
					TagBlock<ZoomData> ZoomData1;
					TagBlock<ZoomData> ZoomData2;

					struct ZoomData
					{
						TagData<uint8_t> Unknown;
					};
					TAG_STRUCT_SIZE_ASSERT(ZoomData, 0x14);
				};
				TAG_STRUCT_SIZE_ASSERT(CameraRefinement, 0x3C);
			};
			TAG_STRUCT_SIZE_ASSERT(BitmapWidget, 0x6C);

		};
		TAG_STRUCT_SIZE_ASSERT(GroupWidget, 0x6C);

		struct ButtonKeyLegend
		{
			TagReference ButtonKeyLegendReference;
		};
		TAG_STRUCT_SIZE_ASSERT(DataSource, 0x10);
	};
	TAG_STRUCT_SIZE_ASSERT(GuiScreenWidgetDefinition, 0xA8);
}