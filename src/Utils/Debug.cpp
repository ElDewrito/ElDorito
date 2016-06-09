#include "Debug.hpp"
#include <vector>
#include <cstdint>
#include <windows.h>

namespace Utils
{
	std::string GetStackTraceString(int framesToSkip, int framesToCapture)
	{
		std::stringstream ss;
		std::vector<uint32_t> backTrace(framesToCapture);
		int capturedFrames = CaptureStackBackTrace(framesToSkip + 1, framesToCapture, reinterpret_cast<void**>(backTrace.data()), nullptr);
		for (int i = capturedFrames - 1; i >= 0; i--)
		{
			if (i == capturedFrames - 1)
			{
				ss << "0x" << std::uppercase << std::hex << backTrace[i];
			}
			else
			{
				ss << "->" << "0x" << std::uppercase << std::hex << backTrace[i];
			}
		}
		return ss.str();
	}
}
