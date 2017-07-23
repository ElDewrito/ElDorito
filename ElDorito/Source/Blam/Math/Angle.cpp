#include "Angle.hpp"

namespace Blam::Math
{
	Angle::Angle()
		: Angle(0.0f)
	{
	}

	Angle::Angle(const float value)
		: Value(value)
	{
	}

	bool Angle::operator==(const Angle &other) const
	{
		return Value == other.Value;
	}

	bool Angle::operator!=(const Angle &other) const
	{
		return !(*this == other);
	}

	Angle::operator float() const
	{
		return Value;
	}

	Angle &Angle::operator+=(const Angle &other)
	{
		Value += other.Value;

		return *this;
	}

	Angle &Angle::operator+=(const float other)
	{
		Value += other;

		return *this;
	}

	Angle Angle::operator+(const Angle &other) const
	{
		return Angle(Value + other.Value);
	}

	Angle Angle::operator+(const float other) const
	{
		return Angle(Value + other);
	}

	Angle operator+(const float a, const Angle &b)
	{
		return Angle(a + b.Value);
	}

	Angle &Angle::operator-=(const Angle &other)
	{
		Value -= other.Value;

		return *this;
	}

	Angle &Angle::operator-=(const float other)
	{
		Value -= other;

		return *this;
	}

	Angle Angle::operator-(const Angle &other) const
	{
		return Angle(Value - other.Value);
	}

	Angle Angle::operator-(const float other) const
	{
		return Angle(Value - other);
	}

	Angle operator-(const float a, const Angle &b)
	{
		return Angle(a - b.Value);
	}

	Angle &Angle::operator*=(const Angle &other)
	{
		Value *= other.Value;

		return *this;
	}

	Angle &Angle::operator*=(const float other)
	{
		Value *= other;

		return *this;
	}

	Angle Angle::operator*(const Angle &other) const
	{
		return Angle(Value * other.Value);
	}

	Angle Angle::operator*(const float other) const
	{
		return Angle(Value * other);
	}

	Angle operator*(const float a, const Angle &b)
	{
		return Angle(a * b.Value);
	}

	Angle &Angle::operator/=(const Angle &other)
	{
		Value /= other.Value;

		return *this;
	}

	Angle &Angle::operator/=(const float other)
	{
		Value /= other;

		return *this;
	}

	Angle Angle::operator/(const Angle &other) const
	{
		return Angle(Value / other.Value);
	}

	Angle Angle::operator/(const float other) const
	{
		return Angle(Value / other);
	}

	Angle operator/(const float a, const Angle &b)
	{
		return Angle(a / b.Value);
	}
}

