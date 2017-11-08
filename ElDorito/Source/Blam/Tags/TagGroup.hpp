#pragma once
#include "Blam\Tags\Tag.hpp"
#include "Definitions\StructDefinition.hpp"

namespace Blam::Tags
{
	template <const Tag GroupTagValue>
	struct TagGroup
	{
		static const Tag GroupTag = GroupTagValue;
		static const Definitions::StructDefinition Definition;
	};
}
