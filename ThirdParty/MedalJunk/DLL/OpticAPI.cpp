#include "OpticAPI.h"
#include "OpticLua.h"
#include "OpticPack.h"
#include "OpticRender.h"
#include "OpticAnimation.h"
#include "EngineTypes.h"
#include "EnginePointers.h"
#include "OpticEventHandler.h"
#include "OpticHooks.h"
#include "DebugHelper.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <sstream>

#pragma warning(disable : 4800)

namespace Optic { namespace API {

int attach_sprite(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 2) {
		luaL_error(state, "Invalid number of arguments");
	}

	lua_getglobal(state, "renderer");
	OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -1));
	lua_pop(state, 1);

	if(!lua_isstring(state, 1)) {
		return luaL_argerror(state, 1, "medal name expected");
	}

	if(!lua_isstring(state, 2)) {
		return luaL_argerror(state, 2, "sprite name expected");
	}

	try {
		render->attachSprite(lua_tostring(state, 1), lua_tostring(state, 2));
	} catch(std::runtime_error& e) {
		return luaL_error(state, e.what());
	}

	return 0;
}

int create_sprite(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 2 && args != 3) {
		luaL_error(state, "Invalid number of arguments");
	}

	if(!lua_isstring(state, 1)) {
		return luaL_argerror(state, 1, "sprite name expected");
	}

	if(!lua_isstring(state, 2)) {
		return luaL_argerror(state, 2, "image name expected");
	}

	if(args == 3 && !lua_isstring(state, 3)) {
		return luaL_argerror(state, 3, "animation name expected");
	}

	lua_getglobal(state, "renderer");
	lua_getglobal(state, "resources");
	OpticPack* pack = static_cast<OpticPack*>(lua_touserdata(state, -1));
	OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -2));
	lua_pop(state, 2);

	try {
		OpticImage image = pack->fetchImage(lua_tostring(state, 2));
		if(args == 3) {
			render->createSprite(lua_tostring(state, 1), image, lua_tostring(state, 3));
		} else {
			render->createSprite(lua_tostring(state, 1), image);
		}
	} catch(OpticException& e) {
		return luaL_error(state, e.what());
	}

	return 0;
}

int create_animation(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 1) {
		luaL_error(state, "Invalid number of arguments");
	}

	if(!lua_isstring(state, 1)) {
		return luaL_argerror(state, 1, "animation name expected");
	}

	lua_getglobal(state, "renderer");
	OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -1));
	lua_pop(state, 1);

	try {
		render->createAnimation(lua_tostring(state, 1));
	} catch(OpticException& e) {
		return luaL_error(state, e.what());
	}

	return 0;
}

int create_medal(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 1 && args != 2) {
		luaL_error(state, "Invalid number of arguments");
	}

	if(!lua_isstring(state, 1)) {
		return luaL_argerror(state, 1, "medal name expected");
	}

	if(args == 2 && !lua_isstring(state, 2)) {
		return luaL_argerror(state, 2, "animation name expected");
	}

	lua_getglobal(state, "renderer");
	OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -1));
	lua_pop(state, 1);

	try {
		if(args == 1) {
			render->createMedal(lua_tostring(state, 1));
		} else {
			render->createMedal(lua_tostring(state, 1), lua_tostring(state, 2));
		}
	} catch(OpticException& e) {
		return luaL_error(state, e.what());
	}

	return 0;
}

int sprite_properties(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 7) {
		luaL_error(state, "Invalid number of arguments");
	}

	if(!lua_isstring(state, 1)) {
		return luaL_argerror(state, 1, "sprite name expected");
	}

	if(!lua_isboolean(state, 2)) {
		return luaL_argerror(state, 2, "boolean expected");
	}

	for(int i = 3; i != 7; i++) {
		if(!lua_isnumber(state, i)) {
			return luaL_argerror(state, i, "number expected");
		}
	}

	lua_getglobal(state, "renderer");
	OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -1));
	lua_pop(state, 1);

	try {
		render->setSpriteProperties(lua_tostring(state, 1), lua_toboolean(state, 2), lua_tointeger(state, 3),
									lua_tointeger(state, 4),lua_tointeger(state, 5), lua_tointeger(state, 6),
									lua_tointeger(state, 7));
	} catch(OpticException& e) {
		return luaL_error(state, e.what());
	}

	return 0;
}

int add_keyframe(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 4 && args != 5 && args != 8) {
		luaL_error(state, "Invalid number of arguments for adding keyframes");
	}

	if(!lua_isstring(state, 1)) {
		return luaL_argerror(state, 1, "animation name expected");
	}

	if(!lua_isnumber(state, 2)) {
		return luaL_argerror(state, 2, "time value expected");
	}

	if(!lua_isnumber(state, 3)) {
		return luaL_argerror(state, 3, "transition state expected");
	}

	if(!lua_isnumber(state, 4)) {
		return luaL_argerror(state, 4, "attribute expected");
	}

	if(args == 5 && !lua_isnumber(state, 5)) {
		return luaL_argerror(state, 5, "tween type expected");
	}

	if(args == 8) {
		for(int i = 5; i != 8; i++) {
			if(!lua_isnumber(state, i)) {
				return luaL_argerror(state, i, "control point coordinate expected");
			}
		}
	}

	lua_getglobal(state, "renderer");
	OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -1));
	lua_pop(state, 1);

	try {
		if(args == 4) {
			render->addKeyframe(lua_tostring(state, 1), Keyframe(lua_tointeger(state, 2), lua_tonumber(state, 3)),
								TRANSFORM(lua_tointeger(state, 4)), LINEAR);	
		} else if(args == 5) {
			render->addKeyframe(lua_tostring(state, 1), Keyframe(lua_tointeger(state, 2), lua_tonumber(state, 3)),
								TRANSFORM(lua_tointeger(state, 4)), TWEEN(lua_tointeger(state, 5)));
		} else {
			OpticBezier bezier(lua_tonumber(state, 5), lua_tonumber(state, 6), lua_tonumber(state, 7), lua_tonumber(state, 8));
			render->addKeyframe(lua_tostring(state, 1), Keyframe(lua_tointeger(state, 2), lua_tonumber(state, 3)),
								TRANSFORM(lua_tointeger(state, 4)), bezier);
		}
	} catch(OpticException& e) {
		luaL_error(state, e.what());
	}

	return 0;
}


int register_callback(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 2) {
		luaL_error(state, "Invalid number of arguments for registering callback");
	}
	
	lua_getglobal(state, "handler");
	OpticEventHandler* handler = static_cast<OpticEventHandler*>(lua_touserdata(state, -1));
	EVENTS event = static_cast<EVENTS>(lua_tointeger(state, 1));
	
	if(!handler->registerLuaCallback(event, lua_tostring(state, 2))) {
		luaL_error(state, "Invalid callback event ID");
	}

	return 0;
}

int medal_properties(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 5) {
		luaL_error(state, "Invalid number of arguments for medal properties");
	}

	if(!lua_isstring(state, 1)) {
		luaL_argerror(state, 1, "Medal name was expected");
	}

	if(!lua_isnumber(state, 2)) {
		luaL_argerror(state, 2, "Group number was expected");
	}

	if(!lua_isnumber(state, 3)) {
		luaL_argerror(state, 3, "Priority value was expected");
	}

	if(!lua_isboolean(state, 4)) {
		luaL_argerror(state, 4, "Immunity value expected");
	}

	if(!lua_isnumber(state, 5)) {
		luaL_argerror(state, 5, "Slide reverse animation time expected");
	}

	try {
		lua_getglobal(state, "renderer");
		OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -2));
		lua_pop(state, 1);

		render->setMedalProperties(lua_tostring(state, 1), lua_tointeger(state, 2), lua_tointeger(state, 3),
								   lua_toboolean(state, 4), lua_tointeger(state, 5));
	} catch(OpticException& e) {
		luaL_error(state, e.what());
	}

	return 0;
}

int medal_slide_animation(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 2) {
		luaL_error(state, "Invalid number of arguments for medal animation");
	}

	if(!lua_isstring(state, 1)) {
		luaL_argerror(state, 1, "Medal name was expected");
	}

	if(!lua_isstring(state, 2)) {
		luaL_argerror(state, 2, "Animation name was expected");
	}

	try {
		lua_getglobal(state, "renderer");
		OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -1));
		lua_pop(state, 1);

		render->setSlideAnimation(lua_tostring(state, 1), lua_tostring(state, 2));
	} catch(OpticException& e) {
		luaL_error(state, e.what());
	}

	return 0;
}

int display_medal(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 1 && args != 2) {
		luaL_error(state, "Invalid number of arguments for medal display");
	}

	if(!lua_isstring(state, 1)) {
		luaL_argerror(state, 2, "Either a medal name or image name was expected");
	}

	if(args == 2 && !lua_isstring(state, 2)) {
		luaL_argerror(state, 4, "An animation name was expected");
	}

	try {
		lua_getglobal(state, "renderer");
		lua_getglobal(state, "resources");
		
		OpticPack* pack = static_cast<OpticPack*>(lua_touserdata(state, -1));
		OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -2));
		lua_pop(state, 2);

		if(args == 2) {
			render->renderMedal(pack->fetchImage(lua_tostring(state, 1)), lua_tostring(state, 2));
		} else {
			std::string name = lua_tostring(state, 1);
			if(render->medalExists(name)) {
				render->renderMedal(name);
			} else {
				
				render->renderMedal(pack->fetchImage(name));
			}
		}
	} catch(std::exception& e) {
		luaL_error(state, e.what());
	}

	return 0;
}

int queue_audio(lua_State* state) {
	int args = lua_gettop(state);

	if(args != 1 && args != 2) {
		luaL_error(state, "Invalid number of arguments for queue audio");
	}

	if(!lua_isstring(state, 1)) {
		luaL_argerror(state, 1, "Audio filename was expected");
	}

	if(args == 2 && !lua_isboolean(state, 2)) {
		luaL_argerror(state, 2, "Boolean indicating priority expected");
	}

	try {
		lua_getglobal(state, "renderer");
		lua_getglobal(state, "resources");
		
		OpticPack* pack = static_cast<OpticPack*>(lua_touserdata(state, -1));
		OpticRender* render = static_cast<OpticRender*>(lua_touserdata(state, -2));
		lua_pop(state, 2);

		if(args == 1) {
			render->queueAudio(pack->fetchAudio(lua_tostring(state, 1)));
		} else {
			render->queueAudio(pack->fetchAudio(lua_tostring(state, 1)), lua_toboolean(state, 2));
		}
	} catch(OpticException& e) {
		luaL_error(state, e.what());
	}

	return 0;
}

void registerFunctions(lua_State* state) {
	lua_register(state, "register_callback", register_callback);
	lua_register(state, "display_medal", display_medal);
	lua_register(state, "attach_sprite", attach_sprite);
	lua_register(state, "create_sprite", create_sprite);
	lua_register(state, "create_medal", create_medal);
	lua_register(state, "create_animation", create_animation);
	lua_register(state, "sprite_properties", sprite_properties);
	lua_register(state, "add_keyframe", add_keyframe);
	lua_register(state, "medal_properties", medal_properties);
	lua_register(state, "medal_slide_animation", medal_slide_animation);
	lua_register(state, "queue_audio", queue_audio);
}

}}