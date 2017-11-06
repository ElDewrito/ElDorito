#pragma once
#include <string>
#include <memory>

namespace Console
{
	// Base class for objects which handle console output.
	class ConsoleOutputHandler
	{
	public:
		virtual ~ConsoleOutputHandler() {}

		// Writes a line to the output.
		virtual void WriteLine(const std::string &line) = 0;
	};

	// Initializes the console.
	void Init();

	// Writes one or more lines to the console.
	void WriteLine(const std::string &line);

	// Registers an output handler.
	void RegisterHandler(std::shared_ptr<ConsoleOutputHandler> handler);
}
