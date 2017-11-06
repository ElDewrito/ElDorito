#pragma once

namespace Blam::Math
{
	struct RealColorRGB
	{
		float Red;
		float Green;
		float Blue;

		RealColorRGB();
		RealColorRGB(const float red, const float green, const float blue);

		bool operator==(const RealColorRGB &other) const;
		bool operator!=(const RealColorRGB &other) const;

		explicit operator const float *() const;
	};
}

