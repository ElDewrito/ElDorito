#include "Logging.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamNetwork.hpp"
#include <Psapi.h>
#include <fstream>
#include "../Blam/BlamMemory.hpp"
#include "../Utils/Logger.hpp"

namespace
{
	void dbglog(const char* module, char* format, ...);
	void __stdcall debuglog_string(char* name, char* value);
	void __stdcall debuglog_int(char* name, int value);
	void __stdcall debuglog_float(char* name, float value);
	int networkLogHook(char* format, ...);
	void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5);
	void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5);
	bool __fastcall packetRecvHook(void *thisPtr, int unused, Blam::BitStream *stream, int *packetIdOut, int *packetSizeOut);
	void __fastcall packetSendHook(void *thisPtr, int unused, Blam::BitStream *stream, int packetId, int packetSize);
	void ExceptionHook(char* msg);
	int __cdecl globalDataAllocateStructHook(int unk1, const char* name1, const char* name2, int size, int unk2, void* allocator, int unk3, int unk4);
	void* __cdecl globalDataInitializeArrayHook(Blam::DataArrayBase* dataArray, const char* name, int maxCount, int datumSize, uint8_t alignmentBits, void** allocator);
	int __cdecl globalDataInitializePoolHook(Blam::DataPoolBase* dataPool, const char* name, int size, int unk, void** allocator);
	void** __cdecl globalDataInitializeCacheHook(Blam::LruvCacheBase* lruvCache, const char* name, int unk1, void* unk2, void* unk3, void* unk4, void* unk5, void** allocator, int unk7);
	void* __stdcall virtualAllocHook(void* address, size_t size, uint32_t allocationType, uint32_t protect);

	void LogPhysicalMemoryAllocationRequestHook();
	void LogSuccessfulPhysicalMemoryAllocationResultHook();
	void LogFailedPhysicalMemoryAllocationResultHook();
	void __stdcall LogPhysicalMemoryAllocationRequest(uint32_t size, uint32_t flags, uint32_t stage);
	void __stdcall LogPhysicalMemoryAllocationResult(uint32_t address, uint32_t endAddress);

	Hook NetworkLogHook(0x9858D0, networkLogHook);
	Hook SSLHook(0xA7FE10, sslLogHook);
	Hook UIHook(0xAED600, uiLogHook);
	Hook Game1Hook(0x106FB0, dbglog);
	Hook DebugLogFloatHook(0x2189F0, debuglog_float);
	Hook DebugLogIntHook(0x218A10, debuglog_int);
	Hook DebugLogStringHook(0x218A30, debuglog_string);
	Hook PacketReceiveHook(0x7FF88, packetRecvHook, HookFlags::IsCall);
	Hook PacketSendHook(0x800A4, packetSendHook, HookFlags::IsCall);
	Hook MemoryGlobalAllocateStructHook(0x1A0010, globalDataAllocateStructHook);
	Hook MemoryGlobalInitializeArrayHook(0x15ACF0, globalDataInitializeArrayHook);
	Hook MemoryGlobalInitializePoolHook(0x56A5E0, globalDataInitializePoolHook);
	Hook MemoryGlobalInitializeCacheHook(0x567AD0, globalDataInitializeCacheHook);

	uint32_t origVirtualAllocAddress;
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

			NetworkLogHook.Apply();
			SSLHook.Apply();
			UIHook.Apply();
			Game1Hook.Apply();
			DebugLogFloatHook.Apply();
			DebugLogIntHook.Apply();
			DebugLogStringHook.Apply();
			PacketReceiveHook.Apply();
			PacketSendHook.Apply();

			MemoryGlobalAllocateStructHook.Apply();
			MemoryGlobalInitializeArrayHook.Apply();
			MemoryGlobalInitializePoolHook.Apply();
			MemoryGlobalInitializeCacheHook.Apply();

			// hook VirtualAlloc
			DWORD temp;
			VirtualProtect((void*)0x1600194, 4, PAGE_READWRITE, &temp);
			origVirtualAllocAddress = *(uint32_t*)0x1600194;
			*(uint32_t*)0x1600194 = (uint32_t)&virtualAllocHook;

			Hook(0x11D186, LogPhysicalMemoryAllocationRequestHook).Apply();
			Hook(0x11D1E0, LogFailedPhysicalMemoryAllocationResultHook).Apply();
			Hook(0x11D266, LogSuccessfulPhysicalMemoryAllocationResultHook).Apply();
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

		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Info, "%s: %s", module, buff);
	}

	void __stdcall debuglog_string(char* name, char* value)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Info, "%s: %s", name, value);
	}

	void __stdcall debuglog_int(char* name, int value)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Info, "%s: %d", name, value);
	}

	void __stdcall debuglog_float(char* name, float value)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Info, "%s: %f", name, value);
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

		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "%s", dstBuf);

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

		Utils::Logger::Instance().Log(Utils::LogTypes::Graphics, Utils::LogLevel::Info, "%s - %s", (const char*)logData1, (char*)logData2);
	}

	void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5)
	{
		char* logData1 = (*(char**)(a3));
		if (logData1 == 0)
			logData1 = "";
		else
			logData1 += 0xC;

		Utils::Logger::Instance().Log(Utils::LogTypes::Graphics, Utils::LogLevel::Info, (char*)logData1);
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
		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "RECV %s (size=0x%X)", packet->Name, *packetSizeOut);
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
		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "SEND %s (size=0x%X)", packet->Name, packetSize);
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

		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Error, std::string(msg));

		Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Error, "Code: 0x%x, flags: 0x%x, record: 0x%x, addr: 0x%x, numparams: 0x%x",
			except->ExceptionCode, except->ExceptionFlags, except->ExceptionRecord, except->ExceptionAddress, except->NumberParameters);

		// identify problematic module, log the location and dump its contents
		auto modules = GetLoadedModules();
		for (auto&& module : modules)
		{
			if (reinterpret_cast<uint32_t>(except->ExceptionAddress) >= module.second.Address && 
				reinterpret_cast<uint32_t>(except->ExceptionAddress) < module.second.Address + module.second.Size)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Error, "0x%08X - 0x%08X | 0x%08X | %s",
					module.second.Address, module.second.Address + module.second.Size, module.second.EntryPoint, module.second.Path.c_str());

				// copy the module from the filesystem rather than in-memory for easier disassembly pre-relocation
				std::ifstream  src(module.second.Path.c_str(), std::ios::binary);
				std::ofstream  dst("crashmodule.bin", std::ios::binary);
				dst << src.rdbuf();

				break;
			}
		}

		Utils::Logger::Instance().Flush();

		std::exit(0);
	}

	int __cdecl globalDataAllocateStructHook(int type, const char* name1, const char* name2, int size, int unk2, void* allocator, int unk3, int unk4)
	{
		uint32_t* unkPtr = reinterpret_cast<uint32_t*>(0x2406494 + type * 0xC88);
		typedef int(__thiscall *GlobalDataInitializeStruct)(void*, const char*, const char*, int, int, void*, int, int);
		int unk = reinterpret_cast<GlobalDataInitializeStruct>(0x65DF00)(unkPtr, name1, name2, size, unk2, allocator, unk3, unk4);

		// call entity-specific allocator
		void* address = reinterpret_cast<void*(__cdecl*)(int)>(allocator)
			(*reinterpret_cast<uint32_t*>(*reinterpret_cast<uint32_t*>(__readfsdword(44)) + 4 * type + 128) + unkPtr[6 * unk + 31]);

		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Info, "AllocateGlobalStruct - Address: 0x%08X, Allocator: 0x%08X, Type: %d, Unk2: %d, Unk3: %d, Unk4: %d, Name1: %s, Name2: %s, Size: %d",
			address, allocator, type, unk2, unk3, unk4, name1, name2, size);

		return unk;
	}

	void* __cdecl globalDataInitializeArrayHook(Blam::DataArrayBase* dataArray, const char* name, int maxCount, int datumSize, uint8_t alignmentBits, void** allocator)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Info, "InitGlobalArray - Address: 0x%08X, Allocator: 0x%08X, Name: %s, Count: %d, Size: %d, Alignment: %d, TotalSize: %d",
			dataArray, allocator, name, maxCount, datumSize, alignmentBits, Blam::CalculateDatumArraySize(maxCount, datumSize, alignmentBits));

		int padding = alignmentBits >= 0x20 ? 1 << alignmentBits : 0;
		char* data = reinterpret_cast<char*>(~((padding ^ (1 << alignmentBits)) - 1) & 
			reinterpret_cast<unsigned int>(&reinterpret_cast<char*>(dataArray)[(padding ^ (1 << alignmentBits)) + 83]));
		char* activeIndies = reinterpret_cast<char*>(data + datumSize * maxCount);

		memset(dataArray, 0, sizeof(Blam::DataArrayBase));
		strncpy(dataArray->Name, name, sizeof(dataArray->Name));
		dataArray->Alignment = alignmentBits;
		dataArray->TotalSize = activeIndies - reinterpret_cast<char*>(dataArray);
		dataArray->Allocator = allocator;
		dataArray->Flags = 0xFFFC;
		dataArray->Data = data;
		dataArray->MaxCount = maxCount;
		dataArray->DatumSize = datumSize;
		dataArray->Signature = 'd@t@';
		dataArray->IsValid = false;
		dataArray->ActiveIndices = reinterpret_cast<uint32_t*>(activeIndies);
		dataArray->HeaderSize = data - reinterpret_cast<char*>(dataArray);
		return memset(activeIndies, 0, 4 * ((maxCount + 31) >> 5));
	}

	int __cdecl globalDataInitializePoolHook(Blam::DataPoolBase* dataPool, const char* name, int size, int unk, void** allocator)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Info, "InitGlobalPool - Address: 0x%08X, Allocator: 0x%08X, Name: %s, Size: %d",
			dataPool, allocator, name, size);
		
		memset(dataPool, 0, sizeof(Blam::DataPoolBase));
		dataPool->Signature = 'pool';
		strncpy(dataPool->Name, name, sizeof(dataPool->Name));
		dataPool->Allocator = allocator;
		dataPool->Unk64 = unk;
		int temp = (reinterpret_cast<int>(dataPool) + 83) & 0xFFFFFFF0;
		dataPool->Padding = temp ? temp - reinterpret_cast<int>(dataPool) : 0;
		dataPool->Size = size;
		dataPool->FreeSize = size;
		dataPool->Unk52 = 0;
		dataPool->Unk56 = 0;
		dataPool->Unk60 = 0;
		dataPool->Unk63 = 0;

		return size;
	}

	void** __cdecl globalDataInitializeCacheHook(Blam::LruvCacheBase* lruvCache, const char* name, int unk1, void* unk2, void* unk3, void* unk4, void* unk5, void** allocator, int unk7)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Info, "InitLruvCache - Address: 0x%08X, Name: %s, Allocator: 0x%X, Unknowns: %d,0x%X,0x%X,0x%X,0x%X,%d",
			lruvCache, name, allocator, unk1, unk2, unk3, unk4, unk5, unk7);

		ZeroMemory(lruvCache, sizeof(Blam::LruvCacheBase));
		lruvCache->Unk124 = unk7;
		lruvCache->Unk32 = unk2;
		lruvCache->Unk36 = unk3;
		lruvCache->Unk40 = unk4;
		lruvCache->Unk44 = unk5;
		lruvCache->Unk60 = unk1;
		lruvCache->Unk56 = 0;
		lruvCache->Unk64 = -1;
		lruvCache->Unk112 = 0;
		lruvCache->Signature = 'weee';
		lruvCache->Unk72 = -1;
		lruvCache->Unk76 = -1;
		lruvCache->Unk68 = 1;
		lruvCache->Unk52 = 0;
		lruvCache->Allocator = allocator;
		return allocator;
	}

	void* __stdcall virtualAllocHook(void* address, size_t size, uint32_t allocationType, uint32_t protect)
	{
		void* addr = reinterpret_cast<void*(__stdcall*)(void*, size_t, uint32_t, uint32_t)>(origVirtualAllocAddress)
			(address, size, allocationType, protect);

		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Trace, "VirtualAlloc - Address: 0x%08X, Size: 0x%08X, Type: 0x%08X, Protect: 0x%08X, CallStack: %s",
			addr, size, allocationType, protect, Utils::GetStackTraceString(1, 5).c_str());

		return addr;
	}

	__declspec (naked) void LogPhysicalMemoryAllocationRequestHook()
	{
		__asm
		{
			mov		eax, dword ptr ds:[0238EC54h]

			pushad
			push	eax ; stage
			push	dword ptr [ebp+14h]	; flags
			push	dword ptr [ebp+10h]	; size
			call	LogPhysicalMemoryAllocationRequest
			popad

			push	051D18Bh
			ret
		}
	}
	
	__declspec (naked) void LogSuccessfulPhysicalMemoryAllocationResultHook()
	{
		__asm
		{
			pushad
			push	dword ptr [ebp+10h]	; end address
			push	eax			; address
			call	LogPhysicalMemoryAllocationResult
			popad

			pop		edi
			pop		esi
			mov		esp, ebp
			pop		ebp
			push	051D26Bh
			ret
		}
	}

	__declspec (naked) void LogFailedPhysicalMemoryAllocationResultHook()
	{
		__asm
		{
			pushad
			push	dword ptr [ebp+10h]	; end address
			push	eax			; address
			call	LogPhysicalMemoryAllocationResult
			popad

			pop		edi
			xor		eax, eax
			pop		esi
			mov		esp, ebp
			push	051D1E6h
			ret
		}
	}

	void __stdcall LogPhysicalMemoryAllocationRequest(uint32_t size, uint32_t flags, uint32_t stage)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Info, "PhysicalMapAllocRequest - Size: 0x%08X, Flags: %d, Stage: %d, CallStack: %s",
			size, flags, stage, Utils::GetStackTraceString(2, 5).c_str());
	}

	void __stdcall LogPhysicalMemoryAllocationResult(uint32_t address, uint32_t endAddress)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Memory, Utils::LogLevel::Info, "PhysicalMapAllocResult - Address: 0x%08X, Size: 0x%08X",
			address, endAddress - address);
	}
}