#include "ElDorito.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <Windows.h>
#include <conio.h> // _getch()
#include <cctype> //isprint

#include <codecvt>
#include <cvt/wstring> // wstring_convert

#include "StringUtils.h"
#include "ElModules.h"

#define ps1 "¯["

size_t ElDorito::MainThreadID = 0;

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

	// English patch
	Patch(0x2333FD, { 0 }).Apply();

	// Enable tag edits
	Patch(0x101A5B, { 0xEB }).Apply();
	Patch::NopFill(Pointer::Base(0x102874), 2);

	// stopgame patch
	Patch(0x1056D0, { 0xC3 }).Apply();

	// No --account args patch
	Patch(0x43731A, { 0xEB, 0x0E }).Apply();
	Patch(0x4373AD, { 0xEB, 0x03 }).Apply();

	// Fix gamepad option in settings (todo: find out why it doesn't detect gamepads
	// and find a way to at least enable pressing ESC when gamepad is enabled)
	Patch::NopFill(Pointer::Base(0x20D7F2), 2);

	// Update window title patch
	const uint8_t windowData[] = { 0x3A, 0x20, 0x45, 0x6C, 0x20, 0x44, 0x6F, 0x72, 0x69, 0x74, 0x6F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00 };
	Pointer::Base(0x159C02F).Write(windowData, sizeof(windowData));
	Pointer::Base(0x159C06F).Write(windowData, sizeof(windowData));
	Pointer::Base(0x1EAAEE0).Write(windowData, sizeof(windowData));

	//Command list
	Commands["help"] = nullptr;
	Commands["history"] = nullptr;
	Commands["quit"] = nullptr;
	Commands["exit"] = nullptr;
	Commands["load"] = std::make_unique<LoadLevel>();
	Commands["god"] = std::make_unique<Godmode>();
	Commands["ammo"] = std::make_unique<Ammo>();
	Commands["hud"] = std::make_unique<Hud>();
	Commands["fov"] = std::make_unique<Fov>();
	//Commands["third"] = std::make_unique<ThirdPerson>();
	Commands["debug"] = std::make_unique<DebugLogging>();
	Commands["spawn"] = std::make_unique<Spawn>();
	Commands["global"] = std::make_unique<Globals>();
	Commands["camera"] = std::make_unique<Camera>();

	//Commands["test"] = std::make_unique<Test>();

	SetSessionMessage("ElDorito: Build Date: " __DATE__);

	// Parse command-line commands
	int numArgs = 0;
	LPWSTR* szArgList = CommandLineToArgvW(GetCommandLineW(), &numArgs);
	if( szArgList && numArgs > 1 )
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		for( int i = 1; i < numArgs; i++ )
		{
			std::wstring arg = std::wstring(szArgList[i]);
			if( arg.compare(0, 1, L"-") != 0 ) // if it doesn't start with -
				continue;

			size_t pos = arg.find(L"=");
			if( pos == std::wstring::npos || arg.length() <= pos + 1 ) // if it doesn't contain an =, or there's nothing after the =
				continue;

			std::string argname = converter.to_bytes(arg.substr(1, pos - 1));
			std::string argvalue = converter.to_bytes(arg.substr(pos + 1));

			if( Commands.count(argname) != 1 || Commands[argname] == nullptr ) // command not registered
				continue;

			Commands[argname]->Run({ argname, argvalue });
		}
	}

	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << ps1;
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
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

				// tolower args
				for(auto &i : Args)
					i = Utils::ToLower(i);

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
				{
					// If the command fails, print usage
					if( !Commands[Args[0]]->Run(Args) )
					{
						std::cout << "Invalid usage." << std::endl;
						std::cout << Commands[Args[0]]->Info() << std::endl;
					}
				}

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
							std::cout << "Unknown Command " << Args[1] << std::endl;
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
				else if( !Args[0].compare("quit") || !Args[0].compare("exit") )
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
					std::cout << "Exiting" << std::endl;
					std::exit(0);
				}
				else if (!Args[0].compare("cls") || !Args[0].compare("clear")) {
					system("cls");
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
		else if( Key == 22 )// Ctrl+v (paste clipboard into terminal)
		{
			// Paste in clipbard
			if( OpenClipboard(nullptr) )
			{
				std::string Clipboard((char*)GetClipboardData(CF_TEXT));
				for( char character : Clipboard )
				{
					Command.push_back(character);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					std::cout << (char)character;
				}
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
					if (PrevCommand != PrevCommands.end() && PrevCommand != PrevCommands.end() - 1){
						PrevCommand++;
						Command = *PrevCommand;
					}
					else if (PrevCommand == PrevCommands.end() - 1){
						Command.clear();
						PrevCommand++;
					}
					else Command.clear();

					

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
		std::cout << "Unable to resolve current directory, error code: " << Error << std::endl;
	}
	GetModuleFileNameA(hMod, Path, sizeof(Path));
	std::string Dir(Path);
	Dir = Dir.substr(0, std::string(Dir).find_last_of('\\') + 1);
	return Dir;
}

void ElDorito::SetSessionMessage(const std::string& Message)
{
	DWORD temp;
	VirtualProtect(Pointer::Base(0x120CCB8), Message.length() + 1, PAGE_EXECUTE_READWRITE, &temp);
	Pointer::Base(0x120CCB8).Write(Message.c_str(), Message.length() + 1);
}

Pointer ElDorito::GetMainTls(size_t Offset /*= 0*/)
{
	static Pointer ThreadLocalStorage;
	if( !ThreadLocalStorage && GetMainThreadID() )
	{
		size_t MainThreadID = GetMainThreadID();

		HANDLE MainThreadHandle = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, false, MainThreadID);

		// Get thread context
		CONTEXT MainThreadContext;
		MainThreadContext.ContextFlags = CONTEXT_FULL;

		SuspendThread(MainThreadHandle);
		BOOL success = GetThreadContext(MainThreadHandle, &MainThreadContext);
		if( !success )
		{
			std::cout << "Error getting thread context: " << GetLastError() << std::endl;
		}
		ResumeThread(MainThreadHandle);

		// Get thread selector

		LDT_ENTRY MainThreadLdt;

		success = GetThreadSelectorEntry(MainThreadHandle, MainThreadContext.SegFs, &MainThreadLdt);
		if( !success )
		{
			std::cout << "Error getting thread context: " << GetLastError() << std::endl;
		}
		size_t TlsPtrArrayAddress = (size_t)((size_t)(MainThreadLdt.HighWord.Bits.BaseHi << 24) | (MainThreadLdt.HighWord.Bits.BaseMid << 16) | MainThreadLdt.BaseLow) + 0x2C;
		size_t TlsPtrAddress = Pointer(TlsPtrArrayAddress).Read<uint32_t>();

		// Index has been consistantly 0. Keep a look out.
		ThreadLocalStorage = Pointer(TlsPtrAddress)[0];
	}

	return ThreadLocalStorage(Offset);
}