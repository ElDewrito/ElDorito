#pragma once
#include "RealVector3D.hpp"

namespace Blam::Math
{
	struct RealPlane3D
	{
		RealVector3D Normal;
		float Distance;

		RealPlane3D();
		RealPlane3D(const RealVector3D &normal, const float distance);
		RealPlane3D(const float i, const float j, const float k, const float d);

		bool operator==(const RealPlane3D &other) const;
		bool operator!=(const RealPlane3D &other) const;
	};
}

