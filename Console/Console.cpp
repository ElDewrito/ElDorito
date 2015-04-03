#include "Console.h"

#include <Windows.h>
#include <stdio.h> //_open_osfhandle
#include <fcntl.h> //_O_TEXT
#include <io.h>

#include <conio.h> // _getch()
#include <cctype> //isgraph

#include <algorithm>

#define ps1beg "\xAF["
#define ps1end "]\xAE"

namespace Console
{
	Console::Console()
		:
		CurArg(0)
	{
		ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleOutputCP(437);

		CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
		if( GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleInfo) )
		{
			ConsoleWidth = ConsoleInfo.dwSize.X;
		}

		SetTextColor(Color::Info);
		CurCommand.push_back("");
	}

	Console::~Console()
	{
		Commands.clear();
	}

	void Console::SetTextColor(uint8_t Color)
	{
		SetConsoleTextAttribute(ConsoleHandle, Color);
	}

	void Console::HandleInput(uint32_t KeyCode)
	{
		if( std::isgraph(KeyCode) )
		{
			if( CurCommand.empty() )
			{
				CurCommand.push_back("");
			}
			CurCommand.back().push_back((uint8_t)KeyCode);

			// Suggest
			Suggestion.clear();
			if( CurArg == 0 )
			{
				// Find closest matching command

				std::vector<std::string> Suggestions;

				for( auto Cmd : Commands )
				{
					if( !CurCommand[0].compare(0, CurCommand[0].length(), ( Cmd.first ), 0, CurCommand[0].length()) )
					{
						Suggestions.push_back(( Cmd.first ));
					}
				}

				if( Suggestions.size() )
				{
					// Return first match
					Suggestion = Suggestions.front();
				}

				Suggestions.clear();
			}
			else
			{
				if( Commands.count(CurCommand.front()) )
				{
					Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
				}
			}
		}
		else if( KeyCode == ' ' ) // Space
		{
			if( !CurCommand.empty() && !CurCommand.back().empty() )
			{
				//Go to next argument;
				CurArg++;
				CurCommand.push_back("");
			}
			if( Commands.count(CurCommand.front()) )
			{
				Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
			}
		}
		else if( KeyCode == '\b' ) // Backspace
		{
			if( !CurCommand.empty() )
			{
				// Remove character from current command
				if( !CurCommand.back().empty() )
				{
					CurCommand.back().pop_back();
				}
				// Current argument is empty.
				// Go back an argument
				if( CurCommand.back().empty() )
				{
					CurCommand.pop_back();
					!CurArg || CurArg--;
				}
			}
		}
		else if( KeyCode == '\r' ) // Enter
		{
			SetTextColor(Color::Info);
			// Clear previous suggestion..
			if( !Suggestion.empty() )
			{
				size_t i;
				for( i = 0; i < Suggestion.length(); i++ )
				{
					std::cout << ' ';
				}
				for( i = 0; i < Suggestion.length() + ( CurCommand.size() ? 0 : 1 ); i++ )
				{
					std::cout << '\b';
				}
			}

			std::cout << ps1end << std::endl;
			if( !CurCommand.empty() )
			{
				if( CurCommand.back().empty() )
				{
					CurCommand.pop_back();
				}
				// Run command
				if( Commands.count(CurCommand[0]) && Commands[CurCommand[0]] )
				{
					if( !Commands[CurCommand[0]]->Run(CurCommand) )
					{
						// Error running command;
						SetTextColor(Color::Error);
						std::cout << "Error running command: " << CurCommand[0] << std::endl;
						SetTextColor(Color::Info);
						std::cout << Commands[CurCommand[0]]->Info();
					}
				}
				else
				{
					SetTextColor(Color::Error);
					std::cout << "Unknown Command: " << CurCommand[0];
					SetTextColor(Color::Info);
				}
			}

			std::cout << std::endl;
			SetTextColor(Color::Info);
			PrevCommands.push_back(CurCommand);
			PrevCommand = PrevCommands.end();
			CurArg = 0;
			CurCommand.clear();
		}
		else if( KeyCode == '\t' ) // Tab
		{
			// Get suggestion
			if( !Suggestion.empty() && !CurCommand.empty() )
			{
				CurCommand.back() = Suggestion;
				Suggestion.clear();
				CurArg++;
				CurCommand.push_back("");
				Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
				// Todo: Allow suggestions with spaces.
				// Simulate keypress with HandleInput
			}
		}
		else if( KeyCode == 22 ) // Ctrl+v
		{
			// Paste in clipbard
			if( OpenClipboard(nullptr) )
			{
				std::string Clipboard((char*)GetClipboardData(CF_TEXT));
				for( char character : Clipboard )
				{
					HandleInput(character);
				}
			}
		}
		else if( KeyCode == 0 || KeyCode == 0xE0 ) // Escape character
		{
			size_t Func = _getch();
			if( Func == 0x48 ) // Up
			{
				if( !PrevCommands.empty() )
				{
					if( PrevCommand != PrevCommands.begin() )
					{
						PrevCommand--;
					}

					CurCommand = *PrevCommand;
					CurArg = CurCommand.size() - 1;
				}
			}
			else if( Func == 0x50 ) // Down
			{
				if( !PrevCommands.empty() )
				{
					if( PrevCommand != PrevCommands.end() && PrevCommand != PrevCommands.end() - 1 )
					{
						PrevCommand++;
						CurCommand = *PrevCommand;
						CurArg = CurCommand.size() - 1;
					}
					else if( PrevCommand == PrevCommands.end() - 1 )
					{
						PrevCommand++;
						CurArg = 0;
						CurCommand.clear();
					}
					else
					{
						CurArg = 0;
						CurCommand.clear();
					}
				}
			}
			else if( Func == 0x4B ) // Left
			{
			}
			else if( Func == 0x4D ) // Right
			{
			}
			else
			{
				// Unknown function key
			}
		}
	}

	void Console::PrintLine()
	{
		SetTextColor(Color::Info);
		std::cout << '\r' << std::string(ConsoleWidth - 2, ' ') << '\r' << ps1beg;
		SetTextColor(Color::Input);

		for( size_t i = 0; i < CurCommand.size(); i++ )
		{
			if( Suggestion.length() && i == CurArg )
			{
				SetTextColor(Color::Suggestion);
				std::cout << Suggestion;
				std::cout << std::string(Suggestion.length(), '\b');
				SetTextColor(Color::Input);
				//Suggestion.clear();
				std::cout << CurCommand[i];
			}
			else
			{
				std::cout << CurCommand[i];
				( i == CurCommand.size() - 1 ) || std::cout << ' ';
			}
		}
		std::cout.flush();
	}

	void Console::PushCommand(const std::string& CommandName, std::shared_ptr<Command> Command)
	{
		Commands[CommandName] = Command;
	}
	void Console::PopCommand(const std::string& CommandName)
	{
		if( Commands.count(CommandName) )
		{
			Commands.erase(CommandName);
		}
	}

	bool AllocateConsole(const std::string& ConsoleTitle)
	{
		//Set up new co-nsole window

		HANDLE hStd;

		AllocConsole();
		SetConsoleTitleA(ConsoleTitle.c_str());
		EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE & SC_MINIMIZE | SC_MAXIMIZE, MF_GRAYED);
		DrawMenuBar(GetConsoleWindow());

		// Setup standard input.
		hStd = GetStdHandle(STD_INPUT_HANDLE); // STD in Handle
		unsigned int hConsole = _open_osfhandle((intptr_t)hStd, _O_TEXT);
		FILE* fIn = _fdopen(hConsole, "r");
		*stdin = *fIn;
		setvbuf(stdin, NULL, _IONBF, 0);

		// Setup standard output.
		hStd = GetStdHandle(STD_OUTPUT_HANDLE); // STD out Handle
		hConsole = _open_osfhandle((intptr_t)hStd, _O_TEXT); // Console Handle
		FILE* fOut = _fdopen(hConsole, "w");
		*stdout = *fOut;
		setvbuf(stdout, NULL, _IONBF, 0);

		CONSOLE_SCREEN_BUFFER_INFOEX ConsoleInfo;

		GetConsoleScreenBufferInfoEx(hStd, &ConsoleInfo);
		ConsoleInfo.dwSize.Y = 25;
		ConsoleInfo.dwSize.X = 30;
		ConsoleInfo.bFullscreenSupported = true;
		SetConsoleScreenBufferInfoEx(hStd, &ConsoleInfo);

		DWORD ConsoleMode;
		GetConsoleMode(hStd, &ConsoleMode);
		ConsoleMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
		SetConsoleMode(hStd, ConsoleMode);

		std::ios::sync_with_stdio();

		return true;
	}
}