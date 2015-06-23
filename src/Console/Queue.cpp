#include "Queue.h"
#include "GameConsole.h"

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
