#pragma once

namespace Blam::Math
{
	struct RealColorARGB
	{
		float Alpha;
		float Red;
		float Green;
		float Blue;

		RealColorARGB();
		RealColorARGB(const float alpha, const float red, const float green, const float blue);

		bool operator==(const RealColorARGB &other) const;
		bool operator!=(const RealColorARGB &other) const;

		explicit operator const float *() const;
	};
}

