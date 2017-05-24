#include "RealVector3D.hpp"
#include "RealQuaternion.hpp"
#include <cmath>
#include <cassert>

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

		float RealVector3D::Length2() const
		{
			return I * I + J * J + K * K;
		}

		float RealVector3D::Length() const
		{
			return std::sqrt(Length2());
		}

		RealVector3D RealVector3D::Normalize(const RealVector3D& v)
		{
			auto len2 = v.Length2();
			assert(len2 != 0);

			return v / len2;
		}

		RealVector3D RealVector3D::Cross(const RealVector3D& a, const RealVector3D& b)
		{
			return RealVector3D(a.J * b.K - a.K * b.J, a.K * b.I - a.I * b.K, a.I * b.J - a.J * b.I);
		}

		RealVector3D RealVector3D::Transform(const RealVector3D& value, const RealQuaternion& rotation)
		{
			RealVector3D result;

			float x2 = rotation.I + rotation.I;
			float y2 = rotation.J + rotation.J;
			float z2 = rotation.K + rotation.K;

			float wx2 = rotation.W * x2;
			float wy2 = rotation.W * y2;
			float wz2 = rotation.W * z2;
			float xx2 = rotation.I * x2;
			float xy2 = rotation.I * y2;
			float xz2 = rotation.I * z2;
			float yy2 = rotation.J * y2;
			float yz2 = rotation.J * z2;
			float zz2 = rotation.K * z2;

			result.I = value.I * (1.0f - yy2 - zz2) + value.J * (xy2 - wz2) + value.K * (xz2 + wy2);
			result.J = value.I * (xy2 + wz2) + value.J * (1.0f - xx2 - zz2) + value.K * (yz2 - wx2);
			result.K = value.I * (xz2 - wy2) + value.J * (yz2 + wx2) + value.K * (1.0f - xx2 - yy2);

			return result;
		}
	}
}
