#pragma once
#include <cstdint>
#include "../Padding.hpp"

namespace Blam::Tags
{
	template <typename Element>
	struct TagBlock
	{
		int32_t Count;
		Element *Elements;
		PAD32;

		inline TagBlock(const int32_t count, Element *const elements)
			: Count(count), Elements(elements)
		{
		}

		inline TagBlock()
			: TagBlock(0, nullptr)
		{
		}

		inline Element *operator->() const
		{
			return Elements;
		}

		inline Element &operator[](const size_t index) const
		{
			return Elements[index];
		}

		inline Element *begin() const
		{
			return Elements;
		}

		inline Element *end() const
		{
			return &Elements[Count];
		}

		inline explicit operator bool() const
		{
			return Elements != nullptr;
		}
	};
}
