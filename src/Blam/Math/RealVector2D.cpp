#include "RealVector2D.hpp"

namespace Blam
{
	namespace Math
	{
		RealVector2D::RealVector2D()
			: RealVector2D(0.0f, 0.0f)
		{
		}

		RealVector2D::RealVector2D(const float x, const float y)
			: X(x), Y(y)
		{
		}

		bool RealVector2D::operator==(const RealVector2D &other) const
		{
			return X == other.X
				&& Y == other.Y;
		}

		bool RealVector2D::operator!=(const RealVector2D &other) const
		{
			return !(*this == other);
		}
	}
}
