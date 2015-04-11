#pragma once

#include "ElModule.h"
#include <inttypes.h>

#include "../Patch.h"

class NameChange : public ElModule
{
public:
	NameChange();
	~NameChange();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info(const std::string& Topic = "") const;

	// Suggest auto-complete strings for arugments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

	wchar_t UserName[17];
private:
	void GetSavedUsername();
};