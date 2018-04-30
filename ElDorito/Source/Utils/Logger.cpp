#include "Logger.hpp"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include "../Modules/ModuleGame.hpp"

namespace Utils
{
	std::string LogLevelToString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::None:
			return "None";
		case LogLevel::Trace:
			return "Trace";
		case LogLevel::Info:
			return "Info";
		case LogLevel::Warning:
			return "Warning";
		case LogLevel::Error:
			return "Error";
		default:
			return "Unknown";
		}
	}

	std::string LogTypesToString(LogTypes types)
	{
		switch (types)
		{
			case None:
				return "None";
			case All:
				return "All";
			default:

				std::vector<std::string> bits;
				for (auto bit = 0; bit < 32; bit++)
				{
					if (!(types >> bit & 1))
						continue;

					switch (1 << bit)
					{
						case Game: bits.push_back("Game"); break;
						case Network: bits.push_back("Network"); break;
						case Graphics: bits.push_back("Graphics"); break;
						case Memory: bits.push_back("Memory"); break;
						case Sound: bits.push_back("Sound"); break;
						case Input: bits.push_back("Input"); break;
						case Debug: bits.push_back("Debug"); break;
						default: bits.push_back(std::string("UnknownBit") + std::to_string(bit)); break;
					}
				}

				std::stringstream ss;
				ss << bits[0];

				if (bits.size() > 1)
				{
					for (size_t i = 1; i < bits.size(); i++)
					{
						ss << " | " << bits[i];
					}
				}

				return ss.str();
		}
	}

	DWORD WINAPI Logger::Flusher(LPVOID lpParam)
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			Instance().Flush();
		}
	}

	Logger::Logger()
	{
		// these will apply until the ElDorito instance is initialized (the patch stage mostly) and the config values are read
		// TODO: decouple the preferences from the module system since patches need to be applied before the modules are loaded
		Level = LogLevel::Warning;
		Types = All;

		// spin up separate thread to periodically flush the log
		// don't want to block main game thread waiting on the file system
		CreateThread(nullptr, 0, Flusher, nullptr, 0, nullptr);
	}

	void Logger::Log(LogTypes type, LogLevel level, std::string message, ...)
	{
		if (level != LogLevel::Error)
		{
			if (Level == LogLevel::None || level < Level)
			{
				return;
			}

			if ((Types & type) == 0)
			{
				return;
			}
		}

		// construct formatted message
		va_list ap;
		va_start(ap, message);
		int bufferSize = _vscprintf(message.c_str(), ap) + 1;
		char* buffer = new char[bufferSize];
		vsprintf_s(buffer, bufferSize, message.c_str(), ap);
		va_end(ap);

		// add it to the queue to be flushed to disk
		LogEntry entry = { };
		entry.Time = std::chrono::system_clock::now();
		entry.Type = type;
		entry.Message = buffer;
		Entries.push(entry);
	}

	void Logger::Flush()
	{
		if (Entries.empty())
			return;

		// lazy thread safety to preserve message order when flushing to disk
		std::lock_guard<std::mutex> lock(flushMutex);

		auto& gameModule = Modules::ModuleGame::Instance();

		std::ofstream outfile;
		outfile.open(gameModule.VarLogName->ValueString, std::ios_base::app);
		if (outfile.fail())
			return;

		LogEntry entry;
		while (Entries.pop(entry))
		{
			std::string message(entry.Message);
			delete[] entry.Message;

			for (auto filter : gameModule.FiltersExclude)
			{
				std::string lowercaseMessage = Utils::String::ToLower(message);
				std::string lowercaseFilter = Utils::String::ToLower(filter);
				if (strstr(lowercaseMessage.c_str(), lowercaseFilter.c_str()) != nullptr)
					goto closefile; // string contains an excluded string
			}

			for (auto filter : gameModule.FiltersInclude)
			{
				std::string lowercaseMessage = Utils::String::ToLower(message);
				std::string lowercaseFilter = Utils::String::ToLower(filter);
				if (strstr(lowercaseMessage.c_str(), lowercaseFilter.c_str()) != nullptr)
					continue;

				goto closefile;  // string doesn't contain an included string
			}

			time_t t = std::chrono::system_clock::to_time_t(entry.Time);
			tm ourLocalTime;
			if (localtime_s(&ourLocalTime, &t) != 0)
				goto closefile;

			outfile << '[' << std::put_time(&ourLocalTime, "%H:%M:%S") << "] " << LogTypesToString(entry.Type) << " - " << message << '\n';
		}

		closefile:
		outfile.close();
	}
}
