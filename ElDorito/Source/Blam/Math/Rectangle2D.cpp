#include "Rectangle2D.hpp"

namespace Blam::Math
{
	Rectangle2D::Rectangle2D()
		: Rectangle2D(0, 0, 0, 0)
	{
	}

	Rectangle2D::Rectangle2D(const int16_t top, const int16_t left, const int16_t bottom, const int16_t right)
		: Top(top), Left(left), Bottom(bottom), Right(right)
	{
	}

	bool Rectangle2D::operator==(const Rectangle2D &other) const
	{
		return Top == other.Top
			&& Left == other.Left
			&& Bottom == other.Bottom
			&& Right == other.Right;
	}

	bool Rectangle2D::operator!=(const Rectangle2D &other) const
	{
		return !(*this == other);
	}

	Rectangle2D::operator const int16_t *() const
	{
		return &Top;
	}

	Rectangle2D &Rectangle2D::operator+=(const Rectangle2D &other)
	{
		Top += other.Top;
		Left += other.Left;
		Bottom += other.Bottom;
		Right += other.Right;

		return *this;
	}

	Rectangle2D &Rectangle2D::operator+=(const int16_t other)
	{
		Top += other;
		Left += other;
		Bottom += other;
		Right += other;

		return *this;
	}

	Rectangle2D Rectangle2D::operator+(const Rectangle2D &other) const
	{
		return Rectangle2D(Top + other.Top, Left + other.Left, Bottom + other.Bottom, Right + other.Right);
	}

	Rectangle2D Rectangle2D::operator+(const int16_t other) const
	{
		return Rectangle2D(Top + other, Left + other, Bottom + other, Right + other);
	}

	Rectangle2D operator+(const int16_t a, const Rectangle2D &b)
	{
		return Rectangle2D(a + b.Top, a + b.Left, a + b.Bottom, a + b.Right);
	}

	Rectangle2D &Rectangle2D::operator-=(const Rectangle2D &other)
	{
		Top -= other.Top;
		Left -= other.Left;
		Bottom -= other.Bottom;
		Right -= other.Right;

		return *this;
	}

	Rectangle2D &Rectangle2D::operator-=(const int16_t other)
	{
		Top -= other;
		Left -= other;
		Bottom -= other;
		Right -= other;

		return *this;
	}

	Rectangle2D Rectangle2D::operator-(const Rectangle2D &other) const
	{
		return Rectangle2D(Top - other.Top, Left - other.Left, Bottom - other.Bottom, Right - other.Right);
	}

	Rectangle2D Rectangle2D::operator-(const int16_t other) const
	{
		return Rectangle2D(Top - other, Left - other, Bottom - other, Right - other);
	}

	Rectangle2D operator-(const int16_t a, const Rectangle2D &b)
	{
		return Rectangle2D(a - b.Top, a - b.Left, a - b.Bottom, a - b.Right);
	}

	Rectangle2D &Rectangle2D::operator*=(const Rectangle2D &other)
	{
		Top *= other.Top;
		Left *= other.Left;
		Bottom *= other.Bottom;
		Right *= other.Right;

		return *this;
	}

	Rectangle2D &Rectangle2D::operator*=(const int16_t other)
	{
		Top *= other;
		Left *= other;
		Bottom *= other;
		Right *= other;

		return *this;
	}

	Rectangle2D Rectangle2D::operator*(const Rectangle2D &other) const
	{
		return Rectangle2D(Top * other.Top, Left * other.Left, Bottom * other.Bottom, Right * other.Right);
	}

	Rectangle2D Rectangle2D::operator*(const int16_t other) const
	{
		return Rectangle2D(Top * other, Left * other, Bottom * other, Right * other);
	}

	Rectangle2D operator*(const int16_t a, const Rectangle2D &b)
	{
		return Rectangle2D(a * b.Top, a * b.Left, a * b.Bottom, a * b.Right);
	}

	Rectangle2D &Rectangle2D::operator/=(const Rectangle2D &other)
	{
		Top /= other.Top;
		Left /= other.Left;
		Bottom /= other.Bottom;
		Right /= other.Right;

		return *this;
	}

	Rectangle2D &Rectangle2D::operator/=(const int16_t other)
	{
		Top /= other;
		Left /= other;
		Bottom /= other;
		Right /= other;

		return *this;
	}

	Rectangle2D Rectangle2D::operator/(const Rectangle2D &other) const
	{
		return Rectangle2D(Top / other.Top, Left / other.Left, Bottom / other.Bottom, Right / other.Right);
	}

	Rectangle2D Rectangle2D::operator/(const int16_t other) const
	{
		return Rectangle2D(Top / other, Left / other, Bottom / other, Right / other);
	}

	Rectangle2D operator/(const int16_t a, const Rectangle2D &b)
	{
		return Rectangle2D(a / b.Top, a / b.Left, a / b.Bottom, a / b.Right);
	}
}

