#pragma once

#include "ElModule.h"

class Spawn : public ElModule
{
public:
	Spawn();
	~Spawn();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info() const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

private:
};