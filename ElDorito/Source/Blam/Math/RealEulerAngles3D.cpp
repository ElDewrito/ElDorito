#include "RealEulerAngles3D.hpp"

namespace Blam::Math
{
	RealEulerAngles3D::RealEulerAngles3D()
		: RealEulerAngles3D(Angle(), Angle(), Angle())
	{
	}

	RealEulerAngles3D::RealEulerAngles3D(const Angle &yaw, const Angle &pitch, const Angle &roll)
		: Yaw(yaw), Pitch(pitch), Roll(roll)
	{
	}

	bool RealEulerAngles3D::operator==(const RealEulerAngles3D &other) const
	{
		return Yaw == other.Yaw
			&& Pitch == other.Pitch
			&& Roll == other.Roll;
	}

	bool RealEulerAngles3D::operator!=(const RealEulerAngles3D &other) const
	{
		return !(*this == other);
	}
}

