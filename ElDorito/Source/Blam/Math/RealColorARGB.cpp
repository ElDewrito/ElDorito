#include "RealColorARGB.hpp"

namespace Blam::Math
{
	RealColorARGB::RealColorARGB()
		: RealColorARGB(1.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	RealColorARGB::RealColorARGB(const float alpha, const float red, const float green, const float blue)
		: Alpha(alpha), Red(red), Green(green), Blue(blue)
	{
	}

	bool RealColorARGB::operator==(const RealColorARGB &other) const
	{
		return Alpha == other.Alpha
			&& Red == other.Red
			&& Green == other.Green
			&& Blue == other.Blue;
	}

	bool RealColorARGB::operator!=(const RealColorARGB &other) const
	{
		return !(*this == other);
	}

	RealColorARGB::operator const float *() const
	{
		return &Alpha;
	}
}

