#pragma once
#include <chrono>
#include <vector>
#include <string>

class ElModule
{
public:
	ElModule()
	{
	}
	~ElModule()
	{
	}
	virtual std::string Info() = 0;
	virtual void Tick(const std::chrono::duration<double>& DeltaTime) = 0;
	virtual void Run(const std::vector<std::string>& Args) = 0;
private:
};