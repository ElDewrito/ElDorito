#pragma once
#include "ElDoritoConfig.h"

#include <chrono>
#include <map>
#include <vector>
#include <stdint.h>
#include <memory>

#include "Utils/Utils.h"
#include "ElModules.h"
#include "Pointer.h"
#include "Patch.h"

class ElDorito : public Utils::Singleton < ElDorito >
{
public:
	ElDorito();
	~ElDorito() = default;

	std::string GetDirectory();

	void Initialize();
	void Tick(const std::chrono::duration<double>& DeltaTile);

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
	static size_t MainThreadID; // Thread
	bool d3d9Loaded = false;
	bool consoleLoaded = false;
	bool windowTitleSet = false;
	DWORD d3d9InitTime = 0;

	void setWindowTitle(const std::string& Message);
};
