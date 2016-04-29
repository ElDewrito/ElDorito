#include "RealVector3D.hpp"

namespace Blam
{
	namespace Math
	{
		RealVector3D::RealVector3D()
			: RealVector3D(0.0f, 0.0f, 0.0f)
		{
		}

		RealVector3D::RealVector3D(const float x, const float y, const float z)
			: X(x), Y(y), Z(z)
		{
		}

		bool RealVector3D::operator==(const RealVector3D &other) const
		{
			return X == other.X
				&& Y == other.Y
				&& Z == other.Z;
		}

		bool RealVector3D::operator!=(const RealVector3D &other) const
		{
			return !(*this == other);
		}
	}
}
