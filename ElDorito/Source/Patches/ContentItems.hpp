#pragma once
#include <string>

namespace Patches::ContentItems
{
	void ApplyAll();
	void GetFilePathForMap(std::wstring name, wchar_t *path);
	void GetFilePathForMap(std::wstring name, wchar_t *path, bool bIsDownload);
	void GetFilePathForGameVariant(std::wstring name, wchar_t *path, int variantType);
	void GetFilePathForGameVariant(std::wstring name, wchar_t *path, int variantType, bool bIsDownload);
	bool LoadBLF(wchar_t* itemPath);
}
