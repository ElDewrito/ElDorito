#include "ColorRGB.hpp"

namespace Blam::Math
{
	ColorRGB::ColorRGB()
		: ColorRGB(0, 0, 0)
	{
	}

	ColorRGB::ColorRGB(const uint8_t red, const uint8_t green, const uint8_t blue)
		: Red(red), Green(green), Blue(blue)
	{
	}

	bool ColorRGB::operator==(const ColorRGB &other) const
	{
		return Red == other.Red
			&& Green == other.Green
			&& Blue == other.Blue;
	}

	bool ColorRGB::operator!=(const ColorRGB &other) const
	{
		return !(*this == other);
	}
}

