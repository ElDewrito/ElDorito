#include "RealPoint3D.hpp"

namespace Blam::Math
{
	RealPoint3D::RealPoint3D()
		: RealPoint3D(0.0f, 0.0f, 0.0f)
	{
	}

	RealPoint3D::RealPoint3D(const float x, const float y, const float z)
		: X(x), Y(y), Z(z)
	{
	}

	bool RealPoint3D::operator==(const RealPoint3D &other) const
	{
		return X == other.X
			&& Y == other.Y
			&& Z == other.Z;
	}

	bool RealPoint3D::operator!=(const RealPoint3D &other) const
	{
		return !(*this == other);
	}

	RealPoint3D::operator const float *() const
	{
		return &X;
	}

	RealPoint3D &RealPoint3D::operator+=(const RealPoint3D &other)
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z;

		return *this;
	}

	RealPoint3D &RealPoint3D::operator+=(const float other)
	{
		X += other;
		Y += other;
		Z += other;

		return *this;
	}

	RealPoint3D RealPoint3D::operator+(const RealPoint3D &other) const
	{
		return RealPoint3D(X + other.X, Y + other.Y, Z + other.Z);
	}

	RealPoint3D RealPoint3D::operator+(const float other) const
	{
		return RealPoint3D(X + other, Y + other, Z + other);
	}

	RealPoint3D operator+(const float a, const RealPoint3D &b)
	{
		return RealPoint3D(a + b.X, a + b.Y, a + b.Z);
	}

	RealPoint3D &RealPoint3D::operator-=(const RealPoint3D &other)
	{
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;

		return *this;
	}

	RealPoint3D &RealPoint3D::operator-=(const float other)
	{
		X -= other;
		Y -= other;
		Z -= other;

		return *this;
	}

	RealPoint3D RealPoint3D::operator-(const RealPoint3D &other) const
	{
		return RealPoint3D(X - other.X, Y - other.Y, Z - other.Z);
	}

	RealPoint3D RealPoint3D::operator-(const float other) const
	{
		return RealPoint3D(X - other, Y - other, Z - other);
	}

	RealPoint3D operator-(const float a, const RealPoint3D &b)
	{
		return RealPoint3D(a - b.X, a - b.Y, a - b.Z);
	}

	RealPoint3D &RealPoint3D::operator*=(const RealPoint3D &other)
	{
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;

		return *this;
	}

	RealPoint3D &RealPoint3D::operator*=(const float other)
	{
		X *= other;
		Y *= other;
		Z *= other;

		return *this;
	}

	RealPoint3D RealPoint3D::operator*(const RealPoint3D &other) const
	{
		return RealPoint3D(X * other.X, Y * other.Y, Z * other.Z);
	}

	RealPoint3D RealPoint3D::operator*(const float other) const
	{
		return RealPoint3D(X * other, Y * other, Z * other);
	}

	RealPoint3D operator*(const float a, const RealPoint3D &b)
	{
		return RealPoint3D(a * b.X, a * b.Y, a * b.Z);
	}

	RealPoint3D &RealPoint3D::operator/=(const RealPoint3D &other)
	{
		X /= other.X;
		Y /= other.Y;
		Z /= other.Z;

		return *this;
	}

	RealPoint3D &RealPoint3D::operator/=(const float other)
	{
		X /= other;
		Y /= other;
		Z /= other;

		return *this;
	}

	RealPoint3D RealPoint3D::operator/(const RealPoint3D &other) const
	{
		return RealPoint3D(X / other.X, Y / other.Y, Z / other.Z);
	}

	RealPoint3D RealPoint3D::operator/(const float other) const
	{
		return RealPoint3D(X / other, Y / other, Z / other);
	}

	RealPoint3D operator/(const float a, const RealPoint3D &b)
	{
		return RealPoint3D(a / b.X, a / b.Y, a / b.Z);
	}
}

