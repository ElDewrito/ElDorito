#pragma once
#include <memory>
#include "../Utils/Utils.hpp"
#include "Queue.hpp"
#include "TextInput.h"

class Queue;	
class ConsoleQueue;
class GlobalChatQueue;
class GameChatQueue;

class GameConsole : public Utils::Singleton<GameConsole>
{
private:
	const size_t INPUT_MAX_CHARS = 512;
	bool capsLockToggled = false;
	void initIRCName();
	void handleDefaultKeyInput(USHORT vKey);

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
	Queue* selectedQueue = &gameChatQueue;
	Queue* lastChatQueue = &gameChatQueue;
	int lastTimeConsoleBlink = 0;
	bool consoleBlinking = false;
	bool showChat = false;
	bool showConsole = false;
	bool disableUI = false;

	GameConsole();
	void PushLineFromGameToUIQueues(std::string text);
	int getMsSinceLastConsoleOpen();
	int getMsSinceLastConsoleBlink();
	void consoleKeyCallBack(USHORT vKey);
	void SwitchToGlobalChat();
	void SwitchToGameChat();
	void mouseCallBack(RAWMOUSE mouseInfo);
	void hideConsole();
	void displayChat(bool console);
};