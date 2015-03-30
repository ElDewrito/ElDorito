#include "ElDoritoConfig.h"

#include <chrono>
#include <map>
#include <vector>
#include <stdint.h>
#include <memory>

#include "Singleton.h"
#include "Modules/ElModule.h"
#include "Pointer.h"

class ElDorito : public Singleton < ElDorito >
{
public:
	ElDorito();
	~ElDorito() = default;

	std::string GetDirectory();

	void Tick(const std::chrono::duration<double>& DeltaTile);

	void SetSessionMessage(const std::string& Message);

private:
	//Console
	void PrintConsole();
	std::map<std::string, std::unique_ptr<ElModule>> Commands;
	std::vector<std::string> PrevCommands;
	std::vector<std::string>::iterator PrevCommand;
	std::string Command;
	std::string PrevSuggestion;
};
