#include <sstream>

#include "Queue.hpp"
#include "GameConsole.hpp"
#include "IRCBackend.hpp"
#include "../DirectXHook.hpp"
#include "../ElModules.hpp"

Queue::Queue(D3DCOLOR color) : color(color)
{
	for (int i = 0; i < numOfLinesBuffer; i++) {
		queue.push_back("");
	}
}

void Queue::pushLineFromGameToUI(std::string line)
{
	for (int i = numOfLinesBuffer - 1; i > 0; i--)
	{
		queue.at(i) = queue.at(i - 1);
	}
	queue.at(0) = line;

	GameConsole::Instance().peekConsole();
}

GlobalChatQueue::GlobalChatQueue() : Queue(
#ifdef _DEBUG
	DirectXHook::COLOR_YELLOW
#else
	DirectXHook::COLOR_GREEN
#endif
	)
{
}

void GlobalChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	auto& ircBackend = IRCBackend::Instance();
	auto& console = GameConsole::Instance();
	ircBackend.sendMessageToChannel(ircBackend.globalChatChannel, &console.globalChatQueue, line);

	std::string preparedLineForUI = GameConsole::Instance().playerName;
	preparedLineForUI = preparedLineForUI.substr(preparedLineForUI.find_first_of("|") + 1, std::string::npos);
	preparedLineForUI += ": ";
	preparedLineForUI += line;
	pushLineFromGameToUI(preparedLineForUI);
}

GameChatQueue::GameChatQueue() : Queue(DirectXHook::COLOR_YELLOW)
{
}

void GameChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	auto& ircBackend = IRCBackend::Instance();
	auto& console = GameConsole::Instance();
	ircBackend.sendMessageToChannel(ircBackend.gameChatChannel, &console.gameChatQueue, line);

	std::string preparedLineForUI = GameConsole::Instance().playerName;
	preparedLineForUI = preparedLineForUI.substr(preparedLineForUI.find_first_of("|") + 1, std::string::npos);
	preparedLineForUI += ": ";
	preparedLineForUI += line;
	pushLineFromGameToUI(preparedLineForUI);
}

ConsoleQueue::ConsoleQueue() : Queue(
#ifdef _DEBUG
	DirectXHook::COLOR_GREEN
#else
	DirectXHook::COLOR_YELLOW
#endif
	)
{
}

std::vector<std::string>& ConsoleQueue::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

void ConsoleQueue::pushLineFromGameToUIMultipleLines(std::string multipleLines)
{
	std::vector<std::string> linesVector;
	split(multipleLines, '\n', linesVector);

	for (std::string line : linesVector)
	{
		pushLineFromGameToUI(line);
	}
}

void ConsoleQueue::pushLineFromKeyboardToGame(std::string line)
{
	pushLineFromGameToUI(line);
	pushLineFromGameToUIMultipleLines(Modules::CommandMap::Instance().ExecuteCommand(line, true));
}