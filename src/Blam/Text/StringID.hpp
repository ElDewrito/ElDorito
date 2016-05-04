#pragma once
#include <cstdint>

namespace Blam
{
	namespace Text
	{
		struct StringID
		{
			static const StringID Null;

			uint32_t Value;

			StringID();
			StringID(const uint32_t value);
			StringID(const int32_t set, const int32_t index);
			StringID(const int32_t length, const int32_t set, const int32_t index);

			bool operator==(const StringID &other) const;
			bool operator!=(const StringID &other) const;

			explicit operator bool() const;
			explicit operator uint32_t() const;
		};
	}
}
