#include "ElDoritoConfig.h"

#include <chrono>
#include <map>
#include <vector>
#include <stdint.h>

#include "Singleton.h"
#include "Modules/ElModule.h"
#include "Pointer.h"

class ElDorito : public Singleton < ElDorito >
{
public:
	ElDorito();
	~ElDorito();

	std::string GetDirectory();

	void Tick(const std::chrono::duration<double> DeltaTile);

private:
	//Console
	void PrintConsole();
	std::map<std::string, ElModule*> Commands;
	std::vector<std::string> PrevCommands;
	std::vector<std::string>::iterator PrevCommand;
	std::string Command;
	std::string PrevSuggestion;
};
