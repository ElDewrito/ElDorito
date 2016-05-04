#pragma once
#include "Tag.hpp"

namespace Blam
{
	namespace Tags
	{
		template <const uint32_t GroupTagValue>
		struct TagGroup
		{
			static const Tag GroupTag;
		};
	}
}
