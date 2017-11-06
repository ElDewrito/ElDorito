#include "Point2D.hpp"

namespace Blam::Math
{
	Point2D::Point2D()
		: Point2D(0, 0)
	{
	}

	Point2D::Point2D(const int16_t x, const int16_t y)
		: X(x), Y(y)
	{
	}

	bool Point2D::operator==(const Point2D &other) const
	{
		return X == other.X
			&& Y == other.Y;
	}

	bool Point2D::operator!=(const Point2D &other) const
	{
		return !(*this == other);
	}

	Point2D::operator const int16_t *() const
	{
		return &X;
	}

	Point2D &Point2D::operator+=(const Point2D &other)
	{
		X += other.X;
		Y += other.Y;

		return *this;
	}

	Point2D &Point2D::operator+=(const int16_t other)
	{
		X += other;
		Y += other;

		return *this;
	}

	Point2D Point2D::operator+(const Point2D &other) const
	{
		return Point2D(X + other.X, Y + other.Y);
	}

	Point2D Point2D::operator+(const int16_t other) const
	{
		return Point2D(X + other, Y + other);
	}

	Point2D operator+(const int16_t a, const Point2D &b)
	{
		return Point2D(a + b.X, a + b.Y);
	}

	Point2D &Point2D::operator-=(const Point2D &other)
	{
		X -= other.X;
		Y -= other.Y;

		return *this;
	}

	Point2D &Point2D::operator-=(const int16_t other)
	{
		X -= other;
		Y -= other;

		return *this;
	}

	Point2D Point2D::operator-(const Point2D &other) const
	{
		return Point2D(X - other.X, Y - other.Y);
	}

	Point2D Point2D::operator-(const int16_t other) const
	{
		return Point2D(X - other, Y - other);
	}

	Point2D operator-(const int16_t a, const Point2D &b)
	{
		return Point2D(a - b.X, a - b.Y);
	}

	Point2D &Point2D::operator*=(const Point2D &other)
	{
		X *= other.X;
		Y *= other.Y;

		return *this;
	}

	Point2D &Point2D::operator*=(const int16_t other)
	{
		X *= other;
		Y *= other;

		return *this;
	}

	Point2D Point2D::operator*(const Point2D &other) const
	{
		return Point2D(X * other.X, Y * other.Y);
	}

	Point2D Point2D::operator*(const int16_t other) const
	{
		return Point2D(X * other, Y * other);
	}

	Point2D operator*(const int16_t a, const Point2D &b)
	{
		return Point2D(a * b.X, a * b.Y);
	}

	Point2D &Point2D::operator/=(const Point2D &other)
	{
		X /= other.X;
		Y /= other.Y;

		return *this;
	}

	Point2D &Point2D::operator/=(const int16_t other)
	{
		X /= other;
		Y /= other;

		return *this;
	}

	Point2D Point2D::operator/(const Point2D &other) const
	{
		return Point2D(X / other.X, Y / other.Y);
	}

	Point2D Point2D::operator/(const int16_t other) const
	{
		return Point2D(X / other, Y / other);
	}

	Point2D operator/(const int16_t a, const Point2D &b)
	{
		return Point2D(a / b.X, a / b.Y);
	}
}

