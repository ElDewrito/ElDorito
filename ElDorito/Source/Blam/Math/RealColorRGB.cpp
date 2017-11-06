#include "RealColorRGB.hpp"

namespace Blam::Math
{
	RealColorRGB::RealColorRGB()
		: RealColorRGB(0.0f, 0.0f, 0.0f)
	{
	}

	RealColorRGB::RealColorRGB(const float red, const float green, const float blue)
		: Red(red), Green(green), Blue(blue)
	{
	}

	bool RealColorRGB::operator==(const RealColorRGB &other) const
	{
		return Red == other.Red
			&& Green == other.Green
			&& Blue == other.Blue;
	}

	bool RealColorRGB::operator!=(const RealColorRGB &other) const
	{
		return !(*this == other);
	}

	RealColorRGB::operator const float *() const
	{
		return &Red;
	}
}

