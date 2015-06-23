#pragma once

#include "Queue.hpp"

class Queue;

class GlobalChatQueue : public Queue
{
public:
	GlobalChatQueue();
	virtual void pushLineFromKeyboardToGame(std::string line);
};