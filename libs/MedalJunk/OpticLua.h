#pragma once

#include "OpticException.h"
#include "Events.h"
#include <string>
#include <memory>

struct lua_State;

namespace Optic {

class OpticPack;
class OpticEventHandler;

class OpticLuaException : public OpticException {
public:
	OpticLuaException() : OpticException("An unknown exception occured!") { }
	OpticLuaException(std::string msg) : OpticException(msg) { };
};

class OpticLua {
	class LuaGlobal {
		lua_State *lstate;
	public:
		LuaGlobal(lua_State* state, const std::string& global);
		~LuaGlobal();
	};
	lua_State *luaState;
	void createEventTable();
	void registerFunctions();

public:
	std::string getVersion();
	OpticLua(std::string lua);
	void call(const std::string& function, std::shared_ptr<Event> event);
	void registerCallbacks(OpticEventHandler* handler);
	void registerGlobal(const std::string& key, void* global);
	~OpticLua();
};

};