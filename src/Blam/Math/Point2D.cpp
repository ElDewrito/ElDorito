#include "Point2D.hpp"

namespace Blam
{
	namespace Math
	{
		Point2D::Point2D()
			: Point2D(0, 0)
		{
		}

		Point2D::Point2D(const int16_t x, const int16_t y)
			: X(x), Y(y)
		{
		}

		bool Point2D::operator==(const Point2D &other) const
		{
			return X == other.X
				&& Y == other.Y;
		}

		bool Point2D::operator!=(const Point2D &other) const
		{
			return !(*this == other);
		}
	}
}
