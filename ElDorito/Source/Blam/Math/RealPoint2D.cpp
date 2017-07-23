#include "RealPoint2D.hpp"

namespace Blam::Math
{
	RealPoint2D::RealPoint2D()
		: RealPoint2D(0, 0)
	{
	}

	RealPoint2D::RealPoint2D(const float x, const float y)
		: X(x), Y(y)
	{
	}

	bool RealPoint2D::operator==(const RealPoint2D &other) const
	{
		return X == other.X
			&& Y == other.Y;
	}

	bool RealPoint2D::operator!=(const RealPoint2D &other) const
	{
		return !(*this == other);
	}

	RealPoint2D::operator const float *() const
	{
		return &X;
	}

	RealPoint2D &RealPoint2D::operator+=(const RealPoint2D &other)
	{
		X += other.X;
		Y += other.Y;

		return *this;
	}

	RealPoint2D &RealPoint2D::operator+=(const float other)
	{
		X += other;
		Y += other;

		return *this;
	}

	RealPoint2D RealPoint2D::operator+(const RealPoint2D &other) const
	{
		return RealPoint2D(X + other.X, Y + other.Y);
	}

	RealPoint2D RealPoint2D::operator+(const float other) const
	{
		return RealPoint2D(X + other, Y + other);
	}

	RealPoint2D operator+(const float a, const RealPoint2D &b)
	{
		return RealPoint2D(a + b.X, a + b.Y);
	}

	RealPoint2D &RealPoint2D::operator-=(const RealPoint2D &other)
	{
		X -= other.X;
		Y -= other.Y;

		return *this;
	}

	RealPoint2D &RealPoint2D::operator-=(const float other)
	{
		X -= other;
		Y -= other;

		return *this;
	}

	RealPoint2D RealPoint2D::operator-(const RealPoint2D &other) const
	{
		return RealPoint2D(X - other.X, Y - other.Y);
	}

	RealPoint2D RealPoint2D::operator-(const float other) const
	{
		return RealPoint2D(X - other, Y - other);
	}

	RealPoint2D operator-(const float a, const RealPoint2D &b)
	{
		return RealPoint2D(a - b.X, a - b.Y);
	}

	RealPoint2D &RealPoint2D::operator*=(const RealPoint2D &other)
	{
		X *= other.X;
		Y *= other.Y;

		return *this;
	}

	RealPoint2D &RealPoint2D::operator*=(const float other)
	{
		X *= other;
		Y *= other;

		return *this;
	}

	RealPoint2D RealPoint2D::operator*(const RealPoint2D &other) const
	{
		return RealPoint2D(X * other.X, Y * other.Y);
	}

	RealPoint2D RealPoint2D::operator*(const float other) const
	{
		return RealPoint2D(X * other, Y * other);
	}

	RealPoint2D operator*(const float a, const RealPoint2D &b)
	{
		return RealPoint2D(a * b.X, a * b.Y);
	}

	RealPoint2D &RealPoint2D::operator/=(const RealPoint2D &other)
	{
		X /= other.X;
		Y /= other.Y;

		return *this;
	}

	RealPoint2D &RealPoint2D::operator/=(const float other)
	{
		X /= other;
		Y /= other;

		return *this;
	}

	RealPoint2D RealPoint2D::operator/(const RealPoint2D &other) const
	{
		return RealPoint2D(X / other.X, Y / other.Y);
	}

	RealPoint2D RealPoint2D::operator/(const float other) const
	{
		return RealPoint2D(X / other, Y / other);
	}

	RealPoint2D operator/(const float a, const RealPoint2D &b)
	{
		return RealPoint2D(a / b.X, a / b.Y);
	}
}

