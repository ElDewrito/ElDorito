#pragma once

namespace Blam
{
	namespace Math
	{
		struct RealPoint2D
		{
			float X;
			float Y;

			RealPoint2D();
			RealPoint2D(const float x, const float y);

			bool operator==(const RealPoint2D &other) const;
			bool operator!=(const RealPoint2D &other) const;
		};
	}
}
