#include "GameConsole.hpp"
#include "../Utils/VersionInfo.hpp"
#include "../DirectXHook.hpp"
#include "KeyboardHook.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Patches/PlayerUid.hpp"
#include "../Pointer.hpp"
#include <openssl/sha.h>
#include <algorithm>
#include "../ElDorito.hpp"

void GameConsole::startIRCBackend()
{
	IRCBackend::Instance();
}

GameConsole::GameConsole()
{
	KeyboardHook::setHook();

	consoleQueue.pushLineFromGameToUI("ElDewrito Version: " + Utils::Version::GetVersionString() + " Build Date: " + __DATE__ + " " + __TIME__);
	consoleQueue.pushLineFromGameToUI("Enter help or help <command> to get started!");
	consoleQueue.pushLineFromGameToUI("Press page-up or page-down while chat is open to scroll.");

	Patches::PlayerUid::Get(); // ensure a UID is generated
	initPlayerName();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&startIRCBackend, 0, 0, 0);
}

bool GameConsole::isConsoleShown() {
	return boolShowConsole;
}

int GameConsole::getMsSinceLastReturnPressed()
{
	return GetTickCount() - lastTimeConsoleShown;
}

int GameConsole::getMsSinceLastConsoleOpen()
{
	return GetTickCount() - lastTimeConsoleShown;
}

void GameConsole::peekConsole()
{
	lastTimeConsoleShown = GetTickCount();
}

void GameConsole::hideConsole()
{
	lastTimeConsoleShown = GetTickCount();
	boolShowConsole = false;
	inputLine.clear();

	// Enables game keyboard input and disables our keyboard hook

	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x06;
	Rid.dwFlags = RIDEV_REMOVE;
	Rid.hwndTarget = 0;

	if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
		consoleQueue.pushLineFromGameToUI("Unregistering keyboard failed");
	}
}

void GameConsole::showConsole()
{
	boolShowConsole = true;
	capsLockToggled = GetKeyState(VK_CAPITAL) & 1;

	// Disables game keyboard input and enables our keyboard hook
	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x06;
	Rid.dwFlags = RIDEV_NOLEGACY; // adds HID keyboard and also ignores legacy keyboard messages
	Rid.hwndTarget = 0;

	if (!RegisterRawInputDevices(&Rid, 1, sizeof(Rid))) {
		consoleQueue.pushLineFromGameToUI("Registering keyboard failed");
	}
}

int currentBacklogIndex = -1;
bool tabHitLast = false;
int try_count = 0;
std::string command_before_completion = "";
std::vector<std::string> current_command_list = std::vector < std::string > {};

void GameConsole::virtualKeyCallBack(USHORT vKey)
{
	HWND hWnd = *((HWND*)0x199C014);
	if (hWnd != nullptr)
	{
		if (GetForegroundWindow() != hWnd)
		{
			return;
		}
	}
	if (!isConsoleShown())
	{
		if (vKey == VK_RETURN)
		{
			showConsole();
		}
		return;
	}

	switch (vKey)
	{
	case VK_RETURN:
		if (!inputLine.empty())
		{
			selectedQueue->unchangingBacklog.push_back(inputLine);
			selectedQueue->pushLineFromKeyboardToGame(inputLine);
		}
		hideConsole();
		lastTimeReturnPressed = GetTickCount();
		break;

	case VK_ESCAPE:
		hideConsole();
		break;

	case VK_BACK:
		if (!inputLine.empty())
		{
			inputLine.pop_back();
			currentBacklogIndex = -1;
		}
		break;

	case VK_F1:
		selectedQueue = &consoleQueue;
		selectedQueue->startIndexForUI = 0;
		consoleQueue.color = DirectXHook::COLOR_GREEN;
		globalChatQueue.color = DirectXHook::COLOR_YELLOW;
		gameChatQueue.color = DirectXHook::COLOR_YELLOW;
		currentBacklogIndex = -1;
		break;

	case VK_F2:
		selectedQueue = &globalChatQueue;
		selectedQueue->startIndexForUI = 0;
		consoleQueue.color = DirectXHook::COLOR_YELLOW;
		globalChatQueue.color = DirectXHook::COLOR_GREEN;
		gameChatQueue.color = DirectXHook::COLOR_YELLOW;
		currentBacklogIndex = -1;
		break;

	case VK_F3:
		selectedQueue = &gameChatQueue;
		selectedQueue->startIndexForUI = 0;
		consoleQueue.color = DirectXHook::COLOR_YELLOW;
		globalChatQueue.color = DirectXHook::COLOR_YELLOW;
		gameChatQueue.color = DirectXHook::COLOR_GREEN;
		currentBacklogIndex = -1;
		break;

	case VK_CAPITAL:
		capsLockToggled = !capsLockToggled;
		break;

	case VK_PRIOR:
		if (selectedQueue->startIndexForUI < selectedQueue->numOfLinesBuffer - selectedQueue->numOfLinesToShow)
		{
			selectedQueue->startIndexForUI++;
		}
		break;

	case VK_NEXT:
		if (selectedQueue->startIndexForUI > 0)
		{
			selectedQueue->startIndexForUI--;
		}
		break;

	case VK_UP:
		currentBacklogIndex++;
		if (currentBacklogIndex > (int)selectedQueue->unchangingBacklog.size() - 1)
		{
			currentBacklogIndex--;
		}
		if (currentBacklogIndex >= 0)
		{
			inputLine = selectedQueue->unchangingBacklog.at(selectedQueue->unchangingBacklog.size() - currentBacklogIndex - 1);
		}
		break;

	case VK_DOWN:
		currentBacklogIndex--;
		if (currentBacklogIndex < 0)
		{
			currentBacklogIndex = -1;
			inputLine = "";
		}
		else
		{
			inputLine = selectedQueue->unchangingBacklog.at(selectedQueue->unchangingBacklog.size() - currentBacklogIndex - 1);
		}
		break;

	case VK_TAB:
		if (dynamic_cast<ConsoleQueue*>(selectedQueue) && inputLine.find_first_of(" ") == std::string::npos && inputLine.length() > 0)
		{
			if (tabHitLast)
			{
				inputLine = current_command_list.at((++try_count) % current_command_list.size());
			}
			else
			{
				try_count = 0;
				current_command_list.clear();
				command_before_completion = inputLine;

				auto currentLine = inputLine;
				std::transform(currentLine.begin(), currentLine.end(), currentLine.begin(), ::tolower);

				for (auto cmd : Modules::CommandMap::Instance().Commands)
				{
					auto commandName = cmd.Name;
					std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);

					if (commandName.compare(0, currentLine.length(), currentLine) == 0)
					{
						current_command_list.push_back(commandName);
					}
				}
				if (current_command_list.size() > 0)
				{
					inputLine = current_command_list.at((++try_count) % current_command_list.size());
				}
			}
		}
		break;
	case 'V':
		if (GetAsyncKeyState(VK_CONTROL)) // CTRL+V pasting
		{
			if (OpenClipboard(nullptr))
			{
				HANDLE hData = GetClipboardData(CF_TEXT);
				if (hData)
				{
					char* textPointer = static_cast<char*>(GlobalLock(hData));
					std::string text(textPointer);
					inputLine += text;
					GlobalUnlock(hData);
				}
				CloseClipboard();
			}
		}
		else
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				inputLine += !capsLockToggled ? 'V' : 'v';
			}
			else
			{
				inputLine += !capsLockToggled ? 'v' : 'V';
			}
		}
		break;

	default:
		WORD buf;
		BYTE keysDown[256] = {};

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) // 0x8000 = 0b1000000000000000
		{
			keysDown[VK_SHIFT] = 0x80; // sets highest-order bit to 1: 0b10000000
		}

		if (capsLockToggled)
		{
			keysDown[VK_CAPITAL] = 0x1; // sets lowest-order bit to 1: 0b00000001
		}

		int retVal = ToAscii(vKey, 0, keysDown, &buf, 0);

		if (retVal == 1)
		{
			inputLine += buf & 0x00ff;
		}
		else if (retVal == 2)
		{
			inputLine += buf >> 8;
			inputLine += buf & 0x00ff;
		}
		break;
	}

	tabHitLast = vKey == VK_TAB;
}

void GameConsole::checkForReturnKey()
{
	if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && getMsSinceLastReturnPressed() > 500 && *((uint16_t*)0x244D24A) != 16256) { // 0x244D24A = 16256 means that tab is pressed in game (shows player k/d ratios)
		showConsole();
		lastTimeReturnPressed = GetTickCount();
	}
}

void GameConsole::initPlayerName()
{
	playerName = GenerateIRCNick(Modules::ModulePlayer::Instance().VarPlayerName->ValueString, Pointer::Base(0x15AB730).Read<uint64_t>());
}

std::string GameConsole::GenerateIRCNick(std::string name, uint64_t uid)
{
	std::string ircNick;
	Utils::String::BytesToHexString(&uid, sizeof(uint64_t), ircNick);
	ircNick += "|" + name;

	size_t maxLen = 27; // TODO: get max name len from server
	maxLen -= 3; // dew prefix

	if (ircNick.length() > maxLen)
		ircNick = ircNick.substr(ircNick.length() - maxLen, maxLen);

	ircNick = "dew" + ircNick;
	return ircNick;
}