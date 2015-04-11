#include "ElDorito.h"

void dbglog(const char* module, char* format, ...);

#pragma region Debug Log Hooks
extern void debuglog_string(char* name, char* value);
extern void debuglog_int(char* name, int value);
extern void debuglog_float(char* name, float value);
extern int networkLogHook(char* format, ...);
extern void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5);
extern void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5);
extern char* Network_GetIPStringFromInAddr(void* inaddr);
#pragma endregion

//extern void __fastcall UI_MenuUpdateHook(void* a1, int unused, int menuIdToLoad);
extern char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr);
extern char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid);

extern char __fastcall UI_Forge_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct);

extern void GLScoreboardPlayerAllocatorHook();
extern void GLScoreboardPlayerConstructorHook();
extern void ScoreboardUpdateHook();
extern void SetLocalPlayerHook();