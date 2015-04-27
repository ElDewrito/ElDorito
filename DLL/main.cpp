#include "InitHooks.h"
#include "DebugHelper.h"
#include "PatchGroup.h"
#include "Shared.h"
#include <Windows.h>
#include <process.h>
#include <iostream>

unsigned int __stdcall hacLaunch(void*);
void unload();

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if(reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(module);

		HANDLE initialiser = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, hacLaunch, 0, CREATE_SUSPENDED, 0));
		
		if(initialiser == NULL) {
			OutputDebugString("Thread failure.");
			return FALSE;
		}

		try {
			HookManager::install();
		} catch(HookException& e) {
			HookManager::uninstall();
			MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		ResumeThread(initialiser);
	}

	if(reason == DLL_PROCESS_DETACH) {
		unload();
	}
    
	return TRUE;
}


void onLaunch(void* args) {
	HANDLE initialiser = static_cast<HANDLE>(args);

	DWORD result = WaitForSingleObject(initialiser, 10000);
	
	if(FAILED(result)) {
		if(result == WAIT_TIMEOUT) {
			OutputDebugString("Init wait timed out.");
		} else if(result == WAIT_FAILED) {
			DebugHelper::Translate("Thread waiting");
		}
	}

	CloseHandle(initialiser);
}

unsigned int __stdcall hacLaunch(void*) {
	dispatcher = new EventDispatcher();
	_beginthreadex(NULL, NULL, dispatcher->start, (void*)dispatcher, NULL, NULL);
	HookManager::post_launch();
	return 0;
}

void unload() {}
