#include "Patcher.h"
#include "codefinder.h"
#include "DebugHelper.h"
extern "C" {
	#include "xde\xde.h"
}
#include <string>

const std::uint8_t PUSHFD = 0x9C;
const std::uint8_t PUSHAD = 0x60;
const std::uint8_t NEAR_CALL = 0xE8;
const std::uint8_t REL_JMP = 0xE9;
const std::uint8_t POPAD = 0x61;
const std::uint8_t POPFD = 0x9D;
const std::uint8_t RETN = 0xC3;
const std::uint8_t NOP = 0x90;

//todo - remove duplication
void CaveHook::init(std::uintptr_t address, int offset, void* destination, HOOK_TYPE type,
	std::uintptr_t* pReturn, unsigned int pushSet) {
	this->installed = false;
	this->pReturn = pReturn;
	this->pushSet = 0;
	this->destination = reinterpret_cast<std::uintptr_t>(destination);
	this->type = type;
	this->offset = offset;
	this->PatchAddress = address;
}

void CaveHook::init(const short* signature, unsigned int signatureLen, int offset, void* destination, HOOK_TYPE type,
	std::uintptr_t* pReturn, unsigned int pushSet) {
	this->installed = false;
	this->pReturn = pReturn;
	this->pushSet = 0;
	this->destination = reinterpret_cast<std::uintptr_t>(destination);
	this->type = type;
	this->offset = offset;
	this->PatchAddress = 0;
	std::copy(signature, signature + signatureLen, this->signature.begin());
}

CaveHook::CaveHook(std::uintptr_t address, int offset, void* destination, HOOK_TYPE type, std::uintptr_t* pReturn) {
	init(address, offset, destination, type, pReturn, 0);
}

CaveHook::CaveHook(std::uintptr_t address, int offset, void* destination, HOOK_TYPE type, unsigned int pushSet) {
	init(address, offset, destination, type, nullptr, pushSet);
}

CaveHook::CaveHook(const short* signature, unsigned int signatureLen, int offset, void* destination, HOOK_TYPE type, std::uintptr_t* pReturn)
		: signature(signatureLen) {
	init(signature, signatureLen, offset, destination, type, pReturn, NULL);
}

CaveHook::CaveHook(const short* signature, unsigned int signatureLen, int offset, void* destination, HOOK_TYPE type, unsigned int pushSet)
		: signature(signatureLen) {
	init(signature, signatureLen, offset, destination, type, NULL, pushSet);
}

CaveHook::~CaveHook() {
	uninstall();
}

bool CaveHook::install() {
	if(this->installed) {
		return true;
	}

	try {
		if(PatchAddress == NULL) {
			PatchAddress = FindCode(GetModuleHandle(0), &signature[0], signature.size());
			PatchAddress += this->offset;
		}
	} catch(std::exception) {
		return false;
	}

	if(PatchAddress == NULL) {
		return false;
	}

	unsigned char *cc_code = (unsigned char*)PatchAddress;

	size_t copied = 0;
	xde_instr info;

	while(copied < 5) {
		xde_disasm(cc_code + copied, &info);
		copied += info.len;
	}

	this->copied = copied;

	if(type == JMP_TP || type == CALL_TP || type == CALL_DETOUR) {
		destination = (std::uintptr_t)generateTrampoline(PatchAddress, destination, copied);
	}

	if(type == CALL_DETOUR) {
		destination += 2;
	}

	originalCode.resize(copied);
	std::copy(cc_code, cc_code + copied, originalCode.begin());

	if(type == JMP_TP) {
		*pReturn = destination + 7;
	} else if(type == NAKED) {
		*pReturn = PatchAddress + copied;
	} else if(type == CALL_DETOUR) {
		*pReturn = destination + 7;
	} else if(type == CALL_NO_TP && pReturn != NULL) {
		*pReturn = *(std::uintptr_t*)(PatchAddress + 1) + PatchAddress + 5;
	}
	
	// insert codecave jump
	DWORD OldProtect;
	if(VirtualProtect((LPVOID)PatchAddress, copied, PAGE_EXECUTE_READWRITE, &OldProtect) == 0) {
		return false;
	}

	cc_code[0] = (type == CALL_NO_TP)? NEAR_CALL : REL_JMP;
	
	std::uintptr_t CallOffset = destination - (PatchAddress + 5);
	*(std::uintptr_t*)(&cc_code[1]) = CallOffset;

	//Calculate how many bytes of the original instruction are remaining and then overwrite them with nops
	size_t bytesLeft = copied - 5; //a jmp hook is five bytes
	memset(&cc_code[5], 0x90, bytesLeft);

	//Restore original page protection
	VirtualProtect((LPVOID)PatchAddress, copied, OldProtect, &OldProtect);

	this->installed = true;
	return true;
}

void fixOffsets(unsigned char* trampoline, std::uintptr_t source) {
	char* OriginalCode = (char*)source;
	if (OriginalCode[0] == '\xE8' || OriginalCode[0] == '\xE9')
	{
		*(DWORD*)(trampoline + 1) += ((DWORD)OriginalCode - (DWORD)trampoline );
	}
}

void* CaveHook::generateTrampoline(std::uintptr_t source, std::uintptr_t destination, int copied) {
	this->generatedTrampoline = std::unique_ptr<std::uint8_t>(new std::uint8_t[14 + copied]);
	std::uint8_t* cave = this->generatedTrampoline.get();
	DWORD oldProtect;
	VirtualProtect(cave, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
	cave[0] = PUSHFD;
	cave[1] = PUSHAD;
	cave[2] = (type == CALL_TP)? NEAR_CALL : REL_JMP;

	std::uintptr_t dstOffset = destination - ((std::uintptr_t)cave + 7);
	memcpy_s(cave + 3, 4, &dstOffset, sizeof(dstOffset));

	cave[7] = POPAD;
	cave[8] = POPFD;

	memcpy_s(cave + 9, copied, (void*)source, copied);
	fixOffsets(&cave[9], source);

	cave[9 + copied] = REL_JMP;
	std::uintptr_t srcOffset = (source + 5) - ((std::uintptr_t)cave + 14 + copied);
	memcpy_s(cave + 10 + copied, 4, &srcOffset, sizeof(srcOffset)); 
	return cave;
}

void CaveHook::uninstall() {
	DWORD OldProtect;
	char *code = (char*)PatchAddress;
	VirtualProtect((LPVOID)PatchAddress, copied, PAGE_EXECUTE_READWRITE, &OldProtect);	
	std::copy(originalCode.begin(), originalCode.end(), code);
	VirtualProtect((LPVOID)PatchAddress, copied, OldProtect, &OldProtect);
	this->installed = false;
}

PatchHook::~PatchHook() {
	uninstall();
}

PatchHook::PatchHook(const short* signature, unsigned int signatureLen, int offset, const void* replacement, size_t replaceLen)
		: signature(signatureLen), replacement(replaceLen) {
	this->installed = false;
	this->PatchAddress = NULL;
	this->offset = offset;
	const std::uint8_t* rep = static_cast<const std::uint8_t*>(replacement);
	std::copy(signature, signature + signatureLen, this->signature.begin());
	std::copy(rep, rep + replaceLen, this->replacement.begin());
}

PatchHook::PatchHook(std::uintptr_t address, int offset, const void* replacement, size_t replaceLen) : replacement(replaceLen) {
	this->installed = false;
	this->offset = offset;
	this->PatchAddress = address;
	const std::uint8_t* rep = static_cast<const std::uint8_t*>(replacement);
	std::copy(rep, rep + replaceLen, this->replacement.begin());
}

bool PatchHook::install() {
	if(installed) {
		return true;
	}

	//If the user provided a signature instead of an address
	if(PatchAddress == NULL) {
		PatchAddress = FindCode(GetModuleHandle(0), &signature[0], signature.size());

		if(PatchAddress == NULL) {
			return false;
		}
	}

	PatchAddress += offset;
	size_t originalSize = replacement.size();

	//Back current code up
	originalCode.resize(originalSize);
	std::copy((char*)PatchAddress, (char*)PatchAddress + originalSize, originalCode.begin());

	//Install the patch and restore the page protection
	DWORD oldProtect = 0;
	VirtualProtect((void*)PatchAddress, originalSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((void*)PatchAddress, &replacement[0], originalSize);
	VirtualProtect((void*)PatchAddress, originalSize, oldProtect, &oldProtect);

	this->installed = true;
	return true;
}

void PatchHook::uninstall() {
	this->installed = false;
	DWORD OldProtect;
	char *code = (char*)PatchAddress;
	VirtualProtect((LPVOID)PatchAddress, replacement.size(), PAGE_EXECUTE_READWRITE, &OldProtect);	
	std::copy(originalCode.begin(), originalCode.end(), code);
	VirtualProtect((LPVOID)PatchAddress, replacement.size(), OldProtect, &OldProtect);
}