#pragma once

namespace Blam::Math
{
	struct Angle
	{
		float Value;

		Angle();
		Angle(const float value);

		bool operator==(const Angle &other) const;
		bool operator!=(const Angle &other) const;

		explicit operator float() const;

		Angle &operator+=(const Angle &other);
		Angle &operator+=(const float other);
		Angle operator+(const Angle &other) const;
		Angle operator+(const float other) const;
		friend Angle operator+(const float a, const Angle &b);

		Angle &operator-=(const Angle &other);
		Angle &operator-=(const float other);
		Angle operator-(const Angle &other) const;
		Angle operator-(const float other) const;
		friend Angle operator-(const float a, const Angle &b);

		Angle &operator*=(const Angle &other);
		Angle &operator*=(const float other);
		Angle operator*(const Angle &other) const;
		Angle operator*(const float other) const;
		friend Angle operator*(const float a, const Angle &b);

		Angle &operator/=(const Angle &other);
		Angle &operator/=(const float other);
		Angle operator/(const Angle &other) const;
		Angle operator/(const float other) const;
		friend Angle operator/(const float a, const Angle &b);
	};
}

