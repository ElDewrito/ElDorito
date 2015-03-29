#pragma once

#include "ElModule.h"
#include <inttypes.h>

class Godmode : public ElModule
{
public:
	Godmode();
	~Godmode();

	std::string Info();
	std::string Usage();

	void Tick(const std::chrono::duration<double>& Delta);
	bool Run(const std::vector<std::string>& Args);
private:
	bool enabled;
};