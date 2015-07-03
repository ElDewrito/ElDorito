#pragma once

#include "OpticLua.h"
#include "EventSubscriber.h"
#include <vector>
#include <WinDef.h>
#include <map>
#include <memory>

namespace Optic {

class OpticPack;
class OpticRender;

class OpticEventHandler : public EventSubscriber {
	DWORD run();
	void processQueue();
	bool exit;
	EventSubscriber* dispatcher;
	std::unique_ptr<OpticPack> pack;
	OpticLua oLua;
	std::map<EVENTS, std::string> callbacks;
	std::unique_ptr<OpticRender> renderer;
	HANDLE exitSem;

public:
	void render();
	void reset();
	void lost();
	void wait();
	OpticEventHandler(std::unique_ptr<OpticPack> pack, EventSubscriber* dispatcher);
	~OpticEventHandler();
	void registerDispatcher(EventSubscriber* dispatcher);
	bool registerLuaCallback(EVENTS type, const std::string& callback);
};

};