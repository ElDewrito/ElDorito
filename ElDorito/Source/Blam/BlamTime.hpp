#pragma once

#include <cstdint>

namespace Blam::Time
{
	struct GameTimeGlobals
	{
		bool IsInitialized;
		uint8_t Unknown01;
		uint16_t Flags;
		uint16_t TicksPerSecond;
		uint16_t Unknown06;
		float SecondsPerTick;
		uint32_t ElapsedTicks;
		float GameSpeed;
		// ...
	};

	static inline __declspec(naked) GameTimeGlobals * Get()
	{
		__asm
		{
			push ebp
			mov eax, dword ptr fs : [2Ch]
			mov eax, dword ptr ds : [eax]
			mov eax, [eax + 0x50]
			pop ebp
			ret
		}
	}

	static inline uint32_t GetGameTicks()
	{
		return Get()->ElapsedTicks;
	}

	static inline float TicksToSeconds(uint32_t ticks)
	{
		return Get()->SecondsPerTick * ticks;
	}

	static inline float GetSecondsPerTick()
	{
		return Get()->SecondsPerTick;
	}

	static inline uint32_t SecondsToTicks(float seconds)
	{
		return uint32_t(Get()->TicksPerSecond * seconds);
	}
}
