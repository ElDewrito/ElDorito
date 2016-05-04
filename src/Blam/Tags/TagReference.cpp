#include "TagReference.hpp"

namespace Blam
{
	namespace Tags
	{
		TagReference::TagReference()
			: TagReference(Tag::Null, -1)
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
			return GroupTag != Tag::Null
				&& TagIndex != -1;
		}
	}
}
