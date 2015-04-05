#pragma once

#include "ElModule.h"
#include <inttypes.h>

#include "../Patch.h"

class ThirdPerson : public ElModule
{
public:
	ThirdPerson();
	~ThirdPerson();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info(const std::string& Topic = "") const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

private:
	bool enabled;

	Patch ThirdPatchX;
	Patch ThirdPatchY;
	Patch ThirdPatchZ;
	Patch ThirdPatchXX;
	Patch ThirdPatchYY;
	Patch ThirdPatchZZ;
	Patch ThirdPatchXXX;
	Patch ThirdPatchZZZ;
};