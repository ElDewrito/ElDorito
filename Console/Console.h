#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

namespace Console
{
	enum Color : uint8_t
	{
		None = 0,
		Bright = 0x8,
		Red = 0x4,
		Blue = 0x1,
		Green = 0x2,
		Yellow = Red | Green,
		Cyan = Green | Blue,
		Magenta = Red | Blue,
		Normal = Red | Blue | Green,
		// Bitshift colors left 4 bits to get background version
		BackBright = Bright << 4,
		BackRed = Red << 4,
		BackBlue = Blue << 4,
		BackGreen = Green << 4,
		BackYellow = Yellow << 4,
		BackCyan = Cyan << 4,
		BackMagenta = Magenta << 4,
		// Prefabs
		Error = Red,
		Info = Bright | Yellow,
		Input = Bright | Cyan,
		Suggestion = Cyan
	};

	class Console
	{
	public:
		Console();
		~Console();

		void SetTextColor(uint8_t Color);

		void HandleInput(uint32_t KeyCode);
		void PrintLine();

	private:

		// History
		std::vector<std::vector<std::string>> PrevCommands;
		std::vector<std::vector<std::string>>::iterator PrevCommand;

		// Current Command
		size_t CurArg;
		std::vector<std::string> CurCommand;
		std::string Suggestion;

		// Console Data
		size_t ConsoleWidth;
		void* ConsoleHandle;
	};

	bool AllocateConsole(const std::string& ConsoleTitle);
}