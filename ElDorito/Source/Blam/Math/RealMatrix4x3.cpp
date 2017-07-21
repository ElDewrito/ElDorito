#include "RealMatrix4x3.hpp"

namespace Blam::Math
{
	RealMatrix4x3::RealMatrix4x3()
		: RealMatrix4x3(0.0f, RealVector3D(), RealVector3D(), RealVector3D(), RealVector3D())
	{
	}

	RealMatrix4x3::RealMatrix4x3(const float scale, const RealVector3D &forward, const RealVector3D &left, const RealVector3D &up, const RealVector3D &position)
		: Scale(scale), Forward(forward), Left(left), Up(up), Position(position)
	{
	}

	bool RealMatrix4x3::operator==(const RealMatrix4x3 &other) const
	{
		return Scale == other.Scale
			&& Forward == other.Forward
			&& Left == other.Left
			&& Up == other.Up
			&& Position == other.Position;
	}

	bool RealMatrix4x3::operator!=(const RealMatrix4x3 &other) const
	{
		return !(*this == other);
	}

	RealMatrix4x3::operator const float *() const
	{
		return &Scale;
	}
}

