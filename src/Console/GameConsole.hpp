#pragma once
#include <memory>
#include "../Utils/Utils.hpp"
#include "Queue.hpp"
#include "IRCBackend.hpp"
#include "TextInput.h"

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
	static void startIRCBackend();
	int getMsSinceLastReturnPressed();
	void hideConsole();
	void showConsole();
	void initPlayerName();

public:
	ConsoleQueue consoleQueue = ConsoleQueue();
	GlobalChatQueue globalChatQueue = GlobalChatQueue();
	GameChatQueue gameChatQueue = GameChatQueue();
	TextInput currentInput;
	int currentBacklogIndex = -1;
	bool tabHitLast = false;
	int tryCount = 0;
	std::string commandPriorComplete = "";
	std::vector<std::string> currentCommandList = std::vector < std::string > {};
#ifdef _DEBUG
	Queue* selectedQueue = &consoleQueue;
#else
	Queue* selectedQueue = &globalChatQueue;
#endif
	std::string playerName = "";
	int lastTimeConsoleBlink = 0;
	bool consoleBlinking = false;

	GameConsole();
	bool isConsoleShown();
	int getMsSinceLastConsoleOpen();
	int getMsSinceLastConsoleBlink();
	void peekConsole();
	void virtualKeyCallBack(USHORT vKey);
	void doTextInput(int characterIndex, char character);
	void checkForReturnKey();

	std::string GenerateIRCNick(std::string name, uint64_t uid);
};