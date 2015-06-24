#pragma once
#include <string>
#include "Singleton.hpp"

namespace Utils
{
	class DebugLog : public Singleton<DebugLog>
	{
	public:
		void Log(std::string module, std::string format, ...);
	};
}