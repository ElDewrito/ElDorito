#pragma once

#include <string>

namespace Optic {

void list();
void load(const std::string& name);
void render();
void unload();
void lost();
void reset();

};