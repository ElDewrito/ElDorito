#pragma once
#include "Angle.hpp"

namespace Blam::Math
{
	struct RealEulerAngles3D
	{
		Angle Yaw;
		Angle Pitch;
		Angle Roll;

		RealEulerAngles3D();
		RealEulerAngles3D(const Angle &yaw, const Angle &pitch, const Angle &roll);

		bool operator==(const RealEulerAngles3D &other) const;
		bool operator!=(const RealEulerAngles3D &other) const;
	};
}
