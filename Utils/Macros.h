#pragma once

namespace Utils
{
	template <typename T, size_t Num>
	size_t GetArraySize(T(&a)[Num])
	{
		return Num
	}
}