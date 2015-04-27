#include "EventSubscriber.h"
#include <iterator>

EventSubscriber::EventSubscriber() {
	InitializeCriticalSection(&handlerLock);
	eventSem = CreateSemaphore(NULL, 0, 100, NULL);
}

void EventSubscriber::enqueue(const std::shared_ptr<Event>& event) const {
	events.push(event);
	ReleaseSemaphore(eventSem, 1, NULL);
}

unsigned int WINAPI EventSubscriber::start(void* obj) {
	EventSubscriber *subscriber = static_cast<EventSubscriber*>(obj);
	return subscriber->run(); 
}

void EventSubscriber::broadcast(const std::shared_ptr<Event>& event) const {
	EnterCriticalSection(&handlerLock);
	std::vector<const EventSubscriber*> *subs = &handlers[event.get()->type];

	for(auto it = begin(*subs); it != end(*subs); it++) {
		(*it)->enqueue(event);
	}

	LeaveCriticalSection(&handlerLock);
}

void EventSubscriber::subscribe(const EventSubscriber* sub, EVENTS type) const {
	EnterCriticalSection(&handlerLock);
	handlers[type].push_back(sub);
	LeaveCriticalSection(&handlerLock);
}

void EventSubscriber::subscribe(const EventSubscriber* sub, const std::vector<EVENTS>& events) const {
	EnterCriticalSection(&handlerLock);

	for(auto it = begin(events); it != end(events); it++) {
		handlers[*it].push_back(sub);
	}

	LeaveCriticalSection(&handlerLock);
}

void EventSubscriber::unsubscribe(const EventSubscriber* sub, EVENTS type) const {
	EnterCriticalSection(&handlerLock);

	for(auto it = handlers[type].begin(); it != handlers[type].end(); ++it) {
		if(*it == sub) {
			handlers[type].erase(it);
			break;
		}
	}

	LeaveCriticalSection(&handlerLock);
}

void EventSubscriber::unsubscribe(const EventSubscriber* sub) const {
	EnterCriticalSection(&handlerLock);

	for(int i = 0; i < LAST; ++i) {
		for(auto it = handlers[i].begin(); it != handlers[i].end(); ++it) {
			if(*it == sub) {
				handlers[i].erase(it);
				break;
			}
		}
	}

	LeaveCriticalSection(&handlerLock);
}