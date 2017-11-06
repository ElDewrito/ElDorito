#pragma once  
#include "../Tags.hpp"  

namespace Blam::Tags::Globals
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;

	struct CacheFileGlobalTags : TagGroup<'cfgt'>
	{
		struct GlobalsTags;

		TagBlock<GlobalsTags> globalsTags;
		int Unknown;

		struct GlobalsTags
		{
			TagReference globalsTagReference;
		};
		TAG_STRUCT_SIZE_ASSERT(GlobalsTags, 0x10);
	};
	TAG_STRUCT_SIZE_ASSERT(CacheFileGlobalTags, 0x10);
}