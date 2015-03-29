#pragma once

#include "ElModule.h"

class Test : public ElModule
{
public:
	Test();
	~Test();

	std::string Info();
	void Tick(const std::chrono::duration<double>& Delta);
	void Run(const std::vector<std::string>& Args);
private:
};