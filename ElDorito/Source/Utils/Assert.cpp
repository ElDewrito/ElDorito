#include "Assert.hpp"
#ifdef _DEBUG

#include <windows.h>
#include <string>

namespace Utils
{
	Utils::FailBehavior ReportFailure(const char* condition,
		const char *msg,
		const char* file,
		const char *fnc,
		long line)
	{
		// Parse file name
		std::string file_name(file);

		char messageBuffer[2048];
		sprintf_s(messageBuffer, "%s\nFile: %s\nFunction: %s\nLine: %d\nCondition: '%s'", msg, file_name.c_str(), fnc, line, condition);

		if( MessageBox(NULL, "Assertion", messageBuffer, MB_ICONERROR | MB_YESNO | MB_DEFBUTTON1) == IDYES )
		{
			return Utils::FailBehavior::Halt;
		}

		return Utils::FailBehavior::Continue;
	}
}
#endif