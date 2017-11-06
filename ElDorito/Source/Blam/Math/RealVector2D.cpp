#include "RealVector2D.hpp"

namespace Blam::Math
{
	RealVector2D::RealVector2D()
		: RealVector2D(0, 0)
	{
	}

	RealVector2D::RealVector2D(const float i, const float j)
		: I(i), J(j)
	{
	}

	bool RealVector2D::operator==(const RealVector2D &other) const
	{
		return I == other.I
			&& J == other.J;
	}

	bool RealVector2D::operator!=(const RealVector2D &other) const
	{
		return !(*this == other);
	}

	RealVector2D::operator const float *() const
	{
		return &I;
	}

	RealVector2D &RealVector2D::operator+=(const RealVector2D &other)
	{
		I += other.I;
		J += other.J;

		return *this;
	}

	RealVector2D &RealVector2D::operator+=(const float other)
	{
		I += other;
		J += other;

		return *this;
	}

	RealVector2D RealVector2D::operator+(const RealVector2D &other) const
	{
		return RealVector2D(I + other.I, J + other.J);
	}

	RealVector2D RealVector2D::operator+(const float other) const
	{
		return RealVector2D(I + other, J + other);
	}

	RealVector2D operator+(const float a, const RealVector2D &b)
	{
		return RealVector2D(a + b.I, a + b.J);
	}

	RealVector2D &RealVector2D::operator-=(const RealVector2D &other)
	{
		I -= other.I;
		J -= other.J;

		return *this;
	}

	RealVector2D &RealVector2D::operator-=(const float other)
	{
		I -= other;
		J -= other;

		return *this;
	}

	RealVector2D RealVector2D::operator-(const RealVector2D &other) const
	{
		return RealVector2D(I - other.I, J - other.J);
	}

	RealVector2D RealVector2D::operator-(const float other) const
	{
		return RealVector2D(I - other, J - other);
	}

	RealVector2D operator-(const float a, const RealVector2D &b)
	{
		return RealVector2D(a - b.I, a - b.J);
	}

	RealVector2D &RealVector2D::operator*=(const RealVector2D &other)
	{
		I *= other.I;
		J *= other.J;

		return *this;
	}

	RealVector2D &RealVector2D::operator*=(const float other)
	{
		I *= other;
		J *= other;

		return *this;
	}

	RealVector2D RealVector2D::operator*(const RealVector2D &other) const
	{
		return RealVector2D(I * other.I, J * other.J);
	}

	RealVector2D RealVector2D::operator*(const float other) const
	{
		return RealVector2D(I * other, J * other);
	}

	RealVector2D operator*(const float a, const RealVector2D &b)
	{
		return RealVector2D(a * b.I, a * b.J);
	}

	RealVector2D &RealVector2D::operator/=(const RealVector2D &other)
	{
		I /= other.I;
		J /= other.J;

		return *this;
	}

	RealVector2D &RealVector2D::operator/=(const float other)
	{
		I /= other;
		J /= other;

		return *this;
	}

	RealVector2D RealVector2D::operator/(const RealVector2D &other) const
	{
		return RealVector2D(I / other.I, J / other.J);
	}

	RealVector2D RealVector2D::operator/(const float other) const
	{
		return RealVector2D(I / other, J / other);
	}

	RealVector2D operator/(const float a, const RealVector2D &b)
	{
		return RealVector2D(a / b.I, a / b.J);
	}
}

