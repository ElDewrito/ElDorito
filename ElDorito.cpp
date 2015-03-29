#include "ElDorito.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <Windows.h>
#include <conio.h> // _getch()
#include <cctype> //isprint

#include "ElModules.h"

#define ps1 "¯["

ElDorito::ElDorito()
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleOutputCP(437);
	unsigned int ConsoleWidth = 80;
	CONSOLE_SCREEN_BUFFER_INFO ConsoleBuf;
	if( GetConsoleScreenBufferInfo(hStdout, &ConsoleBuf) )
	{
		ConsoleWidth = ConsoleBuf.dwSize.X;
	}

	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	std::cout << "ElDoritoÄÄÂBuild date (" << __DATE__ << " : " << __TIME__ << ")" << std::endl;
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	std::cout << "\t  ÃDEElekgolo (DEElekgolo@gmail.com)\n";
	std::cout << "\t  ÃLimited (zerogimp123@gmail.com)\n";
	std::cout << "\t  Àstoker25 (emoose)\n";
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "Enter \"";
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "help";
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "\" or \"";
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "help (command)";
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	std::cout << "\" to get started!" << std::endl;
	std::cout << "Current directory: ";
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << GetDirectory() << std::endl;
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_BLUE);
	std::cout << std::string(ConsoleWidth - 1, 'Ä') << std::endl;
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << ps1;
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	::CreateDirectoryA(GetDirectory().c_str(), NULL);

	// Enable write to all executable memory
	size_t Offset, Total;
	Offset = Total = 0;
	MEMORY_BASIC_INFORMATION MemInfo;

	//printf("\nUnprotecting memory...");
	while( VirtualQuery((uint8_t*)GetBasePointer() + Offset, &MemInfo, sizeof(MEMORY_BASIC_INFORMATION)) )
	{
		Offset += MemInfo.RegionSize;
		if( MemInfo.Protect == PAGE_EXECUTE_READ )
		{
			//printf("%0X\n", (size_t)((uint8_t*)GetBasePointer() + Offset));
			Total += MemInfo.RegionSize;
			VirtualProtect(MemInfo.BaseAddress, MemInfo.RegionSize, PAGE_EXECUTE_READWRITE, &MemInfo.Protect);
		}
	}
	//printf("\nDone! Unprotected %u bytes of memory\n", Total);

	// Eng patch
	Pointer::Base()(0x2333FD).Write<uint8_t>(0);

	// no argpatch
	//const uint8_t NOP[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	//memcpy((uint8_t*)GetBasePointer() + 0x4373AD, NOP, sizeof(NOP));

	//enable tag edits
	Pointer::Base()(0x101A5B).Write<uint8_t>(0xEB);
	Pointer::Base()(0x102874).Write<uint16_t>(0x9090);

	// stopgame patch
	Pointer::Base()(0x1056D0).Write<uint8_t>(0xC3);

	//Command list
	Commands["help"] = nullptr;
	Commands["history"] = nullptr;
	Commands["quit"] = nullptr;
	Commands["test"] = std::make_unique<Test>();
	Commands["load"] = std::make_unique<LoadLevel>();
	Commands["god"] = std::make_unique<Godmode>();
	Commands["ammo"] = std::make_unique<Ammo>();
}

void ElDorito::Tick(const std::chrono::duration<double>& DeltaTime)
{
	PrintConsole();
	for( auto& command : Commands )
		if( command.second )
			command.second->Tick(DeltaTime);
}

void ElDorito::PrintConsole()
{
	if( _kbhit() )
	{
		int Key = _getch();
		if( std::isprint(Key) )
		{
			Command.push_back(Key);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::cout << (char)Key;

			//Clear previous suggestion
			for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
				std::cout << " ";

			for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
				std::cout << "\b";

			PrevSuggestion.clear();

			std::vector<std::string> Suggest;
			for( auto it = Commands.begin(); it != Commands.end(); ++it )
				if( !Command.compare(0, Command.length(), (it->first), 0, Command.length()) )//includes command
					Suggest.push_back((it->first));

			if( Suggest.size() == 1 )
			{
				PrevSuggestion = Suggest[0];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
				std::cout << Suggest[0].substr(Command.length(), std::string::npos);

				//move back to end of input char
				for( unsigned int i = Command.length(); i < Suggest[0].length(); i++ )
					std::cout << "\b";
			}
			Suggest.clear();
		}
		else if( Key == '\b' )
		{
			if( !Command.empty() )
			{
				Command.pop_back();
				std::cout << "\b \b";
			}
			if( !PrevSuggestion.empty() )
			{
				//Clear previous suggestion
				for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
					std::cout << " ";

				for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
					std::cout << "\b";

				PrevSuggestion.clear();
			}
		}
		else if( Key == '\r' )
		{
			//enter
			if( !Command.empty() )
			{
				//Split up string into arguments
				std::stringstream ss(Command);
				std::istream_iterator<std::string> begin(ss);
				std::istream_iterator<std::string> end;
				std::vector<std::string> Args(begin, end);
				//Clear previous suggestion
				for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
					std::cout << " ";

				for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
					std::cout << "\b";

				PrevSuggestion.clear();

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

				std::cout << '\r' << ps1;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

				std::cout << Command;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

				std::cout << "]®" << std::endl;
				//process command and execute it
				if( Commands.count(Args[0]) == 1 && Commands[Args[0]] != nullptr )
					Commands[Args[0]]->Run(Args);

				else if( !Args[0].compare("help") )
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
					if( Args.size() == 2 )
					{
						if( Commands.count(Args[1]) == 1 && Commands[Args[1]] != nullptr )
						{
							std::cout << std::setfill('Ä');
							std::cout.width(48);
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
							std::cout << std::left
								<< ((char)std::toupper(Args[1][0]) + Args[1].substr(1))
								<< std::endl;
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
							std::cout << Commands[Args[1]]->Info() << std::endl;
						}
						else if( !Args[1].compare("help") )
						{
							std::cout << std::setfill('Ä');
							std::cout.width(48);
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
							std::cout << std::left
								<< "Help"
								<< std::endl;
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
							std::cout <<
								"Displays help for commands\n"
								"help [command] to get information related to a command\n"
								<< std::endl;
						}
						else if( !Args[1].compare("history") )
						{
							std::cout << std::setfill('Ä');
							std::cout.width(48);
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
							std::cout << std::left
								<< "history"
								<< std::endl;
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
							std::cout <<
								"Displays previously ran commands\n"
								"Press up or down in the console to cycle through previously ran commands\n"
								<< std::endl;
						}
						else
						{
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
							std::cout << "Unknown Command" << Args[1] << std::endl;
						}
					}
					else
					{
						std::cout << std::setfill('Ä');

						for( auto it = Commands.begin(); it != Commands.end(); ++it )
							if( it->second != nullptr )
							{
							std::cout.width(48);
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
							std::cout << std::left
								<< ((char)std::toupper(it->first[0]) + it->first.substr(1))
								<< std::endl;
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
							std::cout << it->second->Info() << std::endl;
							}
					}
				}
				else if( !Args[0].compare("history") )
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
					std::cout << "Previous commands:" << std::endl;
					std::vector<std::string>::iterator it;
					for( it = PrevCommands.begin(); it != PrevCommands.end(); ++it )
						std::cout << " -" << *it << std::endl;
				}
				else if( !Args[0].compare("quit") )
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
					std::cout << "Exiting" << std::endl;
					exit(0);
				}
				else
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
					std::cout << "Unknown Command" << std::endl;
				}
				PrevCommands.push_back(Command);
				PrevCommand = PrevCommands.end();
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				std::cout << ps1;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				Command.clear();
			}
		}
		else if( Key == '\t' )
		{
			//auto complete
			if( !Command.empty() )
			{
				//Clear previous suggestion
				for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
					std::cout << " ";

				for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
					std::cout << "\b";

				PrevSuggestion.clear();

				//Populate possible matches
				std::vector<std::string> Matches;
				for( auto it = Commands.begin(); it != Commands.end(); ++it )
					if( !Command.compare(0, Command.length(), (it->first), 0, Command.length()) )//includes command
						Matches.push_back((it->first));

				//Calculate common prefix if there are matches
				if( Matches.size() > 1 )
				{
					std::cout << std::endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
											FOREGROUND_RED |
											FOREGROUND_GREEN);
					//Print possible matches
					for( std::vector<std::string>::iterator it = Matches.begin(); it != Matches.end(); it++ )
						std::cout << " Ä" << (*it) << std::endl;

					std::string Prefix = Matches[0];
					for( std::vector<std::string>::iterator it = Matches.begin() + 1; it != Matches.end(); it++ )
					{
						std::string Temp;
						for( unsigned int i = 0; i < Prefix.length(); i++ )
						{
							if( i >= (*it).length() || Prefix[i] != (*it)[i] )
								break;
							else
								Temp.push_back(Prefix[i]);
						}
						Prefix = Temp;
					}
					Command = Prefix;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << ps1;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << Command;
				}
				else if( Matches.size() == 1 && Matches[0].compare(Command) != 0 )
				{
					Command = Matches[0];
					for( unsigned int i = 0; i < Command.length(); i++ )
						std::cout << "\b \b";

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << '\r' << ps1;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << Command;
				}
				Matches.clear();
			}
			else
			{
				std::cout << std::endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
				std::cout << "Available modules:" << std::endl;

				for( auto it = Commands.begin(); it != Commands.end(); ++it )
					std::cout << " Ä" << it->first << std::endl;

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				std::cout << ps1;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				std::cout << Command;
			}
		}
		else if( Key == 0 || Key == 0xe0 )//escape character
		{
			int Func = _getch();
			if( Func == 0x48 )
			{
				//up
				if( !PrevCommands.empty() )
				{
					//Clear previous suggestion
					for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
						std::cout << " ";

					for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
						std::cout << "\b";

					PrevSuggestion.clear();
					//backspace
					for( unsigned int i = 0; i < Command.length(); i++ )
						std::cout << "\b \b";

					//use previous command
					if( PrevCommand != PrevCommands.begin() )
						PrevCommand--;

					Command = *PrevCommand;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << '\r' << ps1;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << Command;
				}
			}
			else if( Func == 0x50 )
			{
				//down
				if( !PrevCommands.empty() )
				{
					//Clear previous suggestion
					for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
						std::cout << " ";

					for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
						std::cout << "\b";

					//backspace
					for( unsigned int i = 0; i < Command.length(); i++ )
						std::cout << "\b \b";

					PrevSuggestion.clear();
					//use previous command
					if( PrevCommand != PrevCommands.end() )
						PrevCommand++;

					Command = *PrevCommand;

					//Clear previous suggestion
					for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
						std::cout << " ";

					for( unsigned int i = Command.length(); i < PrevSuggestion.length(); i++ )
						std::cout << "\b";

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << '\r' << ps1;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << Command;
				}
			}
			else if( Func == 0x4B )
			{
				//left
			}
			else if( Func == 0x4D )
			{
				//right
			}
			else
			{
				//Unknown function key
			}
		}
		std::cout.flush();
	}
}

namespace
{
	static void HandleFinder()
	{
	};
}

std::string ElDorito::GetDirectory()
{
	char Path[MAX_PATH];
	HMODULE hMod;
	if( !GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&::HandleFinder, &hMod) )
	{
		int Error = GetLastError();
		std::cout << "Unable to resolve current directory" << std::endl;
	}
	GetModuleFileNameA(hMod, Path, sizeof(Path));
	std::string Dir(Path);
	Dir = Dir.substr(0, std::string(Dir).find_last_of('\\') + 1);
	return Dir;
}