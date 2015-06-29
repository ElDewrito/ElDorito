#include <Windows.h>
DWORD WINAPI StartTeamspeakClient(LPVOID);
void StopTeamspeakClient();


UINT64 VoIPGetscHandlerID();
UINT64 VoIPGetVadHandlerID();
INT VoIPGetTalkStatus();