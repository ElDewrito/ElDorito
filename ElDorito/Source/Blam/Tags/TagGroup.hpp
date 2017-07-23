#pragma once
#include "Tag.hpp"

namespace Blam::Tags
{
	template <const Tag GroupTagValue>
	struct TagGroup
	{
		static const Tag GroupTag = GroupTagValue;
	};
}
