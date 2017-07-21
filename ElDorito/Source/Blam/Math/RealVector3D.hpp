#pragma once

namespace Blam::Math
{
	struct RealQuaternion;
	struct RealVector3D
	{
		float I;
		float J;
		float K;

		RealVector3D();
		RealVector3D(const float i, const float j, const float k);

		bool operator==(const RealVector3D &other) const;
		bool operator!=(const RealVector3D &other) const;

		explicit operator const float *() const;

		RealVector3D &operator+=(const RealVector3D &other);
		RealVector3D &operator+=(const float other);
		RealVector3D operator+(const RealVector3D &other) const;
		RealVector3D operator+(const float other) const;
		friend RealVector3D operator+(const float a, const RealVector3D &b);

		RealVector3D &operator-=(const RealVector3D &other);
		RealVector3D &operator-=(const float other);
		RealVector3D operator-(const RealVector3D &other) const;
		RealVector3D operator-(const float other) const;
		friend RealVector3D operator-(const float a, const RealVector3D &b);

		RealVector3D &operator*=(const RealVector3D &other);
		RealVector3D &operator*=(const float other);
		RealVector3D operator*(const RealVector3D &other) const;
		RealVector3D operator*(const float other) const;
		friend RealVector3D operator*(const float a, const RealVector3D &b);

		RealVector3D &operator/=(const RealVector3D &other);
		RealVector3D &operator/=(const float other);
		RealVector3D operator/(const RealVector3D &other) const;
		RealVector3D operator/(const float other) const;
		friend RealVector3D operator/(const float a, const RealVector3D &b);

		float Length2() const;
		float Length() const;
		static RealVector3D Normalize(const RealVector3D& v);
		static RealVector3D Cross(const RealVector3D& a, const RealVector3D& b);
		static RealVector3D Transform(const RealVector3D& value, const RealQuaternion& rotation);
	};
}

