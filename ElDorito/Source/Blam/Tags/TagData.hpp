#pragma once
#include <cstdint>
#include "../Padding.hpp"

namespace Blam::Tags
{
	template <typename Element>
	struct TagData
	{
		int32_t Size;
		PAD32;
		PAD32;
		Element *Elements;
		PAD32;

		inline TagData(const int32_t size, Element *const elements)
			: Size(size), Elements(elements)
		{
		}

		inline TagData()
			: TagData(0, nullptr)
		{
		}

		inline bool operator==(const TagData<Element> &other) const
		{
			return Size == other.Size
				&& Elements == other.Elements;
		}

		inline bool operator!=(const TagData<Element> &other) const
		{
			return !(*this == other);
		}

		inline explicit operator bool() const
		{
			return Elements != nullptr;
		}
	};
}
