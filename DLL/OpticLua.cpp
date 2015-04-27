#include "OpticLua.h"
#include "OpticAPI.h"
#include "Events.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <Windows.h>
#include "EnginePointers.h"
#include "EngineTypes.h"
#include <iostream>
#define LUA_ENUM(L, name, val) \
  lua_pushlstring(L, #name, sizeof(#name)-1); \
  lua_pushnumber(L, val); \
  lua_settable(L, -3);

namespace Optic {

OpticLua::LuaGlobal::LuaGlobal(lua_State* state, const std::string& global) : lstate(state) {
	lua_getglobal(state, global.c_str());
}

OpticLua::LuaGlobal::~LuaGlobal() {
	lua_pop(lstate, 1);
}

void OpticLua::registerFunctions() {
	API::registerFunctions(luaState);
}

OpticLua::OpticLua(std::string lua) {
	luaState = luaL_newstate();
    luaL_openlibs(luaState);

	if(luaL_dostring(luaState, lua.c_str()) == 1) {
		std::string error = lua_tostring(luaState, -1);
		lua_pop(luaState, 1);
		throw OpticLuaException(error);
	}

	registerFunctions();
}

void OpticLua::call(const std::string& function, std::shared_ptr<Event> event) {
	int numargs = 0;
	lua_getglobal(luaState, function.c_str());

	if(event->type == H3_GAME_EVENT) {
		GameEvent* levent = reinterpret_cast<GameEvent*>(event.get());
		lua_pushnumber(luaState, levent->event_id);
		numargs = 1;
	}

	if(lua_pcall(luaState, numargs, 0, 0) != 0) {
		std::string error = lua_tostring(luaState, -1);
		lua_pop(luaState, 1);
		throw OpticLuaException(error);
	}
}

std::string OpticLua::getVersion() {
	LuaGlobal global(luaState, "api_version");
	
	if(lua_isstring(luaState, 1)) {
		return std::string(lua_tostring(luaState, -1));
	} else {
		throw OpticLuaException("api_version not defined!");
	}
}

OpticLua::~OpticLua() {
	lua_close(luaState);
}

void OpticLua::registerGlobal(const std::string& key, void* global) {
	lua_pushlightuserdata(luaState, global);
	lua_setglobal(luaState, key.c_str());
}

void OpticLua::registerCallbacks(OpticEventHandler* handler) {
	createEventTable();
	lua_pushlightuserdata(luaState, handler);
	lua_setglobal(luaState, "handler");
	lua_getglobal(luaState, "register_callbacks");
	if(lua_pcall(luaState, 0, 0, 0) != 0) {
		std::string error = lua_tostring(luaState, -1);
		lua_pop(luaState, 1);
		throw OpticLuaException(error);
	}
}

void OpticLua::createEventTable() {
	lua_newtable(luaState);
	int i = 1;
	LUA_ENUM(luaState, H3_GAME_EVENT, i++);
	lua_setglobal(luaState, "cb");
}

};