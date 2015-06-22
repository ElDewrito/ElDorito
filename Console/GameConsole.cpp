#include "GameConsole.h"
#include "../Utils/VersionInfo.h"
#include "../CommandMap.h"
#include <sstream>

void GameConsole::startIRCBackend()
{
	GameConsole::Instance().ircBackend = std::make_unique<IRCBackend>();
}

GameConsole::GameConsole() : 
	DisableKeyboardInputPatch(0x112690, { 0xE9, 0x0B, 0x03, 0x00, 0x00, 0x90 })
{
	for (int i = 0; i < numOfLines; i++) {
		queue.push_back("");
	}

	initPlayerName();
	DirectXHook::hookDirectX();
	KeyboardHook::setHook();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&startIRCBackend, 0, 0, 0);

	pushLineFromGameToUI("ElDewrito Version: " + Utils::Version::GetVersionString() + " Build Date: " + __DATE__ + " " + __TIME__);
	pushLineFromGameToUI("Enter /help or /help <command> to get started!");
}

bool GameConsole::isConsoleShown() {
	return boolShowConsole;
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
	DisableKeyboardInputPatch.Apply(true);
}

void GameConsole::showConsole()
{
	boolShowConsole = true;
	DisableKeyboardInputPatch.Apply(false);
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
			pushLineFromKeyboardToGame(inputLine);
		}
		hideConsole();
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

	default:
		WORD buf;
		BYTE keysDown[256] = {};

		if (GetAsyncKeyState(VK_SHIFT))
		{
			keysDown[VK_SHIFT] = 0x80; // SHIFT down
		}

		if (GetAsyncKeyState(VK_CAPITAL))
		{
			keysDown[VK_SHIFT] = 0x80; // SHIFT down
			// keysDown[VK_CAPITAL] = 0x80; // Caps lock enabled
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

std::string GameConsole::getInputLine()
{
	return inputLine;
}

void GameConsole::pushLineFromKeyboardToGame(std::string line)
{
	if (line.find("/") == 0)
	{
		pushLineFromGameToUI(line);
		line.erase(0, 1);
		pushLineFromGameToUIMultipleLines(Modules::CommandMap::Instance().ExecuteCommand(line));
	}
	else
	{
		std::string preparedLineForIRC = playerName;
		preparedLineForIRC += ": ";
		preparedLineForIRC += line;
		sendThisLineToIRCServer = preparedLineForIRC;
		pushLineFromGameToUI(preparedLineForIRC);
	}
}

void GameConsole::initPlayerName()
{
	wchar_t* inGameName = (wchar_t*)0x19A03E8; // unicode
	std::wstring toWstr(inGameName);
	std::string toStr(toWstr.begin(), toWstr.end());
	playerName = toStr;
}

void GameConsole::pushLineFromGameToUI(std::string line)
{
	for (int i = 0; i < numOfLines - 1; i++)
	{
		queue.at(i) = queue.at(i + 1);
	}
	queue.at(numOfLines - 1) = line;

	peekConsole();
}

void GameConsole::pushLineFromGameToUIMultipleLines(std::string multipleLines)
{
	std::vector<std::string> linesVector;
	split(multipleLines, '\n', linesVector);

	for (std::string line : linesVector)
	{
		pushLineFromGameToUI(line);
	}
}

std::string GameConsole::getPlayerName()
{
	return playerName;
}

int GameConsole::getNumOfLines()
{
	return numOfLines;
}

std::string GameConsole::at(int i)
{
	return queue.at(i);
}

std::vector<std::string>& GameConsole::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}