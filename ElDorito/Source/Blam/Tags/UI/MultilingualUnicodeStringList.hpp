#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::UI
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Text::StringID;

	struct MultilingualUnicodeStringList : TagGroup<'unic'>
	{
		struct LocalizedString;

		TagBlock<LocalizedString> Strings;
		TagData<int8_t> Data;

		struct LocalizedString
		{
			StringID StringID;
			char StringIDStr[32]; //This should really be stored as a string.
			int32_t Offsets[12];
		};
		TAG_STRUCT_SIZE_ASSERT(LocalizedString, 0x54);
	};
}
