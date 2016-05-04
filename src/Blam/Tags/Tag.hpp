#pragma once
#include <cstdint>

namespace Blam
{
	namespace Tags
	{
		struct Tag
		{
			static const Tag Null;

			uint32_t Value;

			Tag();
			Tag(const uint32_t value);

			bool operator==(const Tag &other) const;
			bool operator!=(const Tag &other) const;

			explicit operator bool() const;
		};
	}
}
