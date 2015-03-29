#pragma once

#include "ElModule.h"
#include <inttypes.h>

class Ammo : public ElModule
{
public:
	Ammo();
	~Ammo();

	std::string Info();
	void Tick(const std::chrono::duration<double>& Delta);
	void Run(const std::vector<std::string>& Args);
private:
	bool enabled;
};