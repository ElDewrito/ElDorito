#pragma once

#include "Queue.hpp"

class Queue;

class GameChatQueue : public Queue
{
public:
	GameChatQueue();
	virtual void pushLineFromKeyboardToGame(std::string line);
};