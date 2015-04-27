#pragma once

#include "IHook.h"
#include <exception>
#include <stdexcept>
#include <vector>

namespace HookManager {

typedef void (*LAUNCHCB)(void*);

void install();
void uninstall();
void launchCallback(LAUNCHCB callback, void* data = NULL);
void quitCallback(LAUNCHCB callback, void* data = NULL);
void post_launch();

};