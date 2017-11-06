#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "GameVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Text::StringID;

	struct JuggernautVariant : GameVariant
	{
		int Unknown58;
		int Unknown5C;
		int Unknown60;
		int Unknown64;
		int Unknown68;
		int Unknown6C;
		int Unknown70;
	};
	TAG_STRUCT_SIZE_ASSERT(struct JuggernautVariant, 0x74);
}

