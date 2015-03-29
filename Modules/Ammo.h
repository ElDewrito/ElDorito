#pragma once

#include "ElModule.h"
#include <inttypes.h>

class Ammo : public ElModule
{
public:
	Ammo();
	~Ammo();

	std::string Info();
	std::string Usage();

	void Tick(const std::chrono::duration<double>& Delta);
	bool Run(const std::vector<std::string>& Args);
private:
	bool enabled;
};