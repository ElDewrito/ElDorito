#pragma once

namespace Blam::Math
{
	struct RealMatrix4x3;
	struct RealVector3D;
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

		static RealQuaternion CreateFromRotationMatrix(const RealMatrix4x3& matrix);
		static RealQuaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
		static RealQuaternion CreateFromAxisAngle(const RealVector3D& axis, float angle);

		static RealQuaternion Slerp(const RealQuaternion& a, const RealQuaternion& b, float t);
		static RealQuaternion Normalize(const RealQuaternion& q);
			
	};
}

