#pragma once
#include <sstream>

namespace Utils
{
	std::string GetStackTraceString(int framesToSkip, int framesToCapture);
	void LogEthernetAdapterInfo();
}
