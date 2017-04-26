#pragma once

namespace Blam
{
	namespace Time
	{
		const auto GetGameTicks = (int(*)())(0x00564D50);
		const auto TicksToSeconds = (float(__cdecl*) (float ticks))(0x00564C60);
	}
}