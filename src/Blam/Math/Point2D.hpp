#pragma once
#include <cstdint>

namespace Blam
{
	namespace Math
	{
		struct Point2D
		{
			int16_t X;
			int16_t Y;

			Point2D();
			Point2D(const int16_t x, const int16_t y);

			bool operator==(const Point2D &other) const;
			bool operator!=(const Point2D &other) const;
		};
	}
}
