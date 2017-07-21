#include "RealRectangle2D.hpp"

namespace Blam::Math
{
	RealRectangle2D::RealRectangle2D()
		: RealRectangle2D(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	RealRectangle2D::RealRectangle2D(const float top, const float bottom, const float left, const float right)
		: Top(top), Bottom(bottom), Left(left), Right(right)
	{
	}

	bool RealRectangle2D::operator==(const RealRectangle2D &other) const
	{
		return Top == other.Top
			&& Bottom == other.Bottom
			&& Left == other.Left
			&& Right == other.Right;
	}

	bool RealRectangle2D::operator!=(const RealRectangle2D &other) const
	{
		return !(*this == other);
	}
}

