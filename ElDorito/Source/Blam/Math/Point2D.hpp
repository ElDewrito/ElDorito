#pragma once
#include <cstdint>

namespace Blam::Math
{
	struct Point2D
	{
		int16_t X;
		int16_t Y;

		Point2D();
		Point2D(const int16_t x, const int16_t y);

		bool operator==(const Point2D &other) const;
		bool operator!=(const Point2D &other) const;

		explicit operator const int16_t *() const;

		Point2D &operator+=(const Point2D &other);
		Point2D &operator+=(const int16_t other);
		Point2D operator+(const Point2D &other) const;
		Point2D operator+(const int16_t other) const;
		friend Point2D operator+(const int16_t a, const Point2D &b);

		Point2D &operator-=(const Point2D &other);
		Point2D &operator-=(const int16_t other);
		Point2D operator-(const Point2D &other) const;
		Point2D operator-(const int16_t other) const;
		friend Point2D operator-(const int16_t a, const Point2D &b);

		Point2D &operator*=(const Point2D &other);
		Point2D &operator*=(const int16_t other);
		Point2D operator*(const Point2D &other) const;
		Point2D operator*(const int16_t other) const;
		friend Point2D operator*(const int16_t a, const Point2D &b);

		Point2D &operator/=(const Point2D &other);
		Point2D &operator/=(const int16_t other);
		Point2D operator/(const Point2D &other) const;
		Point2D operator/(const int16_t other) const;
		friend Point2D operator/(const int16_t a, const Point2D &b);
	};
}

