#pragma once

namespace Blam
{
	namespace Math
	{
		struct Angle
		{
			float Radians;

			Angle();
			Angle(const float radians);

			bool operator==(const Angle &other) const;
			bool operator!=(const Angle &other) const;
		};
	}
}
