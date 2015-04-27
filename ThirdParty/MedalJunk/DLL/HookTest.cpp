#include "HookTest.h"
#include "EnginePointers.h"
#include "EngineTypes.h"
#include "Optic.h"
#include <iostream>

tFilter original_event_filter;

char* event_filter(std::uint32_t some_other_id, std::uint32_t tag_id, std::uint32_t unk) {
	dispatcher->enqueue(std::make_shared<GameEvent>(tag_id));
	return original_event_filter(some_other_id, tag_id, unk);
}