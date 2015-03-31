#pragma once

#include "ElModule.h"

class Spawn : public ElModule
{
public:
	Spawn();
	~Spawn();

	std::string Info();

	void Tick(const std::chrono::duration<double>& Delta);
	bool Run(const std::vector<std::string>& Args);
private:
};