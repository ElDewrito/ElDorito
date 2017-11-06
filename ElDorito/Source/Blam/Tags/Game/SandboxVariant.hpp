#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "GameVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Text::StringID;

	struct SandboxVariant : GameVariant
	{
		int Unknown58;
		int Unknown5C;
		int Unknown60;
	};
	TAG_STRUCT_SIZE_ASSERT(struct SandboxVariant, 0x64);
}

