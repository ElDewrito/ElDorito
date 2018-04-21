#include "ElDorito.hpp"

#include "Utils/Utils.hpp"
#include "ElPatches.hpp"
#include "Patches/Network.hpp"
#include "Server/DedicatedServer.hpp"
#include "Server/Stats.hpp"
#include "Server/ServerChat.hpp"
#include "Server/VariableSynchronization.hpp"
#include "Server/BanList.hpp"
#include "Server/Rcon.hpp"
#include "Server/Signaling.hpp"
#include "Patches/Core.hpp"
#include "Console.hpp"
#include "Web/Ui/WebScoreboard.hpp"
#include "Web/Ui/ScreenLayer.hpp"
#include "Web/Ui/WebChat.hpp"
#include "Web/Ui/WebConsole.hpp"
#include "Web/Ui/WebLoadingScreen.hpp"
#include "Web/Ui/VotingScreen.hpp"
#include "Web/Ui/MpEventDispatcher.hpp"
#include "Web/Ui/WebVirtualKeyboard.hpp"
#include "ElModules.hpp"
#include "Modules/ModuleGame.hpp"
#include "Patch.hpp"
#include "Modules/ModuleCamera.hpp"
#include "Modules/ModuleInput.hpp"
#include "Server/Voting.hpp"
#include "ChatCommands/ChatCommandMap.hpp"
#include "Patches/Weapon.hpp"
#include "Patches/Memory.hpp"
#include "Patches/Camera.hpp"
#include "Discord/DiscordRPC.h"
#include "ThirdParty/SOP.hpp"

#include "Blam/Cache/StringIdCache.hpp"

#include <Windows.h>
#include <TlHelp32.h>
#include <ShlObj.h>
#include <codecvt>
#include <detours.h>
#include "Web/Ui/WebSettings.hpp"

size_t ElDorito::MainThreadID = 0;

ElDorito::ElDorito()
{
}

bool(__cdecl * ElDorito::Video_InitD3D)(bool, bool) = (bool(__cdecl *) (bool, bool)) 0xA21B40;

bool __cdecl ElDorito::hooked_Video_InitD3D(bool windowless, bool nullRefDevice) {
	// TEMP: leave window enabled for now so async networkWndProc stuff still works
	return (*Video_InitD3D)(false, nullRefDevice);
}

void ElDorito::killProcessByName(const char *filename, int ourProcessID)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			if (pEntry.th32ProcessID != ourProcessID)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);

				if (hProcess != NULL)
				{
					TerminateProcess(hProcess, 9);
					CloseHandle(hProcess);
				}
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

void ElDorito::Initialize()
{
	::CreateDirectoryA(GetDirectory().c_str(), NULL);

	if (!SOP_CheckProfile("ElDewrito"))
	{
		SOP_SetProfile("ElDewrito", "eldorado.exe");
	}

	// Parse command-line commands
	int numArgs = 0;
	LPWSTR* szArgList = CommandLineToArgvW(GetCommandLineW(), &numArgs);

	//Check for the instance switch before initializing anything
	if (szArgList && numArgs > 1)
	{
		for (int i = 1; i < numArgs; i++)
		{
			std::wstring arg = std::wstring(szArgList[i]);
			if (arg.compare(L"-instance") == 0 && i < numArgs - 1)
			{
				instanceName = Utils::String::ThinString(szArgList[i + 1]);
			}
		}
	}

	// init our command modules
	Console::Init();
	Modules::ElModules::Instance();
	Server::TempBanList::Instance();

	//Get the local appdata folder
	PWSTR localAppdata;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, NULL, NULL, &localAppdata);
	auto wide_ed_appdata = std::wstring(localAppdata);
	auto ed_appdata = Utils::String::ThinString(wide_ed_appdata);
	ed_appdata += "\\ElDewrito";
	::CreateDirectoryA(ed_appdata.c_str(), NULL);

	// load variables/commands from cfg file
	// If instancing is enabled then load the instanced dewrito_prefs.cfg
	if (instanceName != "")
	{
		std::stringstream keystream;
		keystream << "Execute dewrito_prefs_" << instanceName << ".cfg";
		Modules::CommandMap::Instance().ExecuteCommand(keystream.str());

		std::stringstream ss;
		ss << "Execute " << ed_appdata << "\\keys_" << instanceName << ".cfg";
		Modules::CommandMap::Instance().ExecuteCommand(ss.str());
	}
	else
	{
		std::stringstream ss;
		ss << "Execute dewrito_prefs.cfg";
		Modules::CommandMap::Instance().ExecuteCommand(ss.str());

		std::stringstream keystream;
		keystream << "Execute " << ed_appdata << "\\keys.cfg";
		Modules::CommandMap::Instance().ExecuteCommand(keystream.str());
	}
	Modules::CommandMap::Instance().ExecuteCommand("Execute autoexec.cfg"); // also execute autoexec, which is a user-made cfg guaranteed not to be overwritten by ElDew

	// maybe use an unordered_map here
	if (!Modules::ModuleInput::Instance().IsCommandBound("game.takescreenshot"))
		Modules::CommandMap::Instance().ExecuteCommand("Bind PrintScreen Game.TakeScreenshot");
	if (!Modules::ModuleInput::Instance().IsCommandBound("game.showscreen discord"))
		Modules::CommandMap::Instance().ExecuteCommand("Bind F4 Game.ShowScreen discord");
	if (!Modules::ModuleInput::Instance().IsCommandBound("game.showscreen browser"))
		Modules::CommandMap::Instance().ExecuteCommand("Bind F11 game.showscreen browser");

	mapsFolder = "maps\\";

	skipTitleSplash = Modules::ModuleGame::Instance().VarSkipTitleSplash->ValueInt == 1;

	if( szArgList && numArgs > 1 )
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		for( int i = 1; i < numArgs; i++ )
		{
			std::wstring arg = std::wstring(szArgList[i]);
			if (arg.compare(0, 1, L"-") != 0) // if it doesn't start with -
				continue;

			if (arg.compare(L"-dedicated") == 0 || arg.compare(L"-headless") == 0)
			{
				isDedicated = true;
				Modules::CommandMap::Instance().ExecuteCommand("Server.Dedicated 1");
			}

			if (arg.compare(L"-maps") == 0 && i < numArgs - 1)
			{
				mapsFolder = Utils::String::ThinString(szArgList[i + 1]);
				if (mapsFolder.length() > 0 && mapsFolder.back() != '\\' && mapsFolder.back() != '/')
					mapsFolder += "\\";
			}

			if (arg.compare(L"-webdebug") == 0)
				webDebugging = true;

			if (arg.compare(L"-connect") == 0 && i < numArgs - 1)
			{
				serverAddress = Utils::String::ThinString(szArgList[i + 1]);
				connectToServer = true;
			}

			if (arg.compare(L"-password") == 0 && i < numArgs - 1)
				serverPassword = Utils::String::ThinString(szArgList[i + 1]);

			if (arg.compare(L"-cache-memory-increase") == 0 && i < numArgs - 1)
				Patches::Memory::SetGlobalCacheIncrease(std::stoul(szArgList[i + 1]));

			if (arg.compare(L"-lod-increase") == 0)
				Patches::Camera::IncreaseLOD();

			size_t pos = arg.find(L'=');
			if( pos == std::wstring::npos || arg.length() <= pos + 1 ) // if it doesn't contain an =, or there's nothing after the =
				continue;

			std::string argname = converter.to_bytes(arg.substr(1, pos - 1));
			std::string argvalue = converter.to_bytes(arg.substr(pos + 1));

			Modules::CommandMap::Instance().ExecuteCommand(argname + " \"" + argvalue + "\"", true);
		}
	}

#if _DEBUG
	// Always enable web debugging in debug builds
	webDebugging = true;
#endif

	Patches::Core::SetMapsFolder(mapsFolder);

	if (isDedicated)
	{
		Patches::Network::ForceDedicated();
		//// Commenting this out for now because it makes testing difficult
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach((PVOID*)&Video_InitD3D, &hooked_Video_InitD3D);

		if (DetourTransactionCommit() != NO_ERROR) {
			return;
		}
	}
	else
	{
		Web::Ui::ScreenLayer::Init();
		Web::Ui::MpEventDispatcher::Init();
		Web::Ui::WebChat::Init();
		Web::Ui::WebScoreboard::Init();
		Web::Ui::WebConsole::Init();
		Web::Ui::WebLoadingScreen::Init();
		Web::Ui::Voting::Init();
		Web::Ui::WebVirtualKeyboard::Init();
		Web::Ui::WebSettings::Init();

		if (connectToServer)
		{
			//Skip the title splash screen if we're trying to connect to a server.
			skipTitleSplash = true;

			std::stringstream connectString;
			connectString << "connect " << serverAddress << " " << serverPassword;
			Modules::CommandMap::Instance().ExecuteCommand(connectString.str());
		}
	}

	setWatermarkText("ElDewrito | Version: " + Utils::Version::GetVersionString() + " | Build Date: " __DATE__);

	// Ensure a ban list file exists
	Server::SaveDefaultBanList(Server::LoadDefaultBanList());

	// Initialize server modules
	Server::Chat::Initialize();
	ChatCommands::Init();
	Server::Stats::Init();
	Server::Voting::Init();
	Server::VariableSynchronization::Initialize();
	Server::Rcon::Initialize();
	Server::Signaling::Initialize();

	if (!Blam::Cache::StringIDCache::Instance.Load(mapsFolder + "string_ids.dat"))
	{
		std::string msg("Failed to load '" + mapsFolder + "string_ids.dat'!");
		MessageBox(NULL, msg.c_str(), "", MB_OK);
	}
}

void ElDorito::Tick()
{
	Server::VariableSynchronization::Tick();
	Server::Chat::Tick();
	Patches::Tick();
	if (!isDedicated) {
		Web::Ui::ScreenLayer::Tick();
		Web::Ui::WebScoreboard::Tick();
	}
	else if (GameHasMenuShown)
		Server::DedicatedServer::Tick();

	Server::Stats::Tick();
	Server::Voting::Tick();
	ChatCommands::Tick();
	Discord::DiscordRPC::Instance().Update();

	// TODO: refactor this elsewhere
	Modules::ModuleCamera::Instance().UpdatePosition();

	Utils::AntiCheat::OnTickCheck();

	if (executeCommandQueue)
	{
		Modules::CommandMap::Instance().ExecuteQueue();
		executeCommandQueue = false;
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
		OutputDebugString(std::string("Unable to resolve current directory, error code: ").append(std::to_string(Error)).c_str());
	}
	GetModuleFileNameA(hMod, Path, sizeof(Path));
	std::string Dir(Path);
	Dir = Dir.substr(0, std::string(Dir).find_last_of('\\') + 1);
	return Dir;
}

bool firstShow = true;

void ElDorito::OnMainMenuShown()
{
	if (GameHasMenuShown)
		return;
	GameHasMenuShown = true;
	executeCommandQueue = true;
	if (isDedicated)
		Server::DedicatedServer::Init();
	else
		Web::Ui::ScreenLayer::Show("title", "{}");
}

// This is for the watermark in the bottom right corner (hidden by default)
void ElDorito::setWatermarkText(const std::string& Message)
{
	static wchar_t msgBuf[256];
	wmemset(msgBuf, 0, 256);

	std::wstring msgUnicode = Utils::String::WidenString(Message);
	wcscpy_s(msgBuf, 256, msgUnicode.c_str());

	Pointer::Base(0x2E5338).Write<uint8_t>(0x68);
	Pointer::Base(0x2E5339).Write(&msgBuf);
	Pointer::Base(0x2E533D).Write<uint8_t>(0x90);
	Pointer::Base(0x2E533E).Write<uint8_t>(0x90);
}

void* _mainTLS;
Pointer ElDorito::GetMainTls(size_t tlsOffset)
{
	// cache the result allowing future cross-thread calls to succeed
	if (_mainTLS == nullptr)
	{
		_asm
		{
			mov     eax, dword ptr fs:[2Ch]
			mov     eax, dword ptr ds:[eax]
			mov		_mainTLS, eax
		}
	}

	return Pointer(_mainTLS)(tlsOffset);
}