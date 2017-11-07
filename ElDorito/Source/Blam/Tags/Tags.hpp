#pragma once
#include "Tag.hpp"
#include "TagBlock.hpp"
#include "TagData.hpp"
#include "TagGroup.hpp"
#include "TagInstance.hpp"
#include "TagReference.hpp"

// Asserts that a tag structure is the correct size.
#define TAG_STRUCT_SIZE_ASSERT(type, size)           \
	static_assert                                    \
	(                                                \
		sizeof(type) == (size),                      \
		"Tag structure " #type " has incorrect size" \
	)