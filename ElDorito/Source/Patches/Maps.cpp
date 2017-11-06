#include "Maps.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamTypes.hpp"

namespace
{
	void __fastcall c_map_variant_initialize_default_hook(Blam::MapVariant* thisptr, void* unused, int mapId);
	int EnumerateMapsHook(int a1);

	bool TryLoadDefaultMapVariant(int mapId, Blam::MapVariant* buffer);
}

namespace Patches::Maps
{
	void ApplyAll()
	{
		Pointer(0x0054AD9B + 1).Write(uint32_t(&EnumerateMapsHook));
		Hook(0x6A81CF, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
		Hook(0x6A7954, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
		Hook(0x6A77CA, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
		Hook(0x6A78A3, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
		Hook(0x6A79D9, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
		// default to guardian
		Patch(0x14BCA0, { 0xB8, 0x40, 0x01, 0x00, 0x00, 0xC3 }).Apply();

		Patch(0x7004DF, { 0xEB }).Apply();
	}

	void InitializeMapVariant(Blam::MapVariant *mapv, int mapid)
	{
		const auto c_map_variant_initialize = (void(__thiscall *)(Blam::MapVariant* thisptr, int mapId))(0x00581F70);
		// if we can't load the variant in the .map file, default to one generated from the scenario
		if (!TryLoadDefaultMapVariant(mapid, mapv))
			c_map_variant_initialize(mapv, mapid);
	}
}

namespace
{
	const auto CACHE_FILE_HEADER_SIZE = 0x3390;
	const auto MAP_INFO_SIZE = 0x89E0;
	const auto MAPV_SIZE = 0xE0A0;

	const auto filo_create = (void*(*)(void *filo, char *filePath, bool a3))(0x00528550);
	const auto file_open = (bool(*)(void* filo, int flags, int* pLastError))(0x52A220);
	const auto file_get_size = (bool(*)(void* filo, long* pSize))(0x00529B00);
	const auto file_read = (long(*)(void* filo, long size, int a3, void* buffer))(0x52A7E0);
	const auto file_close = (bool(*)(void* filo))(0x00528B60);
	const auto file_set_position = (bool(*)(void* filo, int offset, char a3))(0x0052B060);
	const auto maps_store_map_info = (bool(*)(uint8_t* mapinfo, char a2, char *mapsPath, char a4))(0x0054A6C0);
	const auto maps_get_map_path = (void(*)(int campaignId, int mapid, char *buff, int buffLen))(0x0054C040);
	const auto maps_get_maps_path = (char*(*)())(0x00501FC0);
	const auto cache_file_valid_header = (bool(*)(uint8_t* header))(0x00501950);

	bool DeserializeAndStoreMapInfo(void* filo, char* mapsPath)
	{
		int lastError;
		long fileSize;
		bool success = false;

		// read the cache file header
		uint8_t cacheFileHeader[CACHE_FILE_HEADER_SIZE];
		if (!file_get_size(filo, &fileSize) || fileSize < CACHE_FILE_HEADER_SIZE
			|| !file_open(filo, 1, &lastError)
			|| !file_read(filo, CACHE_FILE_HEADER_SIZE, 0, cacheFileHeader) || !cache_file_valid_header(cacheFileHeader))
		{
			file_close(filo);
			return false;
		}

		uint8_t blfhead[0x30];
		if ((fileSize - 0x30) <= 0 || !file_read(filo, 0x30, 0, blfhead) || *(uint32_t*)blfhead != 'flb_')
		{
			file_close(filo);
			return false;
		}

		uint8_t mapi[MAP_INFO_SIZE];
		if ((fileSize - MAP_INFO_SIZE) >= 0 && file_read(filo, MAP_INFO_SIZE, 0, mapi))
		{
			maps_store_map_info(mapi, 0, mapsPath, 0);
			success = true;
		}

		file_close(filo);
		return success;
	}

	bool TryLoadDefaultMapVariant(int mapId, Blam::MapVariant* buffer)
	{
		// get the path to the .map file
		char mapPath[256];
		maps_get_map_path(-1, mapId, mapPath, sizeof(mapPath));
		if (strlen(mapPath) + 4 >= sizeof(mapPath))
			return false;
		strcat_s(mapPath, 256, ".map");

		// create a filo instance and attempt to open the .map
		uint8_t filo[0x220];
		filo_create(filo, mapPath, true);

		int lastError;
		long fileSize;
		bool success = false;

		const auto offset = CACHE_FILE_HEADER_SIZE + MAP_INFO_SIZE;
		// read the mapv
		uint8_t buff[MAPV_SIZE];
		if (file_get_size(filo, &fileSize) && file_open(filo, 1, &lastError)
			&& (fileSize - offset) >= MAPV_SIZE && file_set_position(filo, offset, 0)
			&& file_read(filo, MAPV_SIZE, 0, buff) && *(uint32_t*)buff == 'vpam')
		{
			memcpy((void*)buffer, buff + 0x10, sizeof(Blam::MapVariant));
			success = true;
		}

		file_close(filo);
		return success;
	}

	void __fastcall c_map_variant_initialize_default_hook(Blam::MapVariant* thisptr, void* unused, int mapId)
	{
		Patches::Maps::InitializeMapVariant(thisptr, mapId);
	}

	int EnumerateMapsHook(int a1)
	{
		const auto sub_52B4A0 = (char(__cdecl *)(int a1, void *a2, void* a3))(0x52B4A0);
		const auto sub_52B450 = (void*(__cdecl *)(int a1))(0x52B450);
		const auto sub_52B830 = (wchar_t*(__cdecl*) (int a1, int a2, void *filo))(0x52B830);
		const auto c_filo_get_file_name_part = (wchar_t *(__cdecl *)(void *filo, int flags, wchar_t *buff, int a4))(0x005286E0);
		const auto string_util_unicode_to_ascii = (void(__thiscall*)(char* dest, wchar_t* src))(0x004EC470);
		const auto maps_read_map_cache_file = (void(*)(void *fileo, char *mapsPath))(0x0054C530);
		const auto maps_read_campaign = (bool(*)(void *filo, wchar_t *path, bool a3))(0x0054C3C0);

		uint8_t filo[0x1000];
		if (*(uint32_t *)a1)
		{
			if (*(uint32_t *)a1 == 1)
			{
				if (!sub_52B4A0(*(uint32_t *)(a1 + 4), &filo, nullptr))
				{
					sub_52B450(*(uint32_t *)(a1 + 4));
					return ++*(uint32_t *)a1 == 2;
				}

				char asciiFileName[256];
				wchar_t fileName[256], extension[256];

				c_filo_get_file_name_part(filo, 12, fileName, 256);
				c_filo_get_file_name_part(filo, 1, fileName, 256); // directory
				string_util_unicode_to_ascii(asciiFileName, fileName);
				c_filo_get_file_name_part(filo, 8, extension, 256);

				if (!wcscmp(extension, L"map"))
				{
					if (!DeserializeAndStoreMapInfo(filo, asciiFileName))
						maps_read_map_cache_file(filo, asciiFileName);
				}
				else if (!wcscmp(extension, L"campaign"))
				{
					maps_read_campaign(filo, fileName, 1);
				}
			}
		}
		else
		{

			const auto mapsPath = maps_get_maps_path();
			void* mapsDirFilo = filo_create(filo, mapsPath, 1);
			sub_52B830(*(uint32_t *)(a1 + 4), 0, mapsDirFilo);
			++*(uint32_t *)a1;
		}

		return *(uint32_t *)a1 == 2;
	}
}