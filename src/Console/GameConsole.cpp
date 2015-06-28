#include "GameConsole.hpp"
#include "../Utils/VersionInfo.hpp"
#include "../DirectXHook.hpp"
#include "KeyboardHook.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Patches/PlayerUid.hpp"
#include "../Pointer.hpp"
#include <openssl/sha.h>
#include <algorithm>

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

int GameConsole::getMsSinceLastConsoleOpen()
{
	return GetTickCount() - lastTimeConsoleShown;
}

int GameConsole::getMsSinceLastConsoleBlink()
{
	return GetTickCount() - lastTimeConsoleBlink;
}

void GameConsole::peekConsole()
{
	lastTimeConsoleShown = GetTickCount();
}

void GameConsole::hideConsole()
{
	lastTimeConsoleShown = GetTickCount();
	boolShowConsole = false;
	currentInput.set("");

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
	selectedQueue->startIndexForScrolling = 0;
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

void GameConsole::virtualKeyCallBack(USHORT vKey)
{
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
		if (!currentInput.currentInput.empty())
		{
			selectedQueue->unchangingBacklog.push_back(currentInput.currentInput);
			selectedQueue->pushLineFromKeyboardToGame(currentInput.currentInput);
		}
		hideConsole();
		break;

	case VK_ESCAPE:
		hideConsole();
		break;

	case VK_BACK:
		if (!currentInput.currentInput.empty())
		{
			currentInput.backspace();
		}
		break;

	case VK_DELETE:
		if (!currentInput.currentInput.empty())
		{
			currentInput.del();
		}
		break;

	case VK_F1:
		selectedQueue = &consoleQueue;
		selectedQueue->startIndexForScrolling = 0;
		consoleQueue.color = DirectXHook::COLOR_GREEN;
		globalChatQueue.color = DirectXHook::COLOR_YELLOW;
		gameChatQueue.color = DirectXHook::COLOR_YELLOW;
		currentBacklogIndex = -1;
		break;

	case VK_F2:
		selectedQueue = &globalChatQueue;
		selectedQueue->startIndexForScrolling = 0;
		consoleQueue.color = DirectXHook::COLOR_YELLOW;
		globalChatQueue.color = DirectXHook::COLOR_GREEN;
		gameChatQueue.color = DirectXHook::COLOR_YELLOW;
		currentBacklogIndex = -1;
		break;

	case VK_F3:
		selectedQueue = &gameChatQueue;
		selectedQueue->startIndexForScrolling = 0;
		consoleQueue.color = DirectXHook::COLOR_YELLOW;
		globalChatQueue.color = DirectXHook::COLOR_YELLOW;
		gameChatQueue.color = DirectXHook::COLOR_GREEN;
		currentBacklogIndex = -1;
		break;

	case VK_CAPITAL:
		capsLockToggled = !capsLockToggled;
		break;

	case VK_PRIOR:
		if (selectedQueue->startIndexForScrolling < selectedQueue->numOfLinesBuffer - selectedQueue->numOfLinesToShow)
		{
			selectedQueue->startIndexForScrolling++;
		}
		break;

	case VK_NEXT:
		if (selectedQueue->startIndexForScrolling > 0)
		{
			selectedQueue->startIndexForScrolling--;
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
			currentInput.currentInput = selectedQueue->unchangingBacklog.at(selectedQueue->unchangingBacklog.size() - currentBacklogIndex - 1);
		}
		break;

	case VK_DOWN:
		currentBacklogIndex--;
		if (currentBacklogIndex < 0)
		{
			currentBacklogIndex = -1;
			currentInput.currentInput = "";
		}
		else
		{
			currentInput.currentInput = selectedQueue->unchangingBacklog.at(selectedQueue->unchangingBacklog.size() - currentBacklogIndex - 1);
		}
		break;

	case VK_LEFT:
		currentInput.left();
		break;

	case VK_RIGHT:
		currentInput.right();
		break;

	case VK_TAB:
		if (dynamic_cast<ConsoleQueue*>(selectedQueue) && currentInput.currentInput.find_first_of(" ") == std::string::npos && currentInput.currentInput.length() > 0)
		{
			if (tabHitLast)
			{
				if (currentCommandList.size() > 0)
				{
					currentInput.set(currentCommandList.at((++tryCount) % currentCommandList.size()));
				}
			}
			else
			{
				tryCount = 0;
				currentCommandList.clear();
				commandPriorComplete = currentInput.currentInput;

				auto currentLine = currentInput.currentInput;
				std::transform(currentLine.begin(), currentLine.end(), currentLine.begin(), ::tolower);

				for (auto cmd : Modules::CommandMap::Instance().Commands)
				{
					auto commandName = cmd.Name;
					std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);

					if (commandName.compare(0, currentLine.length(), currentLine) == 0)
					{
						currentCommandList.push_back(commandName);
					}
				}
				consoleQueue.pushLineFromGameToUI(std::to_string(currentCommandList.size()) + " commands found starting with \"" + currentLine + ".\"");
				consoleQueue.pushLineFromGameToUI("Press tab to go through them.");
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
					currentInput.set(currentInput.currentInput + text);
					GlobalUnlock(hData);
				}
				CloseClipboard();
			}
		}
		else
		{

			if (GetAsyncKeyState(VK_SHIFT))
			{
				char vChar = !capsLockToggled ? 'V' : 'v';
				currentInput.type(vChar);
			}
			else
			{
				char vChar = !capsLockToggled ? 'v' : 'V';
				currentInput.type(vChar);
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
			currentInput.type(buf & 0x00ff);
		}
		else if (retVal == 2)
		{
			currentInput.type(buf >> 8);
			currentInput.type(buf & 0x00ff);
		}
		break;
	}

	tabHitLast = vKey == VK_TAB;
}

void GameConsole::mouseCallBack(RAWMOUSE mouseInfo)
{
	if (mouseInfo.usButtonFlags == RI_MOUSE_WHEEL)
	{
		if ((short) mouseInfo.usButtonData > 0)
		{
			if (selectedQueue->startIndexForScrolling < selectedQueue->numOfLinesBuffer - selectedQueue->numOfLinesToShow)
			{
				selectedQueue->startIndexForScrolling++;
			}
		}
		else
		{
			if (selectedQueue->startIndexForScrolling > 0)
			{
				selectedQueue->startIndexForScrolling--;
			}
		}
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