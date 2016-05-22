#include "Rectangle2D.hpp"

namespace Blam
{
	namespace Math
	{
		Rectangle2D::Rectangle2D()
			: Rectangle2D(0, 0, 0, 0)
		{
		}

		Rectangle2D::Rectangle2D(const int16_t top, const int16_t left, const int16_t bottom, const int16_t right)
			: Top(top), Left(left), Bottom(bottom), Right(right)
		{
		}

		bool Rectangle2D::operator==(const Rectangle2D &other) const
		{
			return Top == other.Top
				&& Left == other.Left
				&& Bottom == other.Bottom
				&& Right == other.Right;
		}

		bool Rectangle2D::operator!=(const Rectangle2D &other) const
		{
			return !(*this == other);
		}
	}
}
