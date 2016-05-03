#pragma once

namespace Blam
{
	namespace Math
	{
		struct RealQuaternion
		{
			float I;
			float J;
			float K;
			float W;

			RealQuaternion();
			RealQuaternion(const float i, const float j, const float k, const float w);

			bool operator==(const RealQuaternion &other) const;
			bool operator!=(const RealQuaternion &other) const;
		};
	}
}
