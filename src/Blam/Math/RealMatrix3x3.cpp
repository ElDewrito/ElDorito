#include "RealMatrix3x3.hpp"

namespace Blam
{
	namespace Math
	{
		RealMatrix3x3::RealMatrix3x3()
			: RealMatrix3x3(RealVector3D(), RealVector3D(), RealVector3D())
		{
		}

		RealMatrix3x3::RealMatrix3x3(const RealVector3D &forward, const RealVector3D &left, const RealVector3D &up)
			: Forward(forward), Left(left), Up(up)
		{
		}

		bool RealMatrix3x3::operator==(const RealMatrix3x3 &other) const
		{
			return Forward == other.Forward
				&& Left == other.Left
				&& Up == other.Up;
		}

		bool RealMatrix3x3::operator!=(const RealMatrix3x3 &other) const
		{
			return !(*this == other);
		}

		RealMatrix3x3::operator const float *() const
		{
			return Forward.operator const float *();
		}
	}
}
