#include "Maps.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamTypes.hpp"

namespace
{
	void __fastcall c_map_variant_initialize_default_hook(Blam::MapVariant* thisptr, void* unused, int mapId);
	int EnumerateMapsHook(int a1);

	const auto CACHE_FILE_HEADER_SIZE = 0x3390;
	const auto CACHE_FILE_MAP_DATA_SIZE = 0x12E30; // ugh
	
	const auto cache_file_valid_header = (bool(*)(uint8_t* header))(0x00501950);

	const auto filo_create = (void*(*)(void *filo, char *filePath, bool a3))(0x00528550);
	const auto file_open = (bool(*)(void* filo, int flags, int* pLastError))(0x52A220);
	const auto file_get_size = (bool(*)(void* filo, long* pSize))(0x00529B00);
	const auto file_read = (long(*)(void* filo, long size, int a3, void* buffer))(0x52A7E0);
	const auto file_close = (bool(*)(void* filo))(0x00528B60);
	const auto file_set_position = (bool(*)(void* filo, int offset, char a3))(0x0052B060);
	const auto blf_file_header_valid = (bool(*)(uint8_t* buff, int size, bool* a3, bool* success))(0x00463540);
	const auto blf_file_read_tag = (bool(*)(uint8_t* buff, int size, int a3, uint32_t signature, int a4,
		int* pSize, uint8_t** pData, int a7, int a8, bool*success))(0x004630D0);
	const auto maps_store_map_info = (bool(*)(uint8_t* mapinfo, char a2, char *mapsPath, char a4))(0x0054A6C0);
	static auto maps_get_map_path = (void(*)(int campaignId, int mapid, char *buff, int buffLen))(0x0054C040);
	const auto maps_get_maps_path = (char*(*)())(0x00501FC0);
	const auto c_map_variant_copy = (void(__thiscall*)(Blam::MapVariant *thisptr, Blam::MapVariant *buff))(0x005860F0);
	const auto c_map_variant_initialize = (void(__thiscall *)(Blam::MapVariant* thisptr, int mapId))(0x00581F70);

	bool TryLoadDefaultMapVariant(int mapId, Blam::MapVariant* buffer);
}

namespace Patches::Maps
{
	void ApplyAll()
	{
		Pointer(0x0054AD9B + 1).Write(uint32_t(&EnumerateMapsHook));
		Hook(0x6A81CF, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
		Hook(0x6A7954, c_map_variant_initialize_default_hook, HookFlags::IsCall).Apply();
	}

	void InitializeMapVariant(Blam::MapVariant *mapv, int mapid)
	{
		// if we can't load the variant in the .map file, default to one generated from the scenario
		if (!TryLoadDefaultMapVariant(mapid, mapv))
			c_map_variant_initialize(mapv, mapid);
	}
}

namespace
{
	bool ReadCacheFileHeader(void* filo, uint8_t* buff)
	{
		long fileSize;
		if (!file_get_size(filo, &fileSize)
			|| fileSize < CACHE_FILE_HEADER_SIZE
			|| !file_read(filo, CACHE_FILE_HEADER_SIZE, 0, buff)
			|| !cache_file_valid_header(buff))
		{
			return false;
		}
		return true;
	}

	bool FindBlfTag(void* filo, uint32_t signature, uint8_t* buff, int buffSize, int offset, uint8_t** ppData)
	{
		int tagSize = 0;
		bool byteswap = true;
		uint8_t* dataStart;
		bool eof;
		bool success = false;

		if (blf_file_header_valid(buff, buffSize, &byteswap, (bool *)&tagSize))
		{
			uint8_t* p = buff + tagSize;
			if (blf_file_read_tag(p, buffSize - tagSize, (int)&byteswap, signature, offset, &tagSize, &dataStart, 0, 0, &eof))
			{
				while (!eof)
				{
					if (dataStart)
					{
						*ppData = dataStart;
						success = true;
					}

					p += tagSize;
					if (!blf_file_read_tag(p, (buff - p) + buffSize, byteswap, signature, offset, &tagSize, &dataStart, 0, 0, &eof))
						break;
				}
			}
		}
		return success;
	}

	void DeserializeAndStoreMapInfo(void* filo, char* mapsPath)
	{
		int lastError;
		long fileSize;

		uint8_t cacheFileHeader[CACHE_FILE_HEADER_SIZE], mapInfo[0x98C0] = { 0 };

		// read the cache file header
		if (!file_get_size(filo, &fileSize) || fileSize < (CACHE_FILE_HEADER_SIZE + CACHE_FILE_MAP_DATA_SIZE)
			|| !file_open(filo, 1, &lastError)
			|| !file_read(filo, CACHE_FILE_HEADER_SIZE, 0, cacheFileHeader) || !cache_file_valid_header(cacheFileHeader))
		{
			file_close(filo);
			return;
		}

		// find the mapinfo
		uint8_t buff[CACHE_FILE_MAP_DATA_SIZE];
		if (file_read(filo, sizeof(buff), 0, buff))
		{
			uint8_t* mapInfoStart;
			if (FindBlfTag(filo, 'levl', buff, sizeof(buff), 0, &mapInfoStart))
				maps_store_map_info(mapInfoStart - 0xC, 0, mapsPath, 0);
		}

		file_close(filo);
	}

	bool TryLoadDefaultMapVariant(int mapId, Blam::MapVariant* buffer)
	{
		// get the path to the .map file
		char mapPath[256];
		maps_get_map_path(-1, mapId, mapPath, sizeof(mapPath));
		if (strlen(mapPath) + 4 >= sizeof(mapPath))
			return false;
		strcat_s(mapPath, 256, ".map");

		int lastError;
		long fileSize;
		uint8_t cacheFileHeader[CACHE_FILE_HEADER_SIZE];

		// create a filo instance and attempt to open the .map
		uint8_t filo[0x220];
		filo_create(filo, mapPath, true);

		// read the cache file header
		if (!file_get_size(filo, &fileSize) || fileSize < CACHE_FILE_HEADER_SIZE
			|| !file_open(filo, 1, &lastError)
			|| !file_read(filo, CACHE_FILE_HEADER_SIZE, 0, cacheFileHeader) || !cache_file_valid_header(cacheFileHeader))
		{
			file_close(filo);
			return false;
		}

		// find the mapv and copy into the supplied buffer
		bool success = false;
		uint8_t buff[CACHE_FILE_MAP_DATA_SIZE];
		if (file_read(filo, sizeof(buff), 0, buff))
		{
			uint8_t* mapvStart;
			if (FindBlfTag(filo, 'mapv', buff, sizeof(buff), 0xC, &mapvStart))
			{
				c_map_variant_copy((Blam::MapVariant*)(mapvStart + 4), buffer);
				success = true;
			}
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
					DeserializeAndStoreMapInfo(filo, asciiFileName);
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