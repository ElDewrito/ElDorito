#pragma once
#include "Singleton.hpp"
#include <string>
#include <boost/lockfree/queue.hpp>
#include <mutex>
#include <windows.h>

namespace Utils
{
	enum class LogLevel
	{
		None = 0,
		Trace,
		Info,
		Warning,
		Error
	};

	std::string LogLevelToString(LogLevel level);

	enum LogTypes
	{
		None = 0,
		Game = 1 << 0,
		Network = 1 << 1,
		Graphics = 1 << 2,
		Memory = 1 << 3,
		Sound = 1 << 4,
		Input = 1 << 5,
		Debug = 1 << 31,
		All = 0xFFFFFFFF
	};

	std::string LogTypesToString(LogTypes types);

	struct LogEntry
	{
		std::chrono::time_point<std::chrono::system_clock> Time;
		LogTypes Type;
		const char* Message;
	};

	class Logger : public Singleton<Logger>
	{
		std::mutex flushMutex;
		static DWORD WINAPI Flusher(LPVOID lpParam);

	public:
		LogLevel Level;
		LogTypes Types;
		boost::lockfree::queue<LogEntry, boost::lockfree::fixed_sized<false>> Entries { 0 };
		
		Logger();

		void Log(LogTypes type, LogLevel level, std::string message, ...);
		void Flush();
	};
}
