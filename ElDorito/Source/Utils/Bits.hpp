#pragma once

namespace Utils
{
	namespace Bits
	{
		template<class T>
		int CountBits(T val)
		{
			int result = 0;
			while (val != 0)
			{
				val >>= 1;
				result++;
			}
			return result;
		}
	}
}