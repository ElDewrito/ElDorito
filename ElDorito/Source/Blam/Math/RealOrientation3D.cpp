#include "RealOrientation3D.hpp"

namespace Blam::Math
{
	RealOrientation3D::RealOrientation3D()
		: RealOrientation3D(RealQuaternion(), RealPoint3D(), 0.0f)
	{
	}

	RealOrientation3D::RealOrientation3D(const RealQuaternion &rotation, const RealPoint3D &translation, const float scale)
		: Rotation(rotation), Translation(translation), Scale(scale)
	{
	}

	bool RealOrientation3D::operator==(const RealOrientation3D &other) const
	{
		return Rotation == other.Rotation
			&& Translation == other.Translation
			&& Scale == other.Scale;
	}

	bool RealOrientation3D::operator!=(const RealOrientation3D &other) const
	{
		return !(*this == other);
	}
}

