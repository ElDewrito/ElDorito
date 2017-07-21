#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "GameVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Text::StringID;

	struct TerritoriesVariant : GameVariant
	{
		int Unknown58;
		int Unknown5C;
		int Unknown60;
		int Unknown64;
		int Unknown68;
	};
	TAG_STRUCT_SIZE_ASSERT(struct TerritoriesVariant, 0x6C);
}
