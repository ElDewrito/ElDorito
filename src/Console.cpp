#include "Console.hpp"
#include "Utils/String.hpp"
#include <vector>
#include "Utils/Logger.hpp"

using namespace Console;

namespace
{
	std::vector<std::shared_ptr<ConsoleOutputHandler>> Handlers;

	class LogConsoleOutputHandler : public ConsoleOutputHandler
	{
	public:
		void WriteLine(const std::string &line) override;
	};
}

namespace Console
{
	void Init()
	{
		// Register a log output by default
		RegisterHandler(std::make_shared<LogConsoleOutputHandler>());
	}

	void WriteLine(const std::string &line)
	{
		// Split the string into lines and send each line to each handler
		// This makes things easy for all of the handlers
		auto lines = Utils::String::SplitString(line, '\n');
		for (auto &&lineToPrint : lines)
		{
			for (auto &&handler : Handlers)
				handler->WriteLine(lineToPrint);
		}
	}

	void RegisterHandler(std::shared_ptr<ConsoleOutputHandler> handler)
	{
		Handlers.push_back(handler);
	}
}

namespace
{
	void LogConsoleOutputHandler::WriteLine(const std::string &line)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Info, "%s", line.c_str());
	}
}