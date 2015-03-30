#pragma once

#include "ElModule.h"

class Hud : public ElModule
{
public:
	Hud();
	~Hud();

	std::string Info();

	void Tick(const std::chrono::duration<double>& Delta);
	bool Run(const std::vector<std::string>& Args);
private:
	bool enabled;
};