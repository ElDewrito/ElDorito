#pragma once

namespace Blam::Math
{
	template <typename T>
	struct Bounds
	{
		T Lower;
		T Upper;

		Bounds(const T &lower, const T &upper)
			: Lower(lower), Upper(upper)
		{
		}

		Bounds()
			: Bounds(T(), T())
		{
		}

		bool operator==(const Bounds<T> &other) const
		{
			return Lower == other.Lower
				&& Upper == other.Upper;
		}

		bool operator!=(const Bounds<T> &other) const
		{
			return !(*this == other);
		}
	};
}

