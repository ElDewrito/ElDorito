#pragma once
#include <memory>
#include "../Utils/Utils.hpp"
#include "ConsoleQueue.hpp"
#include "GlobalChatQueue.hpp"
#include "GameChatQueue.hpp"
#include "IRCBackend.hpp"

class Queue;
class ConsoleQueue;
class GlobalChatQueue;
class GameChatQueue;

class GameConsole : public Utils::Singleton<GameConsole>
{
private:
	bool boolShowConsole = false;
	bool capsLockToggled = false;
	int lastTimeReturnPressed = 0;
	int lastTimeConsoleShown = 0;
	std::unique_ptr<IRCBackend> ircBackend = nullptr;

	static void startIRCBackend();
	int getMsSinceLastReturnPressed();
	void hideConsole();
	void showConsole();
	void initPlayerName();

public:
	std::string inputLine = "";
	ConsoleQueue consoleQueue = ConsoleQueue();
	GlobalChatQueue globalChatQueue = GlobalChatQueue();
	GameChatQueue gameChatQueue = GameChatQueue();
#ifdef _DEBUG
	Queue* selectedQueue = &consoleQueue;
#else
	Queue* selectedQueue = &globalChatQueue;
#endif
	std::string playerName = "";

	GameConsole();
	bool isConsoleShown();
	int getMsSinceLastConsoleOpen();
	void peekConsole();
	void virtualKeyCallBack(USHORT vKey);
	void GameConsole::checkForReturnKey();
};