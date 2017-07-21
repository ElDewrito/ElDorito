#pragma once
#include "Angle.hpp"

namespace Blam::Math
{
	struct RealEulerAngles2D
	{
		Angle Yaw;
		Angle Pitch;

		RealEulerAngles2D();
		RealEulerAngles2D(const Angle &yaw, const Angle &pitch);

		bool operator==(const RealEulerAngles2D &other) const;
		bool operator!=(const RealEulerAngles2D &other) const;
	};
}

