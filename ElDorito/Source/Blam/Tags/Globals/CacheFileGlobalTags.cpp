#include "Blam\Tags\Globals\CacheFileGlobalTags.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Globals;
using namespace Definitions;

namespace Blam::Tags
{
	const StructDefinition GlobalsTagStruct =
	{
		"GlobalsTag", sizeof(CacheFileGlobalTags::GlobalsTag),
		{
			{ FieldType::TagReference, "Tag" }
		}
	};

	const StructDefinition TagGroup<CacheFileGlobalTags::GroupTag>::Definition =
	{
		"CacheFileGlobalTags", sizeof(CacheFileGlobalTags),
		{
			{ FieldType::Block, "GlobalsTags", &GlobalsTagStruct },
			{ FieldType::LongInteger, "Unknown" }
		}
	};
}