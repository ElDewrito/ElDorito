#pragma once

#include "ElModule.h"
#include <inttypes.h>

class Godmode : public ElModule
{
public:
	Godmode();
	~Godmode();

	bool enabled; 
	uint8_t reset[8];
	uint8_t god[8];

	std::string Info();
	void Tick(const std::chrono::duration<double>& Delta);
	void Run(const std::vector<std::string>& Args);
private:
};