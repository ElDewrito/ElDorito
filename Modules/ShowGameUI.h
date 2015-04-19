#pragma once

#include "ElModule.h"

class ShowGameUI : public ElModule
{
public:
	ShowGameUI();
	~ShowGameUI();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info(const std::string& Topic = "") const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

	bool DialogShow; // todo: put this somewhere better
	unsigned int DialogStringId;
	int DialogArg1; // todo: figure out a better name for this
	int DialogFlags;
	int DialogParentStringId;
private:
};