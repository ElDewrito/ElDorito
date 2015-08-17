#pragma once

#include <Windows.h>

DWORD WINAPI StartTeamspeakClient(LPVOID);
void StopTeamspeakClient();
int muteTeamspeakClient(const std::string& name);

UINT64 VoIPGetscHandlerID();
UINT64 VoIPGetVadHandlerID();
INT VoIPGetTalkStatus();