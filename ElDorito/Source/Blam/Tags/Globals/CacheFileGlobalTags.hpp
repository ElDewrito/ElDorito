#pragma once  
#include "../Tags.hpp"  

namespace Blam::Tags::Globals
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;

	struct CacheFileGlobalTags : TagGroup<'cfgt'>
	{
		struct GlobalsTag;

		TagBlock<GlobalsTag> GlobalsTags;
		int Unknown;

		struct GlobalsTag
		{
			TagReference Tag;
		};
		TAG_STRUCT_SIZE_ASSERT(GlobalsTag, 0x10);
	};
	TAG_STRUCT_SIZE_ASSERT(CacheFileGlobalTags, 0x10);
}