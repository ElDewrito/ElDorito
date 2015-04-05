#pragma once

#include <inttypes.h>
#include <type_traits>

namespace Utils
{
	// array[5]
	// GetArraySize(array) = 5
	template <typename T, size_t Num>
	inline size_t GetArraySize(T(&a)[Num])
	{
		return Num
	}

	template <typename T>
	inline T Abs(T n)
	{
		return (n >= 0) ? n : -n;
	}

	template <typename T>
	inline T Max(T a, T b)
	{
		return (a >= b) ? a : b;
	}

	template <typename T>
	inline T Min(T a, T b)
	{
		return (a <= b) ? a : b;
	}

	template <typename T>
	inline T Clamp(T n, T low, T high)
	{
		return (n >= high) ? high : ((n <= low) ? low : n);
	}

	inline bool GetBit(size_t n, size_t bit)
	{
		return (n & (1 << bit)) ? true : false;
	}

	template <typename T>
	inline T GetBits(uint32_t n, size_t Position, size_t Length)
	{
		return static_cast<T>((n >> Position) & ((1u << Length) - 1));
	}
}