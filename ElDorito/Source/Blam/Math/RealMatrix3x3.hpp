#pragma once
#include "RealVector3D.hpp"

namespace Blam::Math
{
	struct RealMatrix3x3
	{
		RealVector3D Forward;
		RealVector3D Left;
		RealVector3D Up;

		RealMatrix3x3();
		RealMatrix3x3(const RealVector3D &forward, const RealVector3D &left, const RealVector3D &up);

		bool operator==(const RealMatrix3x3 &other) const;
		bool operator!=(const RealMatrix3x3 &other) const;

		explicit operator const float *() const;
	};
}

