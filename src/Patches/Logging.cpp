#include "Logging.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamNetwork.hpp"
#include <Psapi.h>
#include <fstream>
#include <iostream>

namespace
{
	void dbglog(const char* module, char* format, ...);
	void debuglog_string(char* name, char* value);
	void debuglog_int(char* name, int value);
	void debuglog_float(char* name, float value);
	int networkLogHook(char* format, ...);
	void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5);
	void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5);
	bool __fastcall packetRecvHook(void *thisPtr, int unused, Blam::BitStream *stream, int *packetIdOut, int *packetSizeOut);
	void __fastcall packetSendHook(void *thisPtr, int unused, Blam::BitStream *stream, int packetId, int packetSize);
	void ExceptionHook(char* msg);

	Hook NetworkLogHook(0x9858D0, networkLogHook);
	Hook SSLHook(0xA7FE10, sslLogHook);
	Hook UIHook(0xAED600, uiLogHook);
	Hook Game1Hook(0x106FB0, dbglog);
	Hook DebugLogFloatHook(0x2189F0, debuglog_float);
	Hook DebugLogIntHook(0x218A10, debuglog_int);
	Hook DebugLogStringHook(0x218A30, debuglog_string);
	Hook PacketReceiveHook(0x7FF88, packetRecvHook, HookFlags::IsCall);
	Hook PacketSendHook(0x800A4, packetSendHook, HookFlags::IsCall);
}

namespace Patches
{
	namespace Logging
	{
		void ApplyAll()
		{
			Patch(0x11C158, { 0x8D, 0x85, 0x00, 0xFC, 0xFF, 0xFF, 0x50 }).Apply();
			Patch(0x11C165, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }).Apply();
			Hook(0x11C15F, ExceptionHook, HookFlags::IsCall).Apply();
		}

		void EnableNetworkLog(bool enable)
		{
			NetworkLogHook.Apply(!enable);
		}

		void EnableSslLog(bool enable)
		{
			SSLHook.Apply(!enable);
		}

		void EnableUiLog(bool enable)
		{
			UIHook.Apply(!enable);
		}

		void EnableGame1Log(bool enable)
		{
			Game1Hook.Apply(!enable);
		}

		void EnableGame2Log(bool enable)
		{
			DebugLogFloatHook.Apply(!enable);
			DebugLogIntHook.Apply(!enable);
			DebugLogStringHook.Apply(!enable);
		}

		void EnablePacketsLog(bool enable)
		{
			PacketReceiveHook.Apply(!enable);
			PacketSendHook.Apply(!enable);
		}
	}
}

namespace
{
	void dbglog(const char* module, char* format, ...)
	{
		char* backupFormat = "";
		if (!format)
			format = backupFormat;

		if (module != 0 && strcmp(module, "game_tick") == 0)
			return; // filter game_tick spam

		va_list ap;
		va_start(ap, format);

		char buff[4096];
		vsprintf_s(buff, 4096, format, ap);
		va_end(ap);

		Utils::DebugLog::Instance().Log(module, "%s", buff);
	}

	void debuglog_string(char* name, char* value)
	{
		Utils::DebugLog::Instance().Log("Debug", "%s: %s", name, value);
	}

	void debuglog_int(char* name, int value)
	{
		Utils::DebugLog::Instance().Log("Debug", "%s: %d", name, value);
	}

	void debuglog_float(char* name, float value)
	{
		Utils::DebugLog::Instance().Log("Debug", "%s: %f", name, value);
	}

	int networkLogHook(char* format, ...)
	{
		// fix strings using broken printf statements
		std::string formatStr(format);
		Utils::String::ReplaceString(formatStr, "%LX", "%llX");

		char dstBuf[4096];
		memset(dstBuf, 0, 4096);

		va_list args;
		va_start(args, format);
		vsnprintf_s(dstBuf, 4096, 4096, formatStr.c_str(), args);
		va_end(args);

		Utils::DebugLog::Instance().Log("Network", "%s", dstBuf);

		return 1;
	}

	void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5)
	{
		char* logData1 = (*(char**)(a3));
		char* logData2 = (*(char**)((DWORD_PTR)a3 + 0x8));
		if (logData1 == 0)
			logData1 = "";
		else
			logData1 += 0xC;

		if (logData2 == 0)
			logData2 = "";
		else
			logData2 += 0xC;

		Utils::DebugLog::Instance().Log((const char*)logData1, (char*)logData2);
		return;
	}

	void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5)
	{
		char* logData1 = (*(char**)(a3));
		if (logData1 == 0)
			logData1 = "";
		else
			logData1 += 0xC;

		Utils::DebugLog::Instance().Log("UiLog", (char*)logData1);
		return;
	}

	bool __fastcall packetRecvHook(void *thisPtr, int unused, Blam::BitStream *stream, int *packetIdOut, int *packetSizeOut)
	{
		typedef bool(__thiscall *DeserializePacketInfoPtr)(void *thisPtr, Blam::BitStream *stream, int *packetIdOut, int *packetSizeOut);
		auto DeserializePacketInfo = reinterpret_cast<DeserializePacketInfoPtr>(0x47FFE0);
		if (!DeserializePacketInfo(thisPtr, stream, packetIdOut, packetSizeOut))
			return false;
		
		auto packetTable = Blam::Network::GetPacketTable();
		if (!packetTable)
			return true;
		auto packet = &packetTable->Packets[*packetIdOut];
		Utils::DebugLog::Instance().Log("Packets", "RECV %s (size=0x%X)", packet->Name, *packetSizeOut);
		return true;
	}

	void __fastcall packetSendHook(void *thisPtr, int unused, Blam::BitStream *stream, int packetId, int packetSize)
	{
		typedef bool(__thiscall *SerializePacketInfoPtr)(void *thisPtr, Blam::BitStream *stream, int packetId, int packetSize);
		auto SerializePacketInfo = reinterpret_cast<SerializePacketInfoPtr>(0x4800D0);
		SerializePacketInfo(thisPtr, stream, packetId, packetSize);

		auto packetTable = Blam::Network::GetPacketTable();
		if (!packetTable)
			return;
		auto packet = &packetTable->Packets[packetId];
		Utils::DebugLog::Instance().Log("Packets", "SEND %s (size=0x%X)", packet->Name, packetSize);
	}

	struct ModuleInfo
	{
		std::string Path;
		uint32_t Address;
		size_t Size;
		uint32_t EntryPoint;
	};

	std::map<HMODULE, ModuleInfo> GetLoadedModules()
	{
		std::map<HMODULE, ModuleInfo> modules;

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
		if (!hProcess)
			return modules;

		HMODULE hMods[1024];
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (auto i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				if (!GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
					continue;

				MODULEINFO mod;
				if (!GetModuleInformation(hProcess, hMods[i], &mod, sizeof(MODULEINFO)))
					continue;

				ModuleInfo info;
				info.Path = szModName;
				info.Address = reinterpret_cast<uint32_t>(mod.lpBaseOfDll);
				info.Size = mod.SizeOfImage;
				info.EntryPoint = reinterpret_cast<uint32_t>(mod.EntryPoint);
				modules.insert({ hMods[i], info });
			}
		}

		CloseHandle(hProcess);

		return modules;
	}

	void ExceptionHook(char* msg)
	{
		auto* except = *Pointer::Base(0x1F8E880).Read<EXCEPTION_RECORD**>();

		Utils::DebugLog::Instance().Log("GameCrash", std::string(msg));

		Utils::DebugLog::Instance().Log("GameCrash", "Code: 0x%x, flags: 0x%x, record: 0x%x, addr: 0x%x, numparams: 0x%x",
			except->ExceptionCode, except->ExceptionFlags, except->ExceptionRecord, except->ExceptionAddress, except->NumberParameters);

		// identify problematic module, log the location and dump its contents
		auto modules = GetLoadedModules();
		for (auto&& module : modules)
		{
			if (reinterpret_cast<uint32_t>(except->ExceptionAddress) >= module.second.Address && 
				reinterpret_cast<uint32_t>(except->ExceptionAddress) < module.second.Address + module.second.Size)
			{
				Utils::DebugLog::Instance().Log("CrashModule", "0x%08X - 0x%08X | 0x%08X | %s",
					module.second.Address, module.second.Address + module.second.Size, module.second.EntryPoint, module.second.Path.c_str());

				// copy the module from the filesystem rather than in-memory for easier disassembly pre-relocation
				std::ifstream  src(module.second.Path.c_str(), std::ios::binary);
				std::ofstream  dst("crashmodule.bin", std::ios::binary);
				dst << src.rdbuf();

				break;
			}
		}

		std::exit(0);
	}
}