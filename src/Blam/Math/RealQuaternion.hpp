#pragma once

namespace Blam
{
	namespace Math
	{
		struct RealQuaternion
		{
			float I;
			float J;
			float K;
			float W;

			RealQuaternion();
			RealQuaternion(const float i, const float j, const float k, const float w);

			bool operator==(const RealQuaternion &other) const;
			bool operator!=(const RealQuaternion &other) const;

			explicit operator const float *() const;

			RealQuaternion &operator+=(const RealQuaternion &other);
			RealQuaternion &operator+=(const float other);
			RealQuaternion operator+(const RealQuaternion &other) const;
			RealQuaternion operator+(const float other) const;
			friend RealQuaternion operator+(const float a, const RealQuaternion &b);

			RealQuaternion &operator-=(const RealQuaternion &other);
			RealQuaternion &operator-=(const float other);
			RealQuaternion operator-(const RealQuaternion &other) const;
			RealQuaternion operator-(const float other) const;
			friend RealQuaternion operator-(const float a, const RealQuaternion &b);

			RealQuaternion &operator*=(const RealQuaternion &other);
			RealQuaternion &operator*=(const float other);
			RealQuaternion operator*(const RealQuaternion &other) const;
			RealQuaternion operator*(const float other) const;
			friend RealQuaternion operator*(const float a, const RealQuaternion &b);

			RealQuaternion &operator/=(const RealQuaternion &other);
			RealQuaternion &operator/=(const float other);
			RealQuaternion operator/(const RealQuaternion &other) const;
			RealQuaternion operator/(const float other) const;
			friend RealQuaternion operator/(const float a, const RealQuaternion &b);
		};
	}
}
