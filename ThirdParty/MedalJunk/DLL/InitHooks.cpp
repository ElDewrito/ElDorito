#include "InitHooks.h"
#include "Patcher.h"
#include "codefinder.h"
#include "EngineTypes.h"
#include "EnginePointers.h"
#include "PatchGroup.h"
#include "Shared.h"
#include "Direct3D.h"
#include "DebugHelper.h"
#include "HookTest.h"
#include <sstream>
#include <string>
#include <d3d9.h>
#include <iostream>

namespace HookManager {

LAUNCHCB launchCB, quitCB;
void* launchArg = NULL, * quitArg = NULL;
std::vector <PatchGroup*> hooks;
void uninstall();
void installHooks();
PatchGroup* launch_event_hook();
void installPatches();
void locateAddresses();
PatchGroup* d3dHooks();
PatchGroup* secondaryD3DHook();
PatchGroup* tertiaryD3DHook();
PatchGroup* event_hook();
void find_resolution();
void find_volume();

void launchCallback(LAUNCHCB callback, void* data) {
	launchCB = callback;
	launchArg = data;
}

void quitCallback(LAUNCHCB callback, void* data) {
	quitCB = callback;
	quitArg = data;
}

void install() {	
	installHooks();
	installPatches();
	locateAddresses();
}

void uninstall() {
	hooks.clear();
}

void launchHook() {
	if(launchCB != nullptr) {
		launchCB(launchArg);
	}
}

void post_launch() {
	hooks.emplace_back(d3dHooks());
}

void quitHook() {
	if(quitCB != nullptr) {
		quitCB(quitArg);
	}
}

void installPatches() {

}

void installHooks() {
	hooks.emplace_back(event_hook());
}

void locateAddresses() {
	find_resolution();
	find_volume();
}

void find_volume() {
	short signature[] = { 0xC7, 0x86, 0x18, 0x00, 0x00, 0x00, -1, -1, -1, -1,
		0x8B, 0x96, 0x18, 0x00, 0x00, 0x00, 0x8B, 0x4D, 0x08, 0x89, 0x8A, 0x50, 0x1C, 0x04, 0x00 };

	DWORD address = FindCode(GetModuleHandle(0), signature, sizeof(signature) / 2);

	if(address != NULL) {
		address += 6;
		std::uint8_t* vol = (std::uint8_t*)*(std::uintptr_t*)address;
		vol += 0x41C50;
		master_volume = (std::uint32_t*)vol;
	} else {
		throw HookException("Failed to locate resolution information");
	}
}

void find_resolution() {
	short signature[] = { 0x66, 0x0F, 0x6E, 0x0D, -1, -1, -1, -1, 0x66, 0x0F, 0x6E, 0x05,
		-1, -1, -1, -1, 0x0F, 0x5B, 0xC9, 0x0F, 0x5B, 0xC0, 0x33 };
	DWORD address = FindCode(GetModuleHandle(0), signature, sizeof(signature) / 2);

	if(address != NULL) {
		address += 4;
		pResolution = (EngineTypes::Resolution*)*(DWORD*)address;
	} else {
		throw HookException("Failed to locate resolution information");
	}
}

PatchGroup* event_hook() {
	short signature[] = { 0x51, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x85, 0xC0, 0x74, 0x32, 
		0x6A, 0x00, 0x68, 0x00, 0x01, 0x00, 0x00, 0x8D, 0x8D, -1, -1, -1, -1, 0x51, 0x50 };

	PatchGroup *group = new PatchGroup();
	group->add(new CaveHook(signature, sizeof(signature) / 2, 1, event_filter, CaveHook::CALL_NO_TP,
		reinterpret_cast<std::uintptr_t*>(&original_event_filter)));

	if(!group->install()) {
		delete group;
		throw HookException("Event hook failed!");
	}

	return group;
}

PatchGroup* d3dHooks() {
	PatchGroup* group = tertiaryD3DHook();

	if(group) {
		return group;
	}

	group = secondaryD3DHook();

	if(group) {
		return group;
	}

	throw HookException("Direct3D hook failed!");
}

struct EnumData {
    DWORD processID;
    HWND window;
};

BOOL CALLBACK EnumProc(HWND window, LPARAM lParam) {
    EnumData& ed = *(EnumData*)lParam;
    DWORD processID;

    GetWindowThreadProcessId(window, &processID);

    if(ed.processID == processID) {
        ed.window = window;
        return FALSE;
    }

    return TRUE;
}

PatchGroup* secondaryD3DHook() {
	OutputDebugString("Sec hook");
	LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	
	if(!d3d9) {
		OutputDebugString("Unable to create D3D9 instance");
		return NULL;
	}

	EnumData ed = { GetProcessId(GetCurrentProcess()), NULL };
	EnumWindows(EnumProc, reinterpret_cast<LPARAM>(&ed));

	if(!ed.window) {
		OutputDebugString("Unable to locate window to attach");
		d3d9->Release();
		return NULL;
	}

	LPDIRECT3DDEVICE9 device = NULL;
	D3DPRESENT_PARAMETERS params = {};
	params.Windowed = TRUE;
	params.hDeviceWindow = ed.window;

	HRESULT res = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, ed.window,
									 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
									 &params, &device);

	if(res != D3D_OK) {
		OutputDebugString("Unable to create temporary device");
		device->Release();
		d3d9->Release();
		return NULL;
	}

	PatchGroup *group = new PatchGroup();
	std::uintptr_t* vtable = *reinterpret_cast<std::uintptr_t**>(device);

	group->add(new CaveHook(vtable[42], 0, D3DHook::endScene, CaveHook::CALL_DETOUR,
		reinterpret_cast<std::uintptr_t*>(&D3DHook::originalEndScene))); 
	group->add(new CaveHook(vtable[16], 0, D3DHook::reset, CaveHook::CALL_DETOUR,
		reinterpret_cast<std::uintptr_t*>(&D3DHook::originalReset)));

	device->Release();
	d3d9->Release();

	if(!group->install()) {
		OutputDebugString("Unable to hook D3D functions");
		delete group;
		return NULL;
	}

	return group;
}

PatchGroup* tertiaryD3DHook() {	
	short signature[] = {0xC7, 0x06, -1, -1, -1, -1, 0x89, 0x86, -1, -1, -1, -1, 0x89, 0x86};
	DWORD address = FindCode(GetModuleHandle("d3d9.dll"), signature, sizeof(signature) / 2);
	
	if(address == NULL) {
		OutputDebugString("Unable to locate D3D9 signature");
		return NULL;
	}

	address += 2;

	PatchGroup *group = new PatchGroup();
	std::uintptr_t* vtable = reinterpret_cast<std::uintptr_t*>(*(std::uintptr_t*)address);

	group->add(new CaveHook(vtable[42], 0, D3DHook::endScene, CaveHook::CALL_DETOUR,
		reinterpret_cast<std::uintptr_t*>(&D3DHook::originalEndScene))); 
	group->add(new CaveHook(vtable[16], 0, D3DHook::reset, CaveHook::CALL_DETOUR,
		reinterpret_cast<std::uintptr_t*>(&D3DHook::originalReset)));

	if(!group->install()) {
		OutputDebugString("Unable to hook D3D functions");
		delete group;
		return NULL;
	}

	return group;
}

};