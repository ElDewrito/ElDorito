#pragma once

namespace Blam::Tags
{
	template <typename BaseType, typename EnumType>
	struct Enum
	{
		BaseType Value;

		inline Enum(const BaseType &value) :
			Value(value)
		{
		}

		inline Enum(const EnumType &value) :
			Enum((BaseType)value)
		{
		}

		inline Enum(const Enum<BaseType, EnumType> &other) :
			Enum(other.Value)
		{
		}

		inline bool operator==(const BaseType &other) const { return Value == other; }
		inline bool operator!=(const BaseType &other) const { return !(*this == other); }

		inline bool operator==(const EnumType &other) const { return Value == reinterpret_cast<BaseType>(other); }
		inline bool operator!=(const EnumType &other) const { return !(*this == other); }

		inline bool operator==(const Enum<BaseType, EnumType> &other) const { return Value == other.Value; }
		inline bool operator!=(const Enum<BaseType, EnumType> &other) const { return !(*this == other); }

		inline Enum<BaseType, EnumType> &operator=(const BaseType &value)
		{
			Value = value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator+=(const BaseType &value)
		{
			Value += value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator-=(const BaseType &value)
		{
			Value -= value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator*=(const BaseType &value)
		{
			Value *= value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator/=(const BaseType &value)
		{
			Value /= value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator|=(const BaseType &value)
		{
			Value |= value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator&=(const BaseType &value)
		{
			Value &= value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator=(const EnumType &value)
		{
			Value = (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator+=(const EnumType &value)
		{
			Value += (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator-=(const EnumType &value)
		{
			Value -= (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator*=(const EnumType &value)
		{
			Value *= (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator/=(const EnumType &value)
		{
			Value /= (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator|=(const EnumType &value)
		{
			Value |= (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator&=(const EnumType &value)
		{
			Value &= (BaseType)value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator=(const Enum<BaseType, EnumType> &value)
		{
			Value = value.Value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator+=(const Enum<BaseType, EnumType> &value)
		{
			Value += value.Value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator-=(const Enum<BaseType, EnumType> &value)
		{
			Value -= value.Value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator*=(const Enum<BaseType, EnumType> &value)
		{
			Value *= value.Value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator/=(const Enum<BaseType, EnumType> &value)
		{
			Value /= value.Value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator|=(const Enum<BaseType, EnumType> &value)
		{
			Value |= value.Value;
			return *this;
		}

		inline Enum<BaseType, EnumType> &operator&=(const Enum<BaseType, EnumType> &value)
		{
			Value &= value.Value;
			return *this;
		}
	};
}