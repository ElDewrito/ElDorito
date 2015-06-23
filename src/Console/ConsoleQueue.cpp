#include "ConsoleQueue.hpp"
#include "../CommandMap.hpp"
#include <sstream>
#include "DirectXHook.hpp"

ConsoleQueue::ConsoleQueue() : Queue(
#ifdef _DEBUG
	DirectXHook::COLOR_GREEN
#else
	DirectXHook::COLOR_YELLOW
#endif
	)
{
}

void ConsoleQueue::pushLineFromKeyboardToGame(std::string line)
{
	pushLineFromGameToUI(line);
	pushLineFromGameToUIMultipleLines(Modules::CommandMap::Instance().ExecuteCommand(line));
}

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems)
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