#include "ContentItems.h"

#include "../ElDorito.h"
#include "../Patch.h"

#include "../ThirdParty/dirent.h"

#include <ShlObj.h>

namespace
{
	bool IsProfileAvailable();
	char __stdcall PackageCreateHook(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
	char __stdcall PackageMountHook(int a1, int a2, int a3, int a4);
	wchar_t* __stdcall GetContentMountPathHook(wchar_t* destPtr, int size, int unk);
	char* __cdecl AllocateGameGlobalMemory2Hook(char *Src, int a2, int a3, char a4, void* a5);
	bool __fastcall SaveFileGetNameHook(uint8_t *blfStart, void* unused, int a2, wchar_t *Src, size_t MaxCount);

	char CallsXEnumerateHook();
	char __fastcall FS_GetFiloForContentItemHook(uint8_t* contentItem, void* unused, void* filo);
	char __fastcall FS_GetFiloForContentItemHook1(uint8_t* contentItem, void* unused, void* filo);
	wchar_t* __fastcall FS_GetFilePathForContentItemHook(uint8_t* contentItem, void* unused, wchar_t* dest, size_t MaxCount);
	char __fastcall Game_SetFlagAfterCopyBLFDataHook(uint8_t* flag, void* unused, char flagIdx, char set);
}

namespace Patches
{
	namespace ContentItems
	{
		void ApplyAll()
		{
			// Allow saving content without a profile
			Hook(0x67DCA0, IsProfileAvailable).Apply();

			// Fix storage device checks, so storage device funcs return 0 instead of 0xCACACACA
			Patch(0x1A7A80, { 0x31, 0xc0, 0xC3 }).Apply();
			Patch(0x34D570, { 0x31, 0xc0, 0xC3 }).Apply();

			// Hook this AllocateGameGlobalMemory to use a different one (this one is outdated mb? crashes when object is added to "content items" global without this hook)
			Hook(0x15B010, AllocateGameGlobalMemory2Hook).Apply();

			// Hook GetContentMountPath to actually return a dest folder
			Hook(0x34CC00, GetContentMountPathHook).Apply();

			// Hook (not patch, important otherwise stack gets fucked) content_catalogue_create_new_XContent_item stub to return true
			Hook(0x34CBE0, PackageCreateHook).Apply();

			// Hook (not patch, like above) content package mount stub to return true
			Hook(0x34D010, PackageMountHook).Apply();

			// Hook the func that gets the save file dest. name, 
			Patch(0x12708E, { 0x8B, 0x4D, 0x14 }).Apply(); // pass blf data to our func
			Hook(0x127091, SaveFileGetNameHook, HookFlags::IsCall).Apply();
			Patch(0x127096, 0x90, 3).Apply();

			// needed for the game to load our items in "content items" global
			Patch(0x6DC0EA, 0x90, 6).Apply();
			Patch(0x6DA59A, 0x90, 6).Apply();
			//Patch(0x34CEC8, 0x90, 2).Apply();

			// patch the func that handles game variant BLFs so it'll load byteswapped blfs
			Patch(0x1731CD, 0x90, 2).Apply();
			Patch(0x1732CD, 0x90, 2).Apply();

			// patch proper endian -1 into BLF header creation
			Patch(0x62AE8, { 0xFF, 0xFE }).Apply();

			// patch content items global size
			Pointer::Base(0x1A7E44).Write<uint32_t>(1024);

			Hook(0x1A9050, CallsXEnumerateHook).Apply();
			Hook(0x34CE00, FS_GetFiloForContentItemHook).Apply(); // game doesnt seem to use the filo for this? maybe used for overwriting
			Hook(0x34CE70, FS_GetFilePathForContentItemHook).Apply();
			Hook(0x34CCF0, FS_GetFiloForContentItemHook1).Apply();

			Hook(0x34D376, Game_SetFlagAfterCopyBLFDataHook, HookFlags::IsCall).Apply();
		}
	}
}

namespace
{
	uint8_t* contentItemsGlobal = 0;
	bool enumerated = false;

	bool AddContentItem(wchar_t* itemPath)
	{
		uint8_t fileData[0xF0];

		FILE* file;
		if (_wfopen_s(&file, itemPath, L"rb") != 0)
			return false;

		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		if (fileSize < 0x40)
		{
			// too small to be a BLF
			fclose(file);
			return false;
		}

		uint32_t magic;
		fread(&magic, 4, 1, file);
		if (magic != 0x5F626C66 && magic != 0x666C625F)
		{
			// not a BLF
			fclose(file);
			return false;
		}

		fseek(file, 0x40, SEEK_SET);
		fread(fileData, 1, 0xF0, file);
		fclose(file);

		typedef int(__cdecl *GlobalsArrayPushFunc)(void* globalArrayPtr);
		GlobalsArrayPushFunc globalsArrayPush = (GlobalsArrayPushFunc)0x55B410;

		int dataIdx = globalsArrayPush(contentItemsGlobal);
		uint8_t* dataBasePtr = (uint8_t*)*(uint32_t*)(contentItemsGlobal + 0x44);
		uint8_t* dataPtr = dataBasePtr + (0x240 * (uint16_t)dataIdx);

		*(uint32_t*)(dataPtr + 4) = 0x11;
		*(uint32_t*)(dataPtr + 8) = 4; // this is a blf/variant/content item type field, but setting it to 4 (slayer) works for everything
		*(uint32_t*)(dataPtr + 0xC) = (uint32_t)dataPtr;


		memcpy(dataPtr + 0x10, fileData, 0xF0);
		wcscpy_s((wchar_t*)(dataPtr + 0x100), 0xA0, itemPath);

		return true;
	}

	bool AddContentItem(std::string itemPath)
	{
		// need to convert path from ASCII to unicode now
		std::wstring unicode = Utils::String::WidenString(itemPath);
		return AddContentItem((wchar_t*)unicode.c_str());
	}

	void GetFilePathForItem(wchar_t* dest, size_t MaxCount, wchar_t* variantName, int variantType)
	{
		wchar_t currentDir[256];
		memset(currentDir, 0, 256 * sizeof(wchar_t));
		GetCurrentDirectoryW(256, currentDir);

		if (variantType == 10)
			swprintf_s(dest, MaxCount, L"%ls\\mods\\maps\\%ls\\", currentDir, variantName);
		else
			swprintf_s(dest, MaxCount, L"%ls\\mods\\variants\\%ls\\", currentDir, variantName);

		SHCreateDirectoryExW(NULL, dest, NULL);

		if (variantType == 10)
			swprintf_s(dest, MaxCount, L"%ls\\mods\\maps\\%ls\\sandbox.map", currentDir, variantName);
		else
		{
			typedef wchar_t*(__cdecl *FS_GetFileNameForItemTypeFunc)(uint32_t type);
			FS_GetFileNameForItemTypeFunc FS_GetFileNameForItemType = (FS_GetFileNameForItemTypeFunc)0x526550;
			wchar_t* fileName = FS_GetFileNameForItemType(variantType);

			swprintf_s(dest, MaxCount, L"%ls\\mods\\variants\\%ls\\%ls", currentDir, variantName, fileName);
		}
	}

	void AddAllBLFContentItems(std::string path)
	{
		struct dirent *entry;
		DIR *dp;
		dp = opendir(path.c_str());
		if (dp == NULL)
			return;

		while ((entry = readdir(dp)))
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;

			std::string filePath = path + std::string("\\") + std::string(entry->d_name);

			if (S_ISDIR(entry->d_type)) // if it's a folder, recurse through it
				AddAllBLFContentItems(filePath);
			else if (S_ISREG(entry->d_type)) // otherwise if it's a file try adding it as a BLF
				AddContentItem(filePath);
		}

		closedir(dp);
	}

	char CallsXEnumerateHook()
	{
		if (!contentItemsGlobal)
			return 0;
		if (enumerated)
			return 1;

		TODO("change this to use unicode instead of ASCII");

		char currentDir[256];
		memset(currentDir, 0, 256);
		GetCurrentDirectoryA(256, currentDir);

		std::string variantPath = std::string(currentDir) + std::string("\\mods\\variants");
		std::string mapsPath = std::string(currentDir) + std::string("\\mods\\maps");

		AddAllBLFContentItems(variantPath);
		AddAllBLFContentItems(mapsPath);

		enumerated = true;
		return 1;
	}

	void GetFilePathForContentItem(wchar_t* dest, size_t MaxCount, uint8_t* contentItem)
	{
		wchar_t* variantName = (wchar_t*)(contentItem + 0x18);
		uint32_t variantType = *(uint32_t*)(contentItem + 0xC8);

		GetFilePathForItem(dest, MaxCount, variantName, variantType);
	}


	char __fastcall Game_SetFlagAfterCopyBLFDataHook(uint8_t* flag, void* unused, char flagIdx, char set)
	{
		typedef char(__fastcall *Game_SetFlagFunc)(uint8_t* flag, void* unused, char flagIdx, char set);
		Game_SetFlagFunc Game_SetFlag = (Game_SetFlagFunc)0x52BD40;
		char ret = Game_SetFlag(flag, unused, flagIdx, set);

		uint8_t* contentItem = flag - 4;

		*(uint32_t*)(contentItem + 4) = 0x11;
		*(uint32_t*)(contentItem + 8) = 4; // this is a blf/variant/content item type field, but setting it to 4 (slayer) works for everything
		*(uint32_t*)(contentItem + 0xC) = (uint32_t)contentItem;

		wchar_t* variantPath = (wchar_t*)(contentItem + 0x100);

		GetFilePathForContentItem(variantPath, 0x100, contentItem);

		return ret;
	}

	char __fastcall FS_GetFiloForContentItemHook(uint8_t* contentItem, void* unused, void* filo)
	{
		typedef void*(__cdecl *FSCallsSetupFiloStruct2Func)(void *destFilo, wchar_t *Src, char unk);
		FSCallsSetupFiloStruct2Func fsCallsSetupFiloStruct2 = (FSCallsSetupFiloStruct2Func)0x5285B0;
		fsCallsSetupFiloStruct2(filo, (wchar_t*)(contentItem + 0x100), 0);

		return 1;
	}

	char __fastcall FS_GetFiloForContentItemHook1(uint8_t* contentItem, void* unused, void* filo)
	{
		typedef void*(__cdecl *FSCallsSetupFiloStruct2Func)(void *destFilo, wchar_t *Src, char unk);
		FSCallsSetupFiloStruct2Func fsCallsSetupFiloStruct2 = (FSCallsSetupFiloStruct2Func)0x5285B0;
		fsCallsSetupFiloStruct2(filo, (wchar_t*)(contentItem + 0x100), 1);

		return 1;
	}

	wchar_t* __fastcall FS_GetFilePathForContentItemHook(uint8_t* contentItem, void* unused, wchar_t* dest, size_t MaxCount)
	{
		// copy the path we put in the unused XCONTENT_DATA field in the content item global
		wcscpy_s(dest, MaxCount, (wchar_t*)(contentItem + 0x100));
		return dest;
	}

	bool __fastcall SaveFileGetNameHook(uint8_t *blfStart, void* unused, int a2, wchar_t *Src, size_t MaxCount)
	{
		TODO("moving this to Documents\\My Games\\Halo 3\\Saves\\Maps / Games / etc might be better, no need to worry about admin perms");

		wchar_t* variantName = (wchar_t*)(blfStart + 0x48); // when saving forge maps we only get the variant name, not the blf data :s
		int variantType = *(uint32_t*)(blfStart + 0xF8);

		GetFilePathForItem(Src, MaxCount, variantName, variantType);

		return 1;
	}

	char __stdcall PackageCreateHook(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
	{
		return 1;
	}

	char __stdcall PackageMountHook(int a1, int a2, int a3, int a4)
	{
		return 1;
	}

	wchar_t* __stdcall GetContentMountPathHook(wchar_t* destPtr, int size, int unk)
	{
		TODO("move this to temp folder, or find a way to disable it (game uses path returned by this func to create a 0 byte sandbox.map)");
		wchar_t currentDir[256];
		memset(currentDir, 0, 256 * sizeof(wchar_t));
		GetCurrentDirectoryW(256, currentDir);

		swprintf_s(destPtr, size, L"%ls\\mods\\temp\\", currentDir);

		SHCreateDirectoryExW(NULL, destPtr, NULL);
		return destPtr;
	}

	char* __cdecl AllocateGameGlobalMemory2Hook(char *Src, int a2, int a3, char a4, void* a5)
	{
		typedef char*(__cdecl *AllocateGameGlobalMemory)(char *Src, int a2, int a3, char a4, void* a5);
		AllocateGameGlobalMemory allocateGlobal = (AllocateGameGlobalMemory)0x55AFA0;
		char* retData = allocateGlobal(Src, a2, a3, a4, a5);

		*(uint16_t*)(retData + 0x2A) = 0x3;

		if (Src == (char*)0x165B170) // "content items"
		{
			// set some bytes thats needed in the contentfile related funcs
			*(uint8_t*)(retData + 0x29) = 1;
			*(uint32_t*)(retData + 0x40) = 1;
			contentItemsGlobal = (uint8_t*)retData;
		}

		return retData;
	}

	bool IsProfileAvailable()
	{
		return true;
	}
}