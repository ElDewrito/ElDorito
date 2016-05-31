#include "RealVector3D.hpp"

namespace Blam
{
	namespace Math
	{
		RealVector3D::RealVector3D()
			: RealVector3D(0.0f, 0.0f, 0.0f)
		{
		}

		RealVector3D::RealVector3D(const float i, const float j, const float k)
			: I(i), J(j), K(k)
		{
		}

		bool RealVector3D::operator==(const RealVector3D &other) const
		{
			return I == other.I
				&& J == other.J
				&& K == other.K;
		}

		bool RealVector3D::operator!=(const RealVector3D &other) const
		{
			return !(*this == other);
		}

		RealVector3D::operator const float *() const
		{
			return &I;
		}

		RealVector3D &RealVector3D::operator+=(const RealVector3D &other)
		{
			I += other.I;
			J += other.J;
			K += other.K;

			return *this;
		}

		RealVector3D &RealVector3D::operator+=(const float other)
		{
			I += other;
			J += other;
			K += other;

			return *this;
		}

		RealVector3D RealVector3D::operator+(const RealVector3D &other) const
		{
			return RealVector3D(I + other.I, J + other.J, K + other.K);
		}

		RealVector3D RealVector3D::operator+(const float other) const
		{
			return RealVector3D(I + other, J + other, K + other);
		}

		RealVector3D operator+(const float a, const RealVector3D &b)
		{
			return RealVector3D(a + b.I, a + b.J, a + b.K);
		}

		RealVector3D &RealVector3D::operator-=(const RealVector3D &other)
		{
			I -= other.I;
			J -= other.J;
			K -= other.K;

			return *this;
		}

		RealVector3D &RealVector3D::operator-=(const float other)
		{
			I -= other;
			J -= other;
			K -= other;

			return *this;
		}

		RealVector3D RealVector3D::operator-(const RealVector3D &other) const
		{
			return RealVector3D(I - other.I, J - other.J, K - other.K);
		}

		RealVector3D RealVector3D::operator-(const float other) const
		{
			return RealVector3D(I - other, J - other, K - other);
		}

		RealVector3D operator-(const float a, const RealVector3D &b)
		{
			return RealVector3D(a - b.I, a - b.J, a - b.K);
		}

		RealVector3D &RealVector3D::operator*=(const RealVector3D &other)
		{
			I *= other.I;
			J *= other.J;
			K *= other.K;

			return *this;
		}

		RealVector3D &RealVector3D::operator*=(const float other)
		{
			I *= other;
			J *= other;
			K *= other;

			return *this;
		}

		RealVector3D RealVector3D::operator*(const RealVector3D &other) const
		{
			return RealVector3D(I * other.I, J * other.J, K * other.K);
		}

		RealVector3D RealVector3D::operator*(const float other) const
		{
			return RealVector3D(I * other, J * other, K * other);
		}

		RealVector3D operator*(const float a, const RealVector3D &b)
		{
			return RealVector3D(a * b.I, a * b.J, a * b.K);
		}

		RealVector3D &RealVector3D::operator/=(const RealVector3D &other)
		{
			I /= other.I;
			J /= other.J;
			K /= other.K;

			return *this;
		}

		RealVector3D &RealVector3D::operator/=(const float other)
		{
			I /= other;
			J /= other;
			K /= other;

			return *this;
		}

		RealVector3D RealVector3D::operator/(const RealVector3D &other) const
		{
			return RealVector3D(I / other.I, J / other.J, K / other.K);
		}

		RealVector3D RealVector3D::operator/(const float other) const
		{
			return RealVector3D(I / other, J / other, K / other);
		}

		RealVector3D operator/(const float a, const RealVector3D &b)
		{
			return RealVector3D(a / b.I, a / b.J, a / b.K);
		}
	}
}
