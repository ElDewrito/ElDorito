#pragma once
#include <chrono>
#include <vector>
#include <string>

#include "../Console/Console.h"

class ElModule : public Console::Command
{
public:
	virtual void Tick(const std::chrono::duration<double>& DeltaTime) = 0;
	virtual ~ElModule() = default;
private:
};