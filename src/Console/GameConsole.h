#pragma once
#include <winsock2.h>
#include <vector>
#include <memory>
#include "DirectXHook.h"
#include "KeyboardHook.h"
#include "IRCBackend.h"
#include "../Patch.h"
#include "../Utils/Utils.h"

class GameConsole : public Utils::Singleton<GameConsole>
{
private:
	bool boolShowConsole = false;
	bool capsLockToggled = false;
	int lastTimeReturnPressed = 0;
	int lastTimeConsoleShown = 0;
	std::vector<std::string> queue = std::vector <std::string> {}; // index 0 is newest command; the higher the index, the older the command
	int numOfLinesBuffer = 100;
	std::unique_ptr<IRCBackend> ircBackend = nullptr;

	void pushLineFromKeyboardToGame(std::string line);
	void initPlayerName();
	static void startIRCBackend();
	int getMsSinceLastReturnPressed();
	void hideConsole();
	void showConsole();
	std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);

public:
	GameConsole();
	std::string playerName = "";
	std::string sendThisLineToIRCServer = "";
	std::string inputLine = "";
	int numOfLinesToShow = 12;
	int queueStartIndexForUI = 0;

	bool isConsoleShown();
	int getMsSinceLastConsoleOpen();
	void peekConsole();
	std::string at(int i);
	void virtualKeyCallBack(USHORT vKey);
	void pushLineFromGameToUI(std::string line);
	void pushLineFromGameToUIMultipleLines(std::string multipleLines);
	void GameConsole::checkForReturnKey();
};