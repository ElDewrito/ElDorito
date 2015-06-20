#pragma once
#include <winsock2.h>
#include "DirectXHook.h"
#include "KeyboardHook.h"
#include "IRCBackend.h"
#include <vector>
#include <boost/algorithm/string/predicate.hpp>

class GameConsole
{
private:
	GameConsole();
	GameConsole(GameConsole const&); // Do not implement
	void operator=(GameConsole const&); // Do not implement

	bool boolShowConsole = false;
	int lastTimeConsoleShown = 0;
	std::vector<std::string> queue = std::vector < std::string > {}; // index 0 is oldest command; the higher the index, the more recent the command
	int numOfLines = 8;
	std::string inputLine = "";
	std::string playerName = "";

	void pushLineFromKeyboardToGame(std::string line);
	void initPlayerName();
	void enableGameKeyboardInput();
	void disableGameKeyboardInput();

public:
	std::string sendThisLineToIRCServer = "";

	bool isConsoleShown();
	int getMsSinceLastReturnPressed();
	int getMsSinceLastConsoleOpen();
	void peekConsole();
	void hideConsole();
	void showConsole();
	int getNumOfLines();
	std::string at(int i);
	void virtualKeyCallBack(USHORT vKey);
	std::string getInputLine();
	void pushLineFromGameToUI(std::string line);
	std::string getPlayerName();

	static GameConsole& getInstance()
	{
		static GameConsole instance;
		return instance;
	}
};