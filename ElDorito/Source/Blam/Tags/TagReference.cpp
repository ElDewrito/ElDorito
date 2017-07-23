#include "TagReference.hpp"

namespace Blam::Tags
{
	TagReference::TagReference()
		: TagReference((Tag)-1, -1)
	{
	}

	TagReference::TagReference(const Tag &groupTag, const int32_t tagIndex)
		: GroupTag(groupTag), TagIndex(tagIndex)
	{
	}

	bool TagReference::operator==(const TagReference &other) const
	{
		return GroupTag == other.GroupTag
			&& TagIndex == other.TagIndex;
	}

	bool TagReference::operator!=(const TagReference &other) const
	{
		return !(*this == other);
	}

	TagReference::operator bool() const
	{
		return GroupTag != (Tag)-1
			&& TagIndex != -1;
	}
}
