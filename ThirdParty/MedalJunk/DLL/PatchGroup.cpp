#include "PatchGroup.h"
#include "IHook.h"

void PatchGroup::add(IHook* hook) {
	hooks.emplace_back(hook);
}

bool PatchGroup::install() {
	for(auto i = hooks.begin(); i != hooks.end(); ++i) {
		if(!(*i)->install()) {
			return false;
		}
	}
	return true;
}

void PatchGroup::uninstall() {
	for(std::vector<int>::size_type i = 0; i != hooks.size(); ++i) {
		delete hooks[i];
	}
}

PatchGroup::~PatchGroup() {
	uninstall();
}