#include "Angle.hpp"

namespace Blam
{
	namespace Math
	{
		Angle::Angle()
			: Angle(0.0f)
		{
		}

		Angle::Angle(const float radians)
			: Radians(radians)
		{
		}

		bool Angle::operator==(const Angle &other) const
		{
			return Radians == other.Radians;
		}

		bool Angle::operator!=(const Angle &other) const
		{
			return !(*this == other);
		}
	}
}
