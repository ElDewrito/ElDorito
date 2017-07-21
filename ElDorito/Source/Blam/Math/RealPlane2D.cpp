#include "RealPlane2D.hpp"

namespace Blam::Math
{
	RealPlane2D::RealPlane2D()
		: RealPlane2D(RealVector2D(), 0.0f)
	{
	}

	RealPlane2D::RealPlane2D(const RealVector2D &normal, const float distance)
		: Normal(normal), Distance(distance)
	{
	}

	RealPlane2D::RealPlane2D(const float i, const float j, const float d)
		: Normal(RealVector2D(i, j)), Distance(d)
	{
	}

	bool RealPlane2D::operator==(const RealPlane2D &other) const
	{
		return Normal == other.Normal
			&& Distance == other.Distance;
	}

	bool RealPlane2D::operator!=(const RealPlane2D &other) const
	{
		return !(*this == other);
	}
}

