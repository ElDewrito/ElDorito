#pragma once

namespace Blam::Math
{
	struct RealRectangle2D
	{
		float Top;
		float Bottom;
		float Left;
		float Right;

		RealRectangle2D();
		RealRectangle2D(const float top, const float bottom, const float left, const float right);

		bool operator==(const RealRectangle2D &other) const;
		bool operator!=(const RealRectangle2D &other) const;
	};
}

