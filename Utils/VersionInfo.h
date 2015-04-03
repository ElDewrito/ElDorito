#pragma once

// STL
#include <string>
#include <vector>

// Windows
#include <Windows.h>

namespace Utils
{
  namespace Version
  {
    void SetModule(HMODULE module);

    std::string GetInfo(const std::string &csEntry);
  }
}