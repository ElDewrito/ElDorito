#include <sstream>

#include "Queue.hpp"
#include "GameConsole.hpp"
#include "../DirectXHook.hpp"
#include "../ElModules.hpp"
#include "../Server/ServerChat.hpp"

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

	lastTimeQueueShown = GetTickCount();
}

GameChatQueue::GameChatQueue() : Queue(DirectXHook::COLOR_GREEN)
{
}

void GameChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	if (!Server::Chat::SendGlobalMessage(line))
		pushLineFromGameToUI("(Failed to send message! Are you in a game?)");
}

ConsoleQueue::ConsoleQueue() : Queue(0)
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
	pushLineFromGameToUI(">" + line);
	pushLineFromGameToUIMultipleLines(Modules::CommandMap::Instance().ExecuteCommand(line, true));
}