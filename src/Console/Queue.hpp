#pragma once
#include <string>
#include <vector>
#include <d3dx9core.h>

class Queue
{
public:
	const int numOfLinesBuffer = 100;
	const int numOfLinesToShow = 12;
	int startIndexForUI = 0;
	D3DCOLOR color;
	std::vector<std::string> queue = std::vector<std::string>{}; // index 0 is newest command; the higher the index, the older the command
	std::string sendThisLineToIRCServer = "";

	Queue(D3DCOLOR color);
	void pushLineFromGameToUI(std::string line);
	virtual void pushLineFromKeyboardToGame(std::string line) = 0;
};