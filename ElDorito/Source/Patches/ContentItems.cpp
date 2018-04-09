#include "ContentItems.hpp"

#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamData.hpp"

#include "../ThirdParty/dirent.h"

#include <ShlObj.h>

namespace
{
	const auto kMaxPath = 256;
	struct c_content_catalog;
	struct c_content_item : Blam::DatumBase
	{
		uint16_t Unknown02;
		uint32_t Unknown04;
		int ContentType;
		c_content_catalog *Catalog;
		uint8_t ContentHeader[0xF8];
		int Unknown108;
		int Unknown10C;
		// unimplemented
		char FilePath[kMaxPath];
	};
	static_assert(sizeof(c_content_item) == 0x210, "invalid c_content_item size");

	struct c_content_catalog
	{
		int LocalProfileIndex;
		Blam::DataArray<c_content_item> *Items;
		uint8_t Unknown08[0x2A8];
	};
	static_assert(sizeof(c_content_catalog) == 0x2B0, "invalid c_content_catalog size");

	struct c_content_item_overlapped_task
	{
		int LocalProfileIndex;
		uint32_t ContentItemDatumIndex;
		wchar_t *NewContentName;
		wchar_t *NewContentDescription;
		long *CompletionState;
		long *CompletionStatus;
	};

	bool IsProfileAvailable();
	bool __fastcall c_content_item__init_hook(c_content_item *thisptr, void *unused, int contentType, c_content_catalog *catalog,
		wchar_t *name, wchar_t *dashMetadata, int a5, int a6, int a7);
	char __stdcall PackageMountHook(int a1, int a2, int a3, int a4);
	uint32_t PackageMountHook2(int localProfileIndex, wchar_t *name, int contentType, volatile LONG *pStatus, volatile LONG *pState, volatile LONG *pContentItemDatumIndex);
	char* __cdecl AllocateGameGlobalMemory2Hook(char *Src, int a2, int a3, char a4, void* a5);
	bool __fastcall SaveFileGetNameHook(uint8_t *blfStart, void* unused, int a2, wchar_t *Src, size_t MaxCount);

	char CallsXEnumerateHook();
	char __fastcall FS_GetFiloForContentItemHook(c_content_item* contentItem, void* unused, void* filo);
	wchar_t* __fastcall FS_GetFilePathForContentItemHook(c_content_item* contentItem, void* unused, wchar_t* dest, size_t MaxCount);

	int content_item_overlapped_task_execute_hook(c_content_item_overlapped_task *task);
	int file_write_overlapped_task_execute_hook(uint8_t *task);
	void __fastcall c_gui_screen_pregame_selection__overlapped_task_update_hook(uint8_t *thisptr, void *unused);
	bool __fastcall c_gui_map_selected_item__get_file_path_hook(void *thisptr, void *unused, char *buff, int bufflen);
	bool __fastcall c_gui_game_variant_selected_item__get_file_path_hook(void *thisptr, void *unused, char *buff, int bufflen);
	void __fastcall c_content_item_delete_hook(c_content_item *thisptr, void *unused, int a1);

	struct
	{
		bool IsValid;
		char NewPath[kMaxPath];
	} s_ContentItemRenameState;
}

namespace Patches::ContentItems
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

		Hook(0x34CBE0, c_content_item__init_hook).Apply();

		// Hook (not patch, like above) content package mount stub to return true
		Hook(0x34D010, PackageMountHook).Apply();
		// Disable temp file creation
		Hook(0x1256D0, PackageMountHook2).Apply();

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
		Hook(0x34CCF0, FS_GetFiloForContentItemHook).Apply();
		Hook(0x34CE70, FS_GetFilePathForContentItemHook).Apply();

		Hook(0x00705388, c_gui_screen_pregame_selection__overlapped_task_update_hook, HookFlags::IsCall).Apply();
		Pointer(0x0169E4E4).Write(uint32_t(&c_gui_game_variant_selected_item__get_file_path_hook));
		Pointer(0x0169E244).Write(uint32_t(&c_gui_map_selected_item__get_file_path_hook));
		Pointer(0x526E50).Write(uint32_t(&content_item_overlapped_task_execute_hook));
		Hook(0x1A60E6, c_content_item_delete_hook, HookFlags::IsCall).Apply();
		Patch::NopFill(Pointer::Base(0x1277EA), 2);
		Patch::NopFill(Pointer::Base(0x127490), 2);

		Pointer(0x005AE015 + 1).Write(uint32_t(&file_write_overlapped_task_execute_hook));

		// replace x_memory_pool allocate/free (maybe increase the pool memory runtime state global?)
		Hook(0x127617, malloc, HookFlags::IsCall).Apply();
		Hook(0x1276C5, free, HookFlags::IsCall).Apply();
		Hook(0x127500, malloc, HookFlags::IsCall).Apply();
		Hook(0x1275E0, free, HookFlags::IsCall).Apply();
	}
}

namespace
{
	uint8_t* contentItemsGlobal = 0;
	bool enumerated = false;

	bool AddContentItem(wchar_t* itemPath)
	{
		uint8_t fileData[0xF8];

		FILE* file;
		if (_wfopen_s(&file, itemPath, L"rb") != 0 || !file)
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
		fread(fileData, 1, 0xF8, file);
		fclose(file);

		const auto sub_525330 = (signed int(*)(int a1))(0x525330);

		typedef int(__cdecl *GlobalsArrayPushFunc)(void* globalArrayPtr);
		GlobalsArrayPushFunc globalsArrayPush = (GlobalsArrayPushFunc)0x55B410;

		int dataIdx = globalsArrayPush(contentItemsGlobal);
		uint8_t* dataBasePtr = (uint8_t*)*(uint32_t*)(contentItemsGlobal + 0x44);
		auto* contentItem = (c_content_item*)(dataBasePtr + (0x240 * (uint16_t)dataIdx));

		const auto content_catalog_get = (c_content_catalog *(*)(int localProfileIndex))(0x005A5600);

		auto contentCatalog = content_catalog_get(0);
		auto contentType = sub_525330(*(uint32_t*)(fileData + 0xB8));
		contentItem->Unknown04 = 0x11;
		contentItem->ContentType = contentType;
		contentItem->Catalog = contentCatalog;
		memcpy(contentItem->ContentHeader, fileData, 0xF8);

		auto path = Utils::String::ThinString(itemPath);
		strcpy_s(contentItem->FilePath, path.c_str());

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
		wchar_t currentDir[kMaxPath];
		memset(currentDir, 0, kMaxPath * sizeof(wchar_t));
		GetCurrentDirectoryW(kMaxPath, currentDir);

		if (variantType == 10)
			swprintf_s(dest, MaxCount, L"%ls\\mods\\maps\\%ls\\", currentDir, variantName);
		else
			swprintf_s(dest, MaxCount, L"%ls\\mods\\variants\\%ls\\", currentDir, variantName);

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

		// TODO: change this to use unicode instead of ASCII

		char currentDir[kMaxPath];
		memset(currentDir, 0, kMaxPath);
		GetCurrentDirectoryA(kMaxPath, currentDir);

		std::string variantPath = std::string(currentDir) + std::string("\\mods\\variants");
		std::string mapsPath = std::string(currentDir) + std::string("\\mods\\maps");

		AddAllBLFContentItems(variantPath);
		AddAllBLFContentItems(mapsPath);

		enumerated = true;
		return 1;
	}

	void GetFilePathForContentItem(wchar_t* dest, size_t MaxCount, c_content_item* contentItem)
	{
		auto path = Utils::String::WidenString(contentItem->FilePath);
		wcscpy_s(dest, MaxCount, path.c_str());
	}

	char __fastcall FS_GetFiloForContentItemHook(c_content_item *contentItem, void* unused, void* filo)
	{
		typedef void*(__cdecl *FSCallsSetupFiloStruct2Func)(void *destFilo, const wchar_t *Src, char unk);
		FSCallsSetupFiloStruct2Func fsCallsSetupFiloStruct2 = (FSCallsSetupFiloStruct2Func)0x5285B0;

		auto path = Utils::String::WidenString(contentItem->FilePath);
		fsCallsSetupFiloStruct2(filo, path.c_str(), 0);
		return 1;
	}

	wchar_t* __fastcall FS_GetFilePathForContentItemHook(c_content_item *contentItem, void* unused, wchar_t* dest, size_t MaxCount)
	{
		auto path = Utils::String::WidenString(contentItem->FilePath);
		wcscpy_s(dest, MaxCount, path.c_str());
		return dest;
	}

	bool __fastcall SaveFileGetNameHook(uint8_t *blfStart, void* unused, int a2, wchar_t *Src, size_t MaxCount)
	{
		// TODO: moving this to Documents\\My Games\\Halo 3\\Saves\\Maps / Games / etc might be better, no need to worry about admin perms

		wchar_t* variantName = (wchar_t*)(blfStart + 0x48); // when saving forge maps we only get the variant name, not the blf data :s
		int variantType = *(uint32_t*)(blfStart + 0xF8);

		GetFilePathForItem(Src, MaxCount, variantName, variantType);

		return 1;
	}

	bool __fastcall c_content_item__init_hook(c_content_item *thisptr, void *unused, int contentType, c_content_catalog *catalog,
		wchar_t *name, wchar_t *dashMetadata, int a5, int a6, int a7)
	{
		const auto sub_5244C0 = (signed int(*)(int a1))(0x005244C0);

		thisptr->Unknown04 = 0x11;
		thisptr->ContentType = contentType;
		thisptr->Catalog = catalog;

		wchar_t buff[kMaxPath];
		GetFilePathForItem(buff, kMaxPath, name, sub_5244C0(contentType));
		auto path = Utils::String::ThinString(buff);
		strcpy_s(thisptr->FilePath, path.c_str());

		return true;
	}

	char __stdcall PackageMountHook(int a1, int a2, int a3, int a4)
	{
		return 1;
	}

	uint32_t PackageMountHook2(int localProfileIndex, wchar_t *name, int contentType, volatile LONG *pStatus, volatile LONG *pState, volatile LONG *pContentItemDatumIndex)
	{
		const auto sub_525330 = (int(*)(int a1))(0x525330);
		const auto content_catalog_get = (c_content_catalog *(*)(int localProfileIndex))(0x005A5600);
		const auto c_content_catalog__get_or_create_content_item = (int(__thiscall *)(c_content_catalog *thisptr,
			wchar_t *name, wchar_t *dashMetadata, int contentType, char a5, int a6, int a7))(0x005A5E70);

		auto status = 0;
		uint32_t contentItemIndex = -1;

		contentType = sub_525330(contentType);
		if (contentType != -1)
		{
			auto catalog = content_catalog_get(localProfileIndex);
			if (catalog)
			{
				contentItemIndex = c_content_catalog__get_or_create_content_item(catalog, name, L"", contentType, 0, 0, 0);
				if (contentItemIndex != -1)
					status = 1;
			}
		}
		InterlockedExchange(pState, 1);
		InterlockedExchange(pStatus, status);
		InterlockedExchange(pContentItemDatumIndex, contentItemIndex);
		return 0;
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

	char *GetDirectoryPath(const char *path, char* buff, int bufflen)
	{
		strcpy_s(buff, bufflen, path);
		auto *filePart = strrchr(buff, '\\');
		if (filePart)
		{
			*filePart = 0;
			return buff;
		}
		return nullptr;
	}

	wchar_t *GetDirectoryPathW(wchar_t *path, wchar_t* buff, int bufflen)
	{
		wcscpy_s(buff, bufflen, path);
		auto *filePart = wcsrchr(buff, L'\\');
		if (filePart)
		{
			*filePart = 0;
			return buff;
		}
		return nullptr;
	}


	int content_item_overlapped_task_execute_hook(c_content_item_overlapped_task *task)
	{
		const auto EditContentHeaderOverlappedTaskFunc = (bool(*)(c_content_item_overlapped_task *task))(0x526E90);

		const auto local_profile_get_content_catalog = (c_content_catalog*(*)(int profileIndex))(0x005A5600);
		const auto c_content_catalog__get_availability = (uint32_t(__thiscall *)(c_content_catalog *thisptr))(0x005A7A80);
		const auto c_content_catalog__get_item_filo = (bool(__thiscall *)(c_content_catalog *thisptr, uint32_t contentItemDatumIndex, void *filo))(0x5A6780);
		const auto string_unicode_to_ascii = (void(*)(wchar_t *src, char *dest, int destlen, void *a4))(0x004EDEA0);

		s_ContentItemRenameState.IsValid = false;

		if (task->NewContentName)
		{
			auto catalog = local_profile_get_content_catalog(task->LocalProfileIndex);
			if (c_content_catalog__get_availability(catalog) != 0xCACACACA)
			{
				auto item = catalog->Items->Get(task->ContentItemDatumIndex);
				if (item)
				{
					uint8_t filo[0x208];
					c_content_catalog__get_item_filo(catalog, task->ContentItemDatumIndex, filo);
					auto filePath = (char*)(filo + 0x8);

					char oldDirectory[kMaxPath];
					char newDirectoryPath[kMaxPath];
					wchar_t newFilePath[kMaxPath];

					GetFilePathForItem(newFilePath, kMaxPath, task->NewContentName, *(int*)(item->ContentHeader + 0xB8));
					auto newFilePathThin = Utils::String::ThinString(newFilePath);

					GetDirectoryPath(filePath, oldDirectory, kMaxPath);
					GetDirectoryPath(newFilePathThin.c_str(), newDirectoryPath, kMaxPath);
					SHCreateDirectoryEx(nullptr, newDirectoryPath, nullptr);

					auto v2 = EditContentHeaderOverlappedTaskFunc(task);
					if (*task->CompletionStatus == 1)
					{
						strcpy_s(item->FilePath, newFilePathThin.c_str());
						strcpy_s(s_ContentItemRenameState.NewPath, item->FilePath);
						s_ContentItemRenameState.IsValid = true;

						MoveFileA(filePath, s_ContentItemRenameState.NewPath);
						RemoveDirectoryA(oldDirectory);
						
					}

					return v2;
				}
			}
		}

		return EditContentHeaderOverlappedTaskFunc(task);
	}

	void __fastcall c_content_item_delete_hook(c_content_item *thisptr, void *unused, int a1)
	{
		char tmp[kMaxPath];
		if (GetDirectoryPath(thisptr->FilePath, tmp, kMaxPath))
		{
			DeleteFile(thisptr->FilePath);
			RemoveDirectory(tmp);
		}
	}

	void __fastcall c_gui_screen_pregame_selection__overlapped_task_update_hook(uint8_t *thisptr, void *unused)
	{
		const auto c_gui_screen_pregame_selection__overlapped_task_update = (void(__thiscall*)(uint8_t *thisptr))(0x00B053D0);
		const auto c_gui_widget__find_named_child = (void*(__thiscall *)(void *thisptr, uint32_t name))(0x00AB8AA0);
		const auto c_gui_list_widget__get_ordered_data_source = (void*(__thiscall *)(void *thisptr))(0x00B14FE0);
		const auto c_gui_list_widget__get_selected_item_index = (int(__thiscall *)(void *thisptr))(0xB150E0);
		const auto c_filo__set_path = (int(*)(void *a1, char *a2))(0x528880);
		const auto c_gui_screen_widget__get_local_profile_index = (int(__thiscall *)(void *thisptr))(0x00AB96C0);

		if (*(uint32_t*)(thisptr + 0x1AB0) == -1 && *(uint32_t *)(thisptr + 0x1AB4) != -1)
		{
			if (*(uint32_t*)(thisptr + 0x1AC0) == 1 && s_ContentItemRenameState.IsValid)
			{
				s_ContentItemRenameState.IsValid = false;

				auto subitem = c_gui_widget__find_named_child(thisptr, 0x102E6);
				if (subitem)
				{
					auto datasource = c_gui_list_widget__get_ordered_data_source(subitem);
					auto selectedIndex = c_gui_list_widget__get_selected_item_index(subitem);
					if (selectedIndex != -1)
					{
						const auto c_ordered_datasource__get_item_at =
							(void*(__thiscall*)(void *thisptr, int index))(*(void**)(*(int*)datasource + 0x44));

						auto item = c_ordered_datasource__get_item_at(datasource, selectedIndex);
						if (item)
							c_filo__set_path((char*)item + 0x194, s_ContentItemRenameState.NewPath);
					}
				}
			}
		}

		c_gui_screen_pregame_selection__overlapped_task_update(thisptr);
	}

	bool __fastcall c_gui_map_selected_item__get_file_path_hook(void *thisptr, void *unused, char *buff, int bufflen)
	{
		strcpy_s(buff, bufflen, (char*)((uint8_t*)thisptr + 0x19c));
		return true;
	}

	bool __fastcall c_gui_game_variant_selected_item__get_file_path_hook(void *thisptr, void *unused, char *buff, int bufflen)
	{
		strcpy_s(buff, bufflen, (char*)((uint8_t*)thisptr + 0x1A0));
		return true;
	}

	int file_write_overlapped_task_execute_hook(uint8_t *task)
	{
		const auto file_write_overlapped_task_execute = (int(*)(uint8_t *data))(0x5AF410);

		wchar_t directoryPath[kMaxPath];
		if (GetDirectoryPathW((wchar_t*)task, directoryPath, kMaxPath))
			SHCreateDirectoryExW(nullptr, directoryPath, nullptr);

		return file_write_overlapped_task_execute(task);
	}
}
