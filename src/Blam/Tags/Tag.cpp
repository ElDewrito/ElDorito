#include "Tag.hpp"

namespace Blam
{
	namespace Tags
	{
		const Tag Tag::Null;

		Tag::Tag()
			: Tag(0xFFFFFFFF)
		{
		}

		Tag::Tag(const uint32_t value)
			: Value(value)
		{
		}

		bool Tag::operator==(const Tag &other) const
		{
			return Value == other.Value;
		}

		bool Tag::operator!=(const Tag &other) const
		{
			return !(*this == other);
		}
	}
}
