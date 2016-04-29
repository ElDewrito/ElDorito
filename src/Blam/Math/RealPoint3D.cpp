#include "RealPoint3D.hpp"

namespace Blam
{
	namespace Math
	{
		RealPoint3D::RealPoint3D()
			: RealPoint3D(0.0f, 0.0f, 0.0f)
		{
		}

		RealPoint3D::RealPoint3D(const float x, const float y, const float z)
			: X(x), Y(y), Z(z)
		{
		}

		bool RealPoint3D::operator==(const RealPoint3D &other) const
		{
			return X == other.X
				&& Y == other.Y
				&& Z == other.Z;
		}

		bool RealPoint3D::operator!=(const RealPoint3D &other) const
		{
			return !(*this == other);
		}
	}
}
