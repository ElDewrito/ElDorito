#pragma once

#include "ElModule.h"
#include <inttypes.h>

#include "../Patch.h"

class ThirdPerson : public ElModule
{
public:
	ThirdPerson();
	~ThirdPerson();

	std::string Info();

	void Tick(const std::chrono::duration<double>& Delta);
	bool Run(const std::vector<std::string>& Args);
private:
	bool enabled;

	Patch ThirdPatchX;
	Patch ThirdPatchY;
	Patch ThirdPatchZ;
	Patch ThirdPatchXX;
	Patch ThirdPatchYY;
	Patch ThirdPatchZZ;
	Patch ThirdPatchXXX;
	Patch ThirdPatchYYY;
	Patch ThirdPatchZZZ;
};