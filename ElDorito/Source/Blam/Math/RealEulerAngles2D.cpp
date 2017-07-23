#include "RealEulerAngles2D.hpp"

namespace Blam::Math
{
	RealEulerAngles2D::RealEulerAngles2D()
		: RealEulerAngles2D(Angle(), Angle())
	{
	}

	RealEulerAngles2D::RealEulerAngles2D(const Angle &yaw, const Angle &pitch)
		: Yaw(yaw), Pitch(pitch)
	{
	}

	bool RealEulerAngles2D::operator==(const RealEulerAngles2D &other) const
	{
		return Yaw == other.Yaw
			&& Pitch == other.Pitch;
	}

	bool RealEulerAngles2D::operator!=(const RealEulerAngles2D &other) const
	{
		return !(*this == other);
	}
}

