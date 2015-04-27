#include "Shared.h"
#include "EnginePointers.h"

EventDispatcher* dispatcher;

void __declspec(naked) volume_adjust() {
	__asm {
		mov master_volume, ecx
	}
}