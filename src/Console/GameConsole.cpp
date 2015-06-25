#include "GameConsole.hpp"
#include "../Utils/VersionInfo.hpp"
#include "../DirectXHook.hpp"
#include "KeyboardHook.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Patches/PlayerUid.hpp"
#include "../Pointer.hpp"
#include <openssl/sha.h>

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
		if (!inputLine.empty())
		{
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
		}
		break;

	case VK_F1:
		selectedQueue = &consoleQueue;
		selectedQueue->startIndexForUI = 0;
		consoleQueue.color = DirectXHook::COLOR_GREEN;
		globalChatQueue.color = DirectXHook::COLOR_YELLOW;
		gameChatQueue.color = DirectXHook::COLOR_YELLOW;
		break;

	case VK_F2:
		selectedQueue = &globalChatQueue;
		selectedQueue->startIndexForUI = 0;
		consoleQueue.color = DirectXHook::COLOR_YELLOW;
		globalChatQueue.color = DirectXHook::COLOR_GREEN;
		gameChatQueue.color = DirectXHook::COLOR_YELLOW;
		break;

	case VK_F3:
		selectedQueue = &gameChatQueue;
		selectedQueue->startIndexForUI = 0;
		consoleQueue.color = DirectXHook::COLOR_YELLOW;
		globalChatQueue.color = DirectXHook::COLOR_YELLOW;
		gameChatQueue.color = DirectXHook::COLOR_GREEN;
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