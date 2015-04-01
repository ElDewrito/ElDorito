#pragma once
#include "ElDoritoConfig.h"

#include <chrono>
#include <map>
#include <vector>
#include <stdint.h>
#include <memory>

#include "Utils\Singleton.h"
#include "Modules/ElModule.h"
#include "Pointer.h"
#include "Patch.h"

class ElDorito : public Utils::Singleton < ElDorito >
{
public:
	ElDorito();
	~ElDorito() = default;

	std::string GetDirectory();

	void Tick(const std::chrono::duration<double>& DeltaTile);

	void SetSessionMessage(const std::string& Message);

	static void SetMainThreadID(size_t ThreadID)
	{
		MainThreadID = ThreadID;
	}
	static size_t GetMainThreadID()
	{
		return MainThreadID;
	}

	static Pointer GetMainTls(size_t Offset = 0);

private:

	// Thread
	static size_t MainThreadID;

	//Console
	void PrintConsole();
	std::map<std::string, std::unique_ptr<ElModule>> Commands;
	std::vector<std::string> PrevCommands;
	std::vector<std::string>::iterator PrevCommand;
	std::string Command;
	std::string PrevSuggestion;
};
