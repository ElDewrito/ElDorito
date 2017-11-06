#pragma once

namespace Blam::Math
{
	struct RealPoint2D
	{
		float X;
		float Y;

		RealPoint2D();
		RealPoint2D(const float x, const float y);

		bool operator==(const RealPoint2D &other) const;
		bool operator!=(const RealPoint2D &other) const;

		explicit operator const float *() const;

		RealPoint2D &operator+=(const RealPoint2D &other);
		RealPoint2D &operator+=(const float other);
		RealPoint2D operator+(const RealPoint2D &other) const;
		RealPoint2D operator+(const float other) const;
		friend RealPoint2D operator+(const float a, const RealPoint2D &b);

		RealPoint2D &operator-=(const RealPoint2D &other);
		RealPoint2D &operator-=(const float other);
		RealPoint2D operator-(const RealPoint2D &other) const;
		RealPoint2D operator-(const float other) const;
		friend RealPoint2D operator-(const float a, const RealPoint2D &b);

		RealPoint2D &operator*=(const RealPoint2D &other);
		RealPoint2D &operator*=(const float other);
		RealPoint2D operator*(const RealPoint2D &other) const;
		RealPoint2D operator*(const float other) const;
		friend RealPoint2D operator*(const float a, const RealPoint2D &b);

		RealPoint2D &operator/=(const RealPoint2D &other);
		RealPoint2D &operator/=(const float other);
		RealPoint2D operator/(const RealPoint2D &other) const;
		RealPoint2D operator/(const float other) const;
		friend RealPoint2D operator/(const float a, const RealPoint2D &b);
	};
}

