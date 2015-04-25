#pragma once

#include "ElModule.h"
#include <inttypes.h>

#include "../Patch.h"

enum DebugLoggingModes
{
	Network = 1,
	SSL = 2,
	UI = 4,
	Game1 = 8,
	Game2 = 16,
};

class DebugLogging : public ElModule
{
public:
	DebugLogging();
	~DebugLogging();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info(const std::string& Topic = "") const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;

	bool SetMode(std::string mode);
	std::vector<std::string> FiltersExclude;
	std::vector<std::string> FiltersInclude;
	int EnabledFlags;
private:
};