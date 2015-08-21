#pragma once
#include <string>
#include <vector>
#include <d3dx9core.h>

class Queue
{
public:
	std::vector<std::string> unchangingBacklog = std::vector <std::string> {};
	const int numOfLinesBuffer = 100;
	const int numOfLinesToShow = 12;
	int startIndexForScrolling = 0;
	int lastTimeQueueShown = 0;
	bool visible = true;
	D3DCOLOR color;
	std::vector<std::string> queue = std::vector<std::string>{}; // index 0 is newest command; the higher the index, the older the command

	Queue(D3DCOLOR color);
	void pushLineFromGameToUI(std::string line);
	virtual void pushLineFromKeyboardToGame(std::string line) = 0;
};

class GlobalChatQueue : public Queue
{
public:
	GlobalChatQueue();
	virtual void pushLineFromKeyboardToGame(std::string line);
};

class GameChatQueue : public Queue
{
public:
	GameChatQueue();
	virtual void pushLineFromKeyboardToGame(std::string line);
};

class ConsoleQueue : public Queue
{
public:
	ConsoleQueue();
	std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
	void pushLineFromGameToUIMultipleLines(std::string multipleLines);
	virtual void pushLineFromKeyboardToGame(std::string line);
};