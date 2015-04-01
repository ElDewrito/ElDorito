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

	std::string Info();

	void Tick(const	std::chrono::duration<double>& dt);
	bool Run(const CommandLineArgs& args);
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