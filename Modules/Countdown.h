#pragma once

#include "ElModule.h"

class Countdown : public ElModule
{
public:
	Countdown();
	~Countdown();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info(const std::string& Topic = "") const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

	// Called when the preferences file is edited externally
	void PreferencesChanged();
private:
};