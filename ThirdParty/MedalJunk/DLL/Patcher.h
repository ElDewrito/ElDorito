#pragma once

#include "IHook.h"
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <memory>

class CaveHook : public IHook {
public:
	enum HOOK_TYPE {
		NAKED,
		JMP_TP,
		CALL_TP,
		CALL_NO_TP,
		CALL_DETOUR
	};

	enum PUSH {
		EAX = 1,
		ECX = 2,
		EDX = 4,
		EBX = 8,
		ESP = 16,
		EBP = 32,
		ESI = 64,
		EDI = 128
	};
	
	CaveHook(std::uintptr_t address, int offset, void* destination, HOOK_TYPE type,
			 std::uintptr_t* pReturn = NULL);
	CaveHook(std::uintptr_t address, int offset, void* destination, HOOK_TYPE type,
			 unsigned int pushSet);
	CaveHook(const short* signature, unsigned int signatureLen, int offset, void* destination, HOOK_TYPE type,
			 std::uintptr_t* pReturn = NULL);
	CaveHook(const short* signature, unsigned int signatureLen, int offset, void* destination, HOOK_TYPE type,
			 unsigned int pushSet);
	~CaveHook();
	bool install();

private:
	size_t copied;
	std::vector<char> originalCode;
	std::uintptr_t* pReturn;
	std::uintptr_t destination;
	std::uintptr_t PatchAddress;
	std::uintptr_t targetAddress;
	bool trampoline;
	int offset;
	std::vector<short> signature;
	HOOK_TYPE type;
	bool installed;
	std::unique_ptr<std::uint8_t> generatedTrampoline;
	unsigned int pushSet;

	void uninstall();
	void* generateTrampoline(std::uintptr_t source, std::uintptr_t destination, int copied);
	void init(const short* signature, unsigned int signatureLen, int offset, void* destination, HOOK_TYPE type,
			  std::uintptr_t* pReturn, unsigned int pushSet);
	void init(std::uintptr_t address, int offset, void* destination, HOOK_TYPE type,
			  std::uintptr_t* pReturn, unsigned int pushSet);
};

class PatchHook : public IHook {
	void uninstall();
	std::vector<char> originalCode;
	std::vector<short> signature;
	std::vector<std::uint8_t> replacement;
	int offset;
	std::uintptr_t PatchAddress;
	bool installed;

public:
	PatchHook(const short* signature, unsigned int signatureLen, int offset, const void* replacement, size_t replaceLen);
	PatchHook(std::uintptr_t address, int offset, const void* replacement, size_t replaceLen);
	~PatchHook();
	bool install();
};