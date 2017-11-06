#pragma once
#include "RealPoint3D.hpp"
#include "RealVector3D.hpp"

namespace Blam::Math
{
	struct RealMatrix4x3
	{
		float Scale;
		RealVector3D Forward;
		RealVector3D Left;
		RealVector3D Up;
		RealVector3D Position;

		RealMatrix4x3();
		RealMatrix4x3(const float scale, const RealVector3D &forward, const RealVector3D &left, const RealVector3D &up, const RealVector3D &position);

		bool operator==(const RealMatrix4x3 &other) const;
		bool operator!=(const RealMatrix4x3 &other) const;

		explicit operator const float *() const;
	};
}

