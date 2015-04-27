#include "OpticEventHandler.h"
#include "OpticPack.h"
#include "OpticLua.h"
#include "OpticAPI.h"
#include "Direct3D.h"
#include "OpticRender.h"
#include "OpticMisc.h"
#include "Shared.h"
#include "DebugHelper.h"
#include <sstream>

namespace Optic {

DWORD OpticEventHandler::run() {
	while(!exit) {
		WaitForSingleObject(eventSem, INFINITE);
		processQueue();
	}

	ReleaseSemaphore(exitSem, 1, NULL);
	return 0;
}

void OpticEventHandler::processQueue() {
	std::shared_ptr<Event> event;

	while(!events.empty()) {
		if(!events.try_pop(event)) {
			return;
		}

		switch(event->type) {
			case THREAD_EXIT:
				exit = true;
				continue;
		}

		try {
			oLua.call(callbacks[event->type], event);
		} catch(OpticLuaException& e) {
			std::cout << "Exception: " << e.what() << std::endl;
			exit = true;
		}
	}
}

void OpticEventHandler::registerDispatcher(EventSubscriber* dispatcher) {
	this->dispatcher = dispatcher;
}

OpticEventHandler::OpticEventHandler(std::unique_ptr<OpticPack> pack, EventSubscriber* dispatcher)
	: oLua(pack->getLua()), dispatcher(dispatcher), renderer(new OpticRender()) {
	dispatcher->subscribe(this, THREAD_EXIT);
	exitSem = CreateSemaphore(NULL, 0, 1, NULL);
	exit = false;
	this->pack = std::move(pack);

	Version apiVersion = {1, 0, 0};
	std::string scriptVersion = oLua.getVersion();
	int status = versionCheck(scriptVersion, apiVersion);

	if(status != VERSION_RELATION::TENTATIVE_OKAY && status != VERSION_RELATION::OKAY) {
		throw OpticLuaException("Script is out of date!");
	}

	oLua.registerGlobal("resources", this->pack.get());
	oLua.registerGlobal("renderer", renderer.get());
	oLua.registerCallbacks(this);

	/* Preloading textures from a function called by Halo's rendering thread ensures
	   we won't deadlock D3D later on */
	for(auto i = this->pack->images_begin(); i != this->pack->images_end(); ++i) {
		renderer->createTexture(i->second);
	}
}

bool OpticEventHandler::registerLuaCallback(EVENTS type, const std::string& callback) {
	if(type > LUA_API_SAFE_BEGIN && type < LUA_API_SAFE_END) {
		dispatcher->subscribe(this, type);
		callbacks[type] = callback;
		return true;
	}
	
	return false;
}

void OpticEventHandler::wait() {
	WaitForSingleObject(exitSem, INFINITE);
}

OpticEventHandler::~OpticEventHandler() {
	dispatcher->unsubscribe(this);
	CloseHandle(exitSem);
}

void OpticEventHandler::render() {
	renderer->draw();
}

void OpticEventHandler::reset() {
	renderer->reset();
}

void OpticEventHandler::lost() {
	renderer->lost();
}

};