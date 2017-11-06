#pragma once
#include <cstdint>

namespace Blam::Math
{
	struct ColorARGB
	{
		uint8_t Alpha;
		uint8_t Red;
		uint8_t Green;
		uint8_t Blue;

		ColorARGB();
		ColorARGB(const uint8_t alpha, const uint8_t red, const uint8_t green, const uint8_t blue);

		bool operator==(const ColorARGB &other) const;
		bool operator!=(const ColorARGB &other) const;
	};
}
