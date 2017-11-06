#pragma once

namespace Blam::Math
{
	struct RealRectangle3D
	{
		float Top;
		float Bottom;
		float Left;
		float Right;
		float Front;
		float Back;

		RealRectangle3D();
		RealRectangle3D(const float top, const float bottom, const float left, const float right, const float front, const float back);

		bool operator==(const RealRectangle3D &other) const;
		bool operator!=(const RealRectangle3D &other) const;
	};
}

