#pragma once
#include <winsock2.h>
#include <vector>
#include <memory>
#include "DirectXHook.h"
#include "KeyboardHook.h"
#include "IRCBackend.h"

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
	std::unique_ptr<IRCBackend> ircBackend = nullptr;

	void pushLineFromKeyboardToGame(std::string line);
	void initPlayerName();
	void enableGameKeyboardInput();
	void disableGameKeyboardInput();
	static void startIRCBackend();

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