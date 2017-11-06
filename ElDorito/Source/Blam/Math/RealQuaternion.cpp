#include "RealQuaternion.hpp"
#include "RealMatrix4x3.hpp"
#include <cmath>

namespace Blam::Math
{
	RealQuaternion::RealQuaternion()
		: RealQuaternion(0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	RealQuaternion::RealQuaternion(const float i, const float j, const float k, const float w)
		: I(i), J(j), K(k), W(w)
	{
	}

	bool RealQuaternion::operator==(const RealQuaternion &other) const
	{
		return I == other.I
			&& J == other.J
			&& K == other.K
			&& W == other.W;
	}

	bool RealQuaternion::operator!=(const RealQuaternion &other) const
	{
		return !(*this == other);
	}

	RealQuaternion::operator const float *() const
	{
		return &I;
	}

	RealQuaternion &RealQuaternion::operator+=(const RealQuaternion &other)
	{
		I += other.I;
		J += other.J;
		K += other.K;
		W += other.W;

		return *this;
	}

	RealQuaternion &RealQuaternion::operator+=(const float other)
	{
		I += other;
		J += other;
		K += other;
		W += other;

		return *this;
	}

	RealQuaternion RealQuaternion::operator+(const RealQuaternion &other) const
	{
		return RealQuaternion(I + other.I, J + other.J, K + other.K, W + other.W);
	}

	RealQuaternion RealQuaternion::operator+(const float other) const
	{
		return RealQuaternion(I + other, J + other, K + other, W + other);
	}

	RealQuaternion operator+(const float a, const RealQuaternion &b)
	{
		return RealQuaternion(a + b.I, a + b.J, a + b.K, a + b.W);
	}

	RealQuaternion &RealQuaternion::operator-=(const RealQuaternion &other)
	{
		I -= other.I;
		J -= other.J;
		K -= other.K;
		W -= other.W;

		return *this;
	}

	RealQuaternion &RealQuaternion::operator-=(const float other)
	{
		I -= other;
		J -= other;
		K -= other;
		W -= other;

		return *this;
	}

	RealQuaternion RealQuaternion::operator-(const RealQuaternion &other) const
	{
		return RealQuaternion(I - other.I, J - other.J, K - other.K, W - other.W);
	}

	RealQuaternion RealQuaternion::operator-(const float other) const
	{
		return RealQuaternion(I - other, J - other, K - other, W - other);
	}

	RealQuaternion operator-(const float a, const RealQuaternion &b)
	{
		return RealQuaternion(a - b.I, a - b.J, a - b.K, a - b.W);
	}

	RealQuaternion &RealQuaternion::operator*=(const RealQuaternion &other)
	{
		I *= other.I;
		J *= other.J;
		K *= other.K;
		W *= other.W;

		return *this;
	}

	RealQuaternion &RealQuaternion::operator*=(const float other)
	{
		I *= other;
		J *= other;
		K *= other;
		W *= other;

		return *this;
	}

	RealQuaternion RealQuaternion::operator*(const RealQuaternion &other) const
	{
		float q1x = I;
		float q1y = J;
		float q1z = K;
		float q1w = W;

		float q2x = other.I;
		float q2y = other.J;
		float q2z = other.K;
		float q2w = other.W;

		// cross(av, bv)
		float cx = q1y * q2z - q1z * q2y;
		float cy = q1z * q2x - q1x * q2z;
		float cz = q1x * q2y - q1y * q2x;

		float dot = q1x * q2x + q1y * q2y + q1z * q2z;

		return RealQuaternion
		{
			q1x * q2w + q2x * q1w + cx,
			q1y * q2w + q2y * q1w + cy,
			q1z * q2w + q2z * q1w + cz,
			q1w * q2w - dot
		};
	}

	RealQuaternion RealQuaternion::operator*(const float other) const
	{
		return RealQuaternion(I * other, J * other, K * other, W * other);
	}

	RealQuaternion operator*(const float a, const RealQuaternion &b)
	{
		return RealQuaternion(a * b.I, a * b.J, a * b.K, a * b.W);
	}

	RealQuaternion &RealQuaternion::operator/=(const RealQuaternion &other)
	{
		I /= other.I;
		J /= other.J;
		K /= other.K;
		W /= other.W;

		return *this;
	}

	RealQuaternion &RealQuaternion::operator/=(const float other)
	{
		I /= other;
		J /= other;
		K /= other;
		W /= other;

		return *this;
	}

	RealQuaternion RealQuaternion::operator/(const RealQuaternion &other) const
	{
		return RealQuaternion(I / other.I, J / other.J, K / other.K, W / other.W);
	}

	RealQuaternion RealQuaternion::operator/(const float other) const
	{
		return RealQuaternion(I / other, J / other, K / other, W / other);
	}

	RealQuaternion operator/(const float a, const RealQuaternion &b)
	{
		return RealQuaternion(a / b.I, a / b.J, a / b.K, a / b.W);
	}

	RealQuaternion RealQuaternion::CreateFromRotationMatrix(const RealMatrix4x3& matrix)
	{
		float trace = matrix.Forward.I + matrix.Left.J + matrix.Up.K;

		RealQuaternion q;

		if (trace > 0.0f)
		{
			float s = (float)std::sqrt(trace + 1.0f);
			q.W = s * 0.5f;
			s = 0.5f / s;
			q.I = (matrix.Left.K - matrix.Up.J) * s;
			q.J = (matrix.Up.I - matrix.Forward.K) * s;
			q.K = (matrix.Forward.J - matrix.Left.I) * s;
		}
		else
		{
			if (matrix.Forward.I >= matrix.Left.J && matrix.Forward.I >= matrix.Up.K)
			{
				float s = (float)std::sqrt(1.0f + matrix.Forward.I - matrix.Left.J - matrix.Up.K);
				float invS = 0.5f / s;
				q.I = 0.5f * s;
				q.J = (matrix.Forward.J + matrix.Left.I) * invS;
				q.K = (matrix.Forward.K + matrix.Up.I) * invS;
				q.W = (matrix.Left.K - matrix.Up.J) * invS;
			}
			else if (matrix.Left.J > matrix.Up.K)
			{
				float s = (float)std::sqrt(1.0f + matrix.Left.J - matrix.Forward.I - matrix.Up.K);
				float invS = 0.5f / s;
				q.I = (matrix.Left.I + matrix.Forward.J) * invS;
				q.J = 0.5f * s;
				q.K = (matrix.Up.J + matrix.Left.K) * invS;
				q.W = (matrix.Up.I - matrix.Forward.K) * invS;
			}
			else
			{
				float s = (float)std::sqrt(1.0f + matrix.Up.K - matrix.Forward.I - matrix.Left.J);
				float invS = 0.5f / s;
				q.I = (matrix.Up.I + matrix.Forward.K) * invS;
				q.J = (matrix.Up.J + matrix.Left.K) * invS;
				q.K = 0.5f * s;
				q.W = (matrix.Forward.J - matrix.Left.I) * invS;
			}
		}

		return q;
	}

	RealQuaternion RealQuaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
	{
		//  Roll first, about axis the object is facing, then
		//  pitch upward, then yaw to face into the heading
		float sr, cr, sp, cp, sy, cy;

		float halfRoll = roll * 0.5f;
		sr = (float)std::sin(halfRoll); cr = (float)std::cos(halfRoll);

		float halfPitch = pitch * 0.5f;
		sp = (float)std::sin(halfPitch); cp = (float)std::cos(halfPitch);

		float halfYaw = yaw * 0.5f;
		sy = (float)std::sin(halfYaw); cy = (float)std::cos(halfYaw);

		RealQuaternion result;

		return RealQuaternion
		{
			cy * sp * cr + sy * cp * sr,
			sy * cp * cr - cy * sp * sr,
			cy * cp * sr - sy * sp * cr,
			cy * cp * cr + sy * sp * sr
		};
	}

	RealQuaternion RealQuaternion::CreateFromAxisAngle(const RealVector3D& axis, float angle)
	{
		float halfAngle = angle * 0.5f;
		float s = (float)std::sin(halfAngle);
		float c = (float)std::cos(halfAngle);

		return RealQuaternion
		{
			axis.I * s,
			axis.J * s,
			axis.K * s,
			c
		};
	}

	RealQuaternion RealQuaternion::Slerp(const RealQuaternion& a, const RealQuaternion& b, float t)
	{
		const float epsilon = 1e-6f;

		float cosOmega = a.I * b.I + a.J * b.J +
			a.K * b.K + a.W * b.W;

		bool flip = false;

		if (cosOmega < 0.0f)
		{
			flip = true;
			cosOmega = -cosOmega;
		}

		float s1, s2;

		if (cosOmega > (1.0f - epsilon))
		{
			// Too close, do straight linear interpolation.
			s1 = 1.0f - t;
			s2 = (flip) ? -t : t;
		}
		else
		{
			float omega = (float)std::acos(cosOmega);
			float invSinOmega = (float)(1 / std::sin(omega));

			s1 = (float)std::sin((1.0f - t) * omega) * invSinOmega;
			s2 = (flip)
				? (float)-std::sin(t * omega) * invSinOmega
				: (float)std::sin(t * omega) * invSinOmega;
		}

		return RealQuaternion
		{
			s1 * a.I + s2 * b.I,
			s1 * a.J + s2 * b.J,
			s1 * a.K + s2 * b.K,
			s1 * a.W + s2 * b.W
		};
	}

	RealQuaternion RealQuaternion::Normalize(const RealQuaternion& q)
	{
		float ls = q.I * q.I + q.J * q.J + q.K * q.K + q.W * q.W;

		float invNorm = 1.0f / (float)std::sqrt((double)ls);

		return RealQuaternion
		{
			q.I * invNorm,
			q.J * invNorm,
			q.K * invNorm,
			q.W * invNorm
		};
	}
}

