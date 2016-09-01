#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include <string>

namespace Blam
{
	namespace Tags
	{
		namespace UI
		{
			using Blam::Tags::TagBlock;
			using Blam::Tags::TagData;
			using Blam::Tags::TagGroup;
			using Blam::Tags::TagReference;
			using Blam::Text::StringID;

			struct MultilingualUnicodeStringList : TagGroup<'unic'>
			{
				struct LocalizedString;

				TagBlock<LocalizedString> Strings;
			    TagData<int8_t> Data;

				struct LocalizedString
				{
					StringID StringID;
					int32_t Unknown[8]; //This stuff is actually known, just not mapped out. There's a string in here.
					int32_t Offsets[12];
				};
				TAG_STRUCT_SIZE_ASSERT(LocalizedString, 0x54);
			};
		}
	}
}
