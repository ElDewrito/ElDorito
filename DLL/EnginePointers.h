#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>
#include <vector>
#include "Shared.h"
#include "EngineTypes.h"

/*
 * Variable pointers
 */
extern EngineTypes::Resolution* pResolution;
extern std::uint32_t* master_volume;