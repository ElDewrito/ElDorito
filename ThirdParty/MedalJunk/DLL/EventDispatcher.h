#pragma once

#include "EventSubscriber.h"
#include <vector>
#include <WinDef.h>

class EventDispatcher : public EventSubscriber {
	DWORD run();
	void processQueue();
	bool exit;

public:
	void registerSubscribers(std::vector<EventSubscriber*>& modules);
};