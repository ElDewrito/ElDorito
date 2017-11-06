#pragma once

namespace Blam::Math
{
	struct RealVector2D
	{
		float I;
		float J;

		RealVector2D();
		RealVector2D(const float i, const float j);

		bool operator==(const RealVector2D &other) const;
		bool operator!=(const RealVector2D &other) const;

		explicit operator const float *() const;

		RealVector2D &operator+=(const RealVector2D &other);
		RealVector2D &operator+=(const float other);
		RealVector2D operator+(const RealVector2D &other) const;
		RealVector2D operator+(const float other) const;
		friend RealVector2D operator+(const float a, const RealVector2D &b);

		RealVector2D &operator-=(const RealVector2D &other);
		RealVector2D &operator-=(const float other);
		RealVector2D operator-(const RealVector2D &other) const;
		RealVector2D operator-(const float other) const;
		friend RealVector2D operator-(const float a, const RealVector2D &b);

		RealVector2D &operator*=(const RealVector2D &other);
		RealVector2D &operator*=(const float other);
		RealVector2D operator*(const RealVector2D &other) const;
		RealVector2D operator*(const float other) const;
		friend RealVector2D operator*(const float a, const RealVector2D &b);

		RealVector2D &operator/=(const RealVector2D &other);
		RealVector2D &operator/=(const float other);
		RealVector2D operator/(const RealVector2D &other) const;
		RealVector2D operator/(const float other) const;
		friend RealVector2D operator/(const float a, const RealVector2D &b);
	};
}

