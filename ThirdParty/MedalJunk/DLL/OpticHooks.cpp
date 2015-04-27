#include "OpticHooks.h"
#include "PatchGroup.h"
#include "Patcher.h"
#include "Codefinder.h"
#include <cstdint>

namespace Optic { namespace Hooks {

void SpreeHandlerCodecave();
std::unique_ptr<PatchGroup> patches = nullptr;
std::uintptr_t ContinueSpreeString;
std::uintptr_t SkipSpreeString;

void install() {

}

void uninstall() {
	
}

void __declspec(naked) SpreeHandlerCodecave() {
	__asm {
		mov eax, dword ptr ss:[esp+0x0a0]
		cmp eax, 7
		jb Back
		cmp eax, 18
		ja Back
		cmp eax, 8
		je Back
		cmp eax, 13
		je Back
		jmp SkipSpreeString
	Back:
		jmp ContinueSpreeString
	}
}

}};