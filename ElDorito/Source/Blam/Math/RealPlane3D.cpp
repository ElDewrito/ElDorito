#include "RealPlane3D.hpp"

namespace Blam::Math
{
	RealPlane3D::RealPlane3D()
		: RealPlane3D(RealVector3D(), 0.0f)
	{
	}

	RealPlane3D::RealPlane3D(const RealVector3D &normal, const float distance)
		: Normal(normal), Distance(distance)
	{
	}

	RealPlane3D::RealPlane3D(const float i, const float j, const float k, const float d)
		: Normal(RealVector3D(i, j, k)), Distance(d)
	{
	}

	bool RealPlane3D::operator==(const RealPlane3D &other) const
	{
		return Normal == other.Normal
			&& Distance == other.Distance;
	}

	bool RealPlane3D::operator!=(const RealPlane3D &other) const
	{
		return !(*this == other);
	}
}

