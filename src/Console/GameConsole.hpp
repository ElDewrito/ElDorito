#pragma once
#include <memory>
#include "../Utils/Utils.hpp"
#include "Queue.hpp"
#include "TextInput.h"

class Queue;	
class ConsoleQueue;
class GameChatQueue;

namespace Blam
{
	namespace Input
	{
		struct KeyEvent;
	}
}

class GameConsole : public Utils::Singleton<GameConsole>
{
private:
	const size_t INPUT_MAX_CHARS = 512;
	bool keyDownCallBack(const Blam::Input::KeyEvent& key);
	bool keyTypedCallBack(const Blam::Input::KeyEvent& key);
	bool gameChatEnabled = false;

public:
	ConsoleQueue consoleQueue = ConsoleQueue();
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
	bool consoleKeyCallBack();
	void mouseCallBack(RAWMOUSE mouseInfo);
	void hideConsole();
	void displayChat(bool console);
	void gameInputCallBack();
};