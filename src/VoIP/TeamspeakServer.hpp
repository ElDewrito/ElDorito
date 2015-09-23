#pragma once

DWORD WINAPI StartTeamspeakServer(LPVOID);
int kickTeamspeakClient(const std::string& name);
void StopTeamspeakServer();
bool IsVoIPServerRunning();