#include "DebugLog.hpp"

#include <cstdarg>
#include <fstream>

#include "../Modules/ModuleGame.hpp"

namespace Utils
{
	void DebugLog::Log(std::string module, std::string format, ...)
	{
		va_list ap;
		va_start(ap, format);

		char buff[4096];
		vsprintf_s(buff, 4096, format.c_str(), ap);
		va_end(ap);
		
		auto& gameModule = Modules::ModuleGame::Instance();
		for (auto filter : gameModule.FiltersExclude)
		{
			if (strstr(buff, filter.c_str()) != NULL)
				return; // string contains an excluded string
		}

		for (auto filter : gameModule.FiltersInclude)
		{
			if (strstr(buff, filter.c_str()) == NULL)
				return; // string doesn't contain an included string
		}

		std::ofstream outfile;
		outfile.open(gameModule.VarLogName->ValueString, std::ios_base::app);
		outfile << '[' << module << "] " << buff << '\n';
		outfile.close();
	}
}