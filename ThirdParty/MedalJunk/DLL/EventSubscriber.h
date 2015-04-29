#pragma once

#include "events.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <concurrent_queue.h>
#include <vector>
#include <memory>

class EventSubscriber {	
	mutable std::vector<const EventSubscriber*> handlers[LAST];
	mutable CRITICAL_SECTION handlerLock;

	protected:
	mutable Concurrency::concurrent_queue<std::shared_ptr<Event>> events;
	HANDLE eventSem;
	virtual DWORD run() = 0;
	virtual void processQueue() = 0;
	void broadcast(const std::shared_ptr<Event>& event) const;

	public:
	EventSubscriber();
	virtual ~EventSubscriber() { DeleteCriticalSection(&handlerLock); CloseHandle(eventSem); };
	virtual void enqueue(const std::shared_ptr<Event>& event) const;
	static unsigned int WINAPI start(void* obj);
	void subscribe(const EventSubscriber* sub, EVENTS type) const;
	void subscribe(const EventSubscriber* sub, const std::vector<EVENTS>& events) const;
	void unsubscribe(const EventSubscriber* sub, EVENTS type) const;
	void unsubscribe(const EventSubscriber* sub) const;
};