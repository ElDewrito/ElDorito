#pragma once
#include "../Tags/Tag.hpp"
#include "../Tags/TagBlock.hpp"
#include "../Tags/TagData.hpp"
#include "../Tags/TagGroup.hpp"
#include "../Tags/TagInstance.hpp"
#include "../Tags/TagReference.hpp"

// Asserts that a tag structure is the correct size.
#define TAG_STRUCT_SIZE_ASSERT(type, size)           \
	static_assert                                    \
	(                                                \
		sizeof(type) == (size),                      \
		"Tag structure " #type " has incorrect size" \
	)
