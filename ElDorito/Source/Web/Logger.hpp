#pragma once

// This is just a proxy header which forwards WriteLog to ElDewrito's existing log implementation
// Makes porting the code from Anvil much easier

#include "../Utils/Logger.hpp"

#ifdef _DEBUG
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define WriteLog(p_Format, ...) Utils::Logger::Instance().Log(Utils::LogTypes::Graphics, Utils::LogLevel::Info, __FUNCTION__ ":" STRINGIFY(__LINE__) " - " p_Format, __VA_ARGS__);
#else
#pragma warning(disable : 4390)
#define WriteLog(p_Format, ...)
#endif