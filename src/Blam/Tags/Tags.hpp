#pragma once

#include <cstdint>
#include <utility>

// Asserts that a tag structure is the correct size.
#define TAG_STRUCT_SIZE_ASSERT(type, size)           \
	static_assert                                    \
	(                                                \
		sizeof(type) == (size),                      \
		"Tag structure " #type " has incorrect size" \
	)

// Padding
#define PAD8 char : 8;
#define PAD16 short : 16;
#define PAD24 char : 8; short : 16;
#define PAD32 int : 32;

namespace Blam
{
	namespace Tags
	{
		// Base struct for a tag. groupTag is the magic constant identifying
		// the corresponding group tag, e.g. 'bipd'
		template<int groupTag>
		struct Tag
		{
			static const int GroupTag = groupTag;
		};

		// A tag block element, which references an array of data.
		template<class T>
		struct TagBlock
		{
			int Count;
			T *Data;
			int UnusedC;

			// Accesses an element by index.
			T& operator[](int index) const { return Data[index]; }

			// Gets a pointer to the first element in the tag block.
			T* begin() const { return &Data[0]; }

			// Gets a pointer past the last element in the tag block.
			T* end() const { return &Data[Count]; }

			// Determines whether the tag block is not null.
			explicit operator bool() const { return Data != nullptr; }
		};
		TAG_STRUCT_SIZE_ASSERT(TagBlock<char>, 0xC);

		// A tag reference element, which references another tag.
		struct TagReference
		{
			int GroupTag;
			int Unused4;
			int Unused8;
			uint32_t Index;

			// Determines whether the tag reference is not null.
			explicit operator bool() const { return Index == 0xFFFFFFFF; }
		};
		TAG_STRUCT_SIZE_ASSERT(TagReference, 0x10);

		// References a raw data buffer.
		template<class T>
		struct DataReference
		{
			uint32_t Size;
			int Unused4;
			int Unused8;
			T *Data;
			int Unused10;

			// Determines whether the data reference is not null.
			explicit operator bool() const { return Data != nullptr; }
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

		// Gets a tag by index.
		template<class TagType>
		TagType* GetTag(uint32_t index)
		{
			static_assert(IsTagType<TagType>::Value, "Cannot call GetTag() on a non-tag type");
			if (index != 0xFFFFFFFF)
			{
				typedef TagType* (*GetTagAddressPtr)(int groupTag, uint32_t index);
				auto GetTagAddress = reinterpret_cast<GetTagAddressPtr>(0x503370);
				return GetTagAddress(TagType::GroupTag, index);
			}
			return nullptr;
		}
	}
}