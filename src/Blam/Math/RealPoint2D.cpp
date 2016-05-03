#include "RealPoint2D.hpp"

namespace Blam
{
	namespace Math
	{
		RealPoint2D::RealPoint2D()
			: RealPoint2D(0.0f, 0.0f)
		{
		}

		RealPoint2D::RealPoint2D(const float x, const float y)
			: X(x), Y(y)
		{
		}

		bool RealPoint2D::operator==(const RealPoint2D &other) const
		{
			return X == other.X
				&& Y == other.Y;
		}

		bool RealPoint2D::operator!=(const RealPoint2D &other) const
		{
			return !(*this == other);
		}
	}
}
