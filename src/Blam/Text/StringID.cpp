#include "StringID.hpp"

namespace Blam
{
	namespace Text
	{
		const StringID StringID::Null;

		StringID::StringID()
			: StringID(0)
		{
		}

		StringID::StringID(const uint32_t value)
			: Value(value)
		{
		}

		StringID::StringID(const int32_t set, const int32_t index)
			: StringID(0, set, index)
		{
		}

		StringID::StringID(const int32_t length, const int32_t set, const int32_t index)
			: StringID((uint32_t)(((length & 0xFF) << 24) | ((set & 0xFF) << 16) | (index & 0xFFFF)))
		{
		}

		bool StringID::operator==(const StringID &other) const
		{
			return Value == other.Value;
		}

		bool StringID::operator!=(const StringID &other) const
		{
			return !(*this == other);
		}

		StringID::operator bool() const
		{
			return Value != 0;
		}

		StringID::operator uint32_t() const
		{
			return Value;
		}
	}
}
