#pragma once

#include "Queue.hpp"

class Queue;

class ConsoleQueue : public Queue
{
public:
	ConsoleQueue();

	void pushLineFromGameToUIMultipleLines(std::string multipleLines);
	virtual void pushLineFromKeyboardToGame(std::string line);
};

