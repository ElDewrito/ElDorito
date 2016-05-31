#include "RealQuaternion.hpp"

namespace Blam
{
	namespace Math
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
			return RealQuaternion(I * other.I, J * other.J, K * other.K, W * other.W);
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
	}
}
