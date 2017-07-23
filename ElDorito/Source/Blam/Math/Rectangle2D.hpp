#pragma once
#include <cstdint>

namespace Blam::Math
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

		explicit operator const int16_t *() const;

		Rectangle2D &operator+=(const Rectangle2D &other);
		Rectangle2D &operator+=(const int16_t other);
		Rectangle2D operator+(const Rectangle2D &other) const;
		Rectangle2D operator+(const int16_t other) const;
		friend Rectangle2D operator+(const int16_t a, const Rectangle2D &b);

		Rectangle2D &operator-=(const Rectangle2D &other);
		Rectangle2D &operator-=(const int16_t other);
		Rectangle2D operator-(const Rectangle2D &other) const;
		Rectangle2D operator-(const int16_t other) const;
		friend Rectangle2D operator-(const int16_t a, const Rectangle2D &b);

		Rectangle2D &operator*=(const Rectangle2D &other);
		Rectangle2D &operator*=(const int16_t other);
		Rectangle2D operator*(const Rectangle2D &other) const;
		Rectangle2D operator*(const int16_t other) const;
		friend Rectangle2D operator*(const int16_t a, const Rectangle2D &b);

		Rectangle2D &operator/=(const Rectangle2D &other);
		Rectangle2D &operator/=(const int16_t other);
		Rectangle2D operator/(const Rectangle2D &other) const;
		Rectangle2D operator/(const int16_t other) const;
		friend Rectangle2D operator/(const int16_t a, const Rectangle2D &b);
	};
}

