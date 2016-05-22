#pragma once

namespace Blam
{
	namespace Math
	{
		struct RealVector2D
		{
			float X;
			float Y;

			RealVector2D();
			RealVector2D(const float x, const float y);

			bool operator==(const RealVector2D &other) const;
			bool operator!=(const RealVector2D &other) const;
		};
	}
}
