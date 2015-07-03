#include "EventDispatcher.h"

DWORD EventDispatcher::run() {
	exit = false;

	while(!exit) {
		WaitForSingleObject(eventSem, INFINITE);
		processQueue();
	}

	return 0;
}

void EventDispatcher::processQueue() {
	std::shared_ptr<Event> event;

	while(!events.empty()) {
		if(events.try_pop(event)) {
			broadcast(event);
		}

		switch(event->type) {
			case THREAD_EXIT:
				exit = true;
				break;
		}
	}
}

//todo remove this
void EventDispatcher::registerSubscribers(std::vector<EventSubscriber*>& modules) {
	//Subscribe to every possible event
	for(size_t i = 0; i < modules.size(); i++) {
		if(modules[i] != this) {
			for(int j = 0; j < LAST; j++) {
				modules[i]->subscribe(this, (EVENTS)j);
			}
		}
	}
}