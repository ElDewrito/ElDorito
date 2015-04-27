/*
 *  Copyright HAC2 - blog.haloanticheat.com
 *  Released under MIT
 */
#include "codefinder.h"
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

PIMAGE_SECTION_HEADER CodeFinder::GetSection(HANDLE module) {
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)module;

	if(dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		return NULL;
	}

	IMAGE_NT_HEADERS* NtHeader = (IMAGE_NT_HEADERS*)((std::uintptr_t)module+dosHeader->e_lfanew);

	PIMAGE_SECTION_HEADER Section = (PIMAGE_SECTION_HEADER)( (std::uintptr_t)NtHeader + sizeof(IMAGE_NT_HEADERS));

	for(WORD i = 0; i < NtHeader->FileHeader.NumberOfSections; ++i, ++Section) {
		if(Section->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
			return Section;
		}
	}

	return NULL;
}

void CodeFinder::findCode(HANDLE module, const short *signature, size_t size, bool fastFind) {
    PIMAGE_SECTION_HEADER CodeSection = GetSection(module);
	
    if(CodeSection == NULL) {
		return;
	}

	if(fastFind) {
		boyerFind(signature, size, (BYTE*)module + CodeSection->VirtualAddress, CodeSection->SizeOfRawData);
	} else {
		for(size_t i = 0, j = 0; i < CodeSection->SizeOfRawData - size; i++, j = 0) {
			while(signature[j] == *(reinterpret_cast<BYTE*>(module) + CodeSection->VirtualAddress + i + j) || signature[j] == -1) {
				if(++j == size) {
					locations.emplace_back(CodeSection->VirtualAddress + i + reinterpret_cast<std::uintptr_t>(module));
				}
			}
		}
	}
}

std::vector<std::uintptr_t>::iterator CodeFinder::begin() {
	locations.clear();
	find();
	return locations.begin();
}

std::vector<std::uintptr_t>::iterator CodeFinder::end() {
	return locations.end();
}

CodeFinder::CodeFinder(HANDLE module, const short* signature, unsigned int signatureLen) {
	this->module = module;
	this->signature = signature;
	this->length = signatureLen;
}

std::vector<std::uintptr_t> CodeFinder::find() {
	bool fastFind = true;

	for(unsigned int i = 0; i < length; i++) {
		if(signature[i] == -1) {
			fastFind = false;
		} else if(signature[i] < 0 || signature[i] > 0xFF) {
			MessageBox(NULL, "Invalid signature", 0, 0);
		}
	}

	PIMAGE_DOS_HEADER dosHeader;
	dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);

	if(dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		//@todo error handling - move this to constructor
		return locations;
	}

	findCode(module, signature, length, fastFind);

    return locations; 
}

void CodeFinder::boyerFind(const short* signature, size_t sigLength, BYTE* memory, size_t memLength) {
	size_t bad_char_skip[256];
	size_t last = sigLength - 1;

	for(size_t i = 0; i <= 255; i++) {
		bad_char_skip[i] = sigLength;
	}

	for(size_t i = 0; i < last; i++) {
		bad_char_skip[signature[i]] = last - i;
	}

	while(memLength >= sigLength) {
		for(int i = last; signature[i] == memory[i]; --i) {
			if(i == 0) {
				locations.emplace_back(reinterpret_cast<std::uintptr_t>(&memory[i]));
			}
		}

		memLength -= bad_char_skip[memory[last]];
		memory += bad_char_skip[memory[last]];
	}
}

std::uintptr_t FindCode(HANDLE module, const short* signature, size_t signatureLen) {
	CodeFinder finder(module, signature, signatureLen);
	std::vector<std::uintptr_t> locations = finder.find();

	if(!locations.empty()) {
		return locations[0];
	} else {
		return NULL;
	}
}