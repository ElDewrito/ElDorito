#pragma once

namespace Blam
{
	namespace Math
	{
		struct RealVector3D
		{
			float X;
			float Y;
			float Z;

			RealVector3D();
			RealVector3D(const float x, const float y, const float z);

			bool operator==(const RealVector3D &other) const;
			bool operator!=(const RealVector3D &other) const;
		};
	}
}
