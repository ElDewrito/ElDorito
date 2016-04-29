#pragma once

namespace Blam
{
	namespace Math
	{
		struct RealPoint3D
		{
			float X;
			float Y;
			float Z;

			RealPoint3D();
			RealPoint3D(const float x, const float y, const float z);

			bool operator==(const RealPoint3D &other) const;
			bool operator!=(const RealPoint3D &other) const;
		};
	}
}
