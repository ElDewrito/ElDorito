#pragma once

#include <cstdint>
#include <utility>
#include "../Padding.hpp"

// Asserts that a tag structure is the correct size.
#define TAG_STRUCT_SIZE_ASSERT(type, size)           \
	static_assert                                    \
	(                                                \
		sizeof(type) == (size),                      \
		"Tag structure " #type " has incorrect size" \
	)

namespace Blam
{
	namespace Tags
	{
		// Base struct for a tag. groupTag is the magic constant identifying
		// the corresponding group tag, e.g. 'bipd'
		template<const uint32_t groupTag>
		struct Tag
		{
			static const uint32_t GroupTag = groupTag;
		};

		// A tag block element, which references an array of data.
		template<typename Element>
		struct TagBlock
		{
			int32_t Count;
			Element *Elements;
			PAD32;

			// Accesses an element by index.
			inline Element &operator[](const size_t index) const
			{
				return Elements[index];
			}

			// Gets a pointer to the first element in the tag block.
			inline Element *begin() const
			{
				return &Elements[0];
			}

			// Gets a pointer past the last element in the tag block.
			inline Element *end() const
			{
				return &Elements[Count];
			}

			// Determines whether the tag block is not null.
			explicit operator bool() const
			{
				return Elements != nullptr;
			}
		};
		TAG_STRUCT_SIZE_ASSERT(TagBlock<char>, 0xC);

		// A tag reference element, which references another tag.
		struct TagReference
		{
			int32_t GroupTag;
			PAD32;
			PAD32;
			uint32_t Index;

			// Determines whether the tag reference is not null.
			explicit operator bool() const
			{
				return Index == 0xFFFFFFFF;
			}
		};
		TAG_STRUCT_SIZE_ASSERT(TagReference, 0x10);

		// References a raw data buffer.
		template<typename Element>
		struct DataReference
		{
			int32_t Size;
			PAD32;
			PAD32;
			Element *Data;
			PAD32;

			// Determines whether the data reference is not null.
			explicit operator bool() const
			{
				return Data != nullptr;
			}
		};
		TAG_STRUCT_SIZE_ASSERT(DataReference<char>, 0x14);

		// Statically determines whether a type inherits from Tag<>.
		template<class T>
		struct IsTagType
		{
			typedef char yes[1];
			typedef char no[2];

			template<int U>
			static yes& test(Tag<U> const &);

			static no& test(...);

			static const bool Value = sizeof(test(std::declval<T>())) == sizeof(yes);
		};

		inline int GetGroupTag(uint16_t index)
		{
			typedef int(*GetGroupTagPtr)(uint16_t);
			auto GetGroupTagImpl = reinterpret_cast<GetGroupTagPtr>(0x5033A0);
			return GetGroupTagImpl(index);
		}

		inline void *GetTagAddress(uint32_t group, uint32_t index)
		{
			typedef void *(*GetTagAddressPtr)(int groupTag, uint32_t index);
			auto GetTagAddressImpl = reinterpret_cast<GetTagAddressPtr>(0x503370);
			return GetTagAddressImpl(group, index);
		}

		// Gets a tag by index.
		template<class TagType>
		TagType *GetTag(uint32_t index)
		{
			static_assert(IsTagType<TagType>::Value, "Cannot call GetTag() on a non-tag type");

			if (index != 0xFFFFFFFF)
				return reinterpret_cast<TagType *>(GetTagAddress(TagType::GroupTag, index));

			return nullptr;
		}
	}
}