#pragma once

#include <cstdint>

namespace Forge::Volumes
{
	const auto KILL_VOLUME_TAG_INDEX = 0x00005A8E;
	const auto GARBAGE_VOLUME_TAG_INDEX = 0x00005A8F;

	void Update();
}
