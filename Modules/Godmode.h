#pragma once

#include "ElModule.h"

#include "../Patch.h"

class Godmode : public ElModule
{
public:
	Godmode();
	~Godmode();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info(const std::string& Topic = "") const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;
private:
	bool enabled;
	Patch GodPatch;
};