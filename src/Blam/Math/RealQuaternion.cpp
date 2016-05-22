#include "RealQuaternion.hpp"

namespace Blam
{
	namespace Math
	{
		RealQuaternion::RealQuaternion()
			: RealQuaternion(0.0f, 0.0f, 0.0f, 1.0f)
		{
		}

		RealQuaternion::RealQuaternion(const float i, const float j, const float k, const float w)
			: I(i), J(j), K(k), W(w)
		{
		}

		bool RealQuaternion::operator==(const RealQuaternion &other) const
		{
			return I == other.I
				&& J == other.J
				&& K == other.K
				&& W == other.W;
		}

		bool RealQuaternion::operator!=(const RealQuaternion &other) const
		{
			return !(*this == other);
		}
	}
}
