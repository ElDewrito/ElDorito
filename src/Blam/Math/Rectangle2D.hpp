#pragma once
#include <cstdint>

namespace Blam
{
	namespace Math
	{
		struct Rectangle2D
		{
			int16_t Top;
			int16_t Left;
			int16_t Bottom;
			int16_t Right;

			Rectangle2D();
			Rectangle2D(const int16_t top, const int16_t left, const int16_t bottom, const int16_t right);

			bool operator==(const Rectangle2D &other) const;
			bool operator!=(const Rectangle2D &other) const;
		};
	}
}
