#pragma once

// This is just a proxy header which forwards WriteLog to ElDewrito's existing log implementation
// Makes porting the code from Anvil much easier

#include "../Utils/DebugLog.hpp"

#ifdef _DEBUG
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define WriteLog(p_Format, ...) Utils::DebugLog::Instance().Log(__FUNCTION__ ":" STRINGIFY(__LINE__), p_Format, __VA_ARGS__);
#else
#pragma warning(disable : 4390)
#define WriteLog(p_Format, ...)
#endif