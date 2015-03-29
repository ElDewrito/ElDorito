#pragma once
#include <chrono>
#include <vector>
#include <string>

class ElModule
{
public:
	virtual std::string Info() = 0;
	virtual std::string Usage() = 0;
	virtual void Tick(const std::chrono::duration<double>& DeltaTime) = 0;
	virtual bool Run(const std::vector<std::string>& Args) = 0;
	virtual ~ElModule() = default;
private:
};