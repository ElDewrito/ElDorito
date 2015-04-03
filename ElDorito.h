#pragma once
#include "ElDoritoConfig.h"

#include <chrono>
#include <map>
#include <vector>
#include <stdint.h>
#include <memory>

#include "Console\Console.h"
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

	template<class T,
	class = typename std::enable_if<std::is_base_of<ElModule, T>::value>::type>
		void PushModule(const std::string& Name)
	{
		std::shared_ptr<T> Module = std::make_shared<T>();
		Terminal.PushCommand(Name, Module);
		Commands[Name] = Module;
	}

private:
	// Thread
	static size_t MainThreadID;

	// Console
	Console::Console Terminal;
	std::map<std::string, std::shared_ptr<ElModule>> Commands;
};
