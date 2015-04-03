#pragma	once

#include"ElModule.h"

// STL
#include <string>
#include <unordered_map>
#include <vector>
#include <functional> // std::function

class Globals : public ElModule
{
	typedef std::vector<std::string> CommandLineArgs;
public:
	Globals();
	~Globals();

	void Tick(const std::chrono::duration<double>& Delta);

	// Return true on success
	bool Run(const std::vector<std::string>& Arguments);

	// Command and usage info
	std::string Info() const;

	// Suggest auto-complete strings for arguments
	std::string Suggest(const std::vector<std::string>& Arguments) const;
private:
	struct GlobalCommand
	{
		std::function<bool(const CommandLineArgs& args)> Run;
		std::function<std::string(void)> Info;
	};

	std::unordered_map<std::string, GlobalCommand> commands;

	// Setup methods
	void setupGraphicsGlobals();
	void setupPhysicsGlobals();
	void setupMiscGlobals();
};