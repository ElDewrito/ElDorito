#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

namespace EngineTypes {

#pragma pack(push, 1)

struct Resolution {
	std::uint32_t width;
	std::uint32_t height;
};

#pragma pack(pop)

}
