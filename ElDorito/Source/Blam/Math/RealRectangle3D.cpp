#include "RealRectangle3D.hpp"

namespace Blam::Math
{
	RealRectangle3D::RealRectangle3D()
		: RealRectangle3D(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	RealRectangle3D::RealRectangle3D(const float top, const float bottom, const float left, const float right, const float front, const float back)
		: Top(top), Bottom(bottom), Left(left), Right(right), Front(front), Back(back)
	{
	}

	bool RealRectangle3D::operator==(const RealRectangle3D &other) const
	{
		return Top == other.Top
			&& Bottom == other.Bottom
			&& Left == other.Left
			&& Right == other.Right
			&& Front == other.Front
			&& Back == other.Back;
	}

	bool RealRectangle3D::operator!=(const RealRectangle3D &other) const
	{
		return !(*this == other);
	}
}

