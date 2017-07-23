#pragma once

namespace Blam::Math
{
	struct RealPoint3D
	{
		float X;
		float Y;
		float Z;

		RealPoint3D();
		RealPoint3D(const float x, const float y, const float z);

		bool operator==(const RealPoint3D &other) const;
		bool operator!=(const RealPoint3D &other) const;

		explicit operator const float *() const;

		RealPoint3D &operator+=(const RealPoint3D &other);
		RealPoint3D &operator+=(const float other);
		RealPoint3D operator+(const RealPoint3D &other) const;
		RealPoint3D operator+(const float other) const;
		friend RealPoint3D operator+(const float a, const RealPoint3D &b);

		RealPoint3D &operator-=(const RealPoint3D &other);
		RealPoint3D &operator-=(const float other);
		RealPoint3D operator-(const RealPoint3D &other) const;
		RealPoint3D operator-(const float other) const;
		friend RealPoint3D operator-(const float a, const RealPoint3D &b);

		RealPoint3D &operator*=(const RealPoint3D &other);
		RealPoint3D &operator*=(const float other);
		RealPoint3D operator*(const RealPoint3D &other) const;
		RealPoint3D operator*(const float other) const;
		friend RealPoint3D operator*(const float a, const RealPoint3D &b);

		RealPoint3D &operator/=(const RealPoint3D &other);
		RealPoint3D &operator/=(const float other);
		RealPoint3D operator/(const RealPoint3D &other) const;
		RealPoint3D operator/(const float other) const;
		friend RealPoint3D operator/(const float a, const RealPoint3D &b);
	};
}

