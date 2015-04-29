#pragma once

#include <string>

struct lua_State;

namespace Optic { namespace API {

const std::string API_VERSION = "1.0.0";

void registerFunctions(lua_State* state);

}};