#pragma once

#include "Queue.hpp"

class Queue;

class GlobalChatQueue : public Queue
{
public:
	std::string sendThisLineToIRCServer = "";

	GlobalChatQueue();
	virtual void pushLineFromKeyboardToGame(std::string line);
};