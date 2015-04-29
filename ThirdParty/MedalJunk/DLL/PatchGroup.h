#pragma once

#include <vector>

class IHook;

class PatchGroup {
public:
	void add(IHook* hook);
	bool install();
	void uninstall();
	~PatchGroup();

private:
	std::vector<IHook*> hooks;
};