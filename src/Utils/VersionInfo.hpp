#pragma once

#include <cstdint>
#include <string>

namespace Utils
{
	namespace Version
	{
		uint32_t GetVersionInt();
		std::string GetVersionString();
	}
}