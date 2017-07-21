#include "ColorARGB.hpp"

namespace Blam::Math
{
	ColorARGB::ColorARGB()
		: ColorARGB(255, 0, 0, 0)
	{
	}

	ColorARGB::ColorARGB(const uint8_t alpha, const uint8_t red, const uint8_t green, const uint8_t blue)
		: Alpha(alpha), Red(red), Green(green), Blue(blue)
	{
	}

	bool ColorARGB::operator==(const ColorARGB &other) const
	{
		return Alpha == other.Alpha
			&& Red == other.Red
			&& Green == other.Green
			&& Blue == other.Blue;
	}

	bool ColorARGB::operator!=(const ColorARGB &other) const
	{
		return !(*this == other);
	}
}

