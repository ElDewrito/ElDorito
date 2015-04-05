#include "VersionInfo.h"

#include <windows.h>

static HMODULE s_versionModule = NULL;

namespace Utils
{
	namespace Version
	{
		std::string GetInfo(const std::string &csEntry)
		{
			std::string csRet;

			HRSRC hVersion = FindResource(s_versionModule,
				MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
			if( hVersion != NULL )
			{
				HGLOBAL hGlobal = LoadResource(s_versionModule, hVersion);
				if( hGlobal != NULL )
				{
					LPVOID versionInfo = LockResource(hGlobal);

					if( versionInfo != NULL )
					{
						DWORD versionSize = SizeofResource(s_versionModule, hVersion);
						LPVOID versionCopy = LocalAlloc(LMEM_FIXED, versionSize);
						CopyMemory(versionCopy, versionInfo, versionSize);
						FreeResource(versionInfo);

						DWORD vLen, langD;
						BOOL retVal;

						LPVOID retbuf = NULL;

						static char fileEntry[256];

						sprintf_s(fileEntry, "\\VarFileInfo\\Translation");
						retVal = VerQueryValue(versionCopy, fileEntry, &retbuf, (UINT *)&vLen);
						if( retVal && vLen == 4 )
						{
							memcpy(&langD, retbuf, 4);
							sprintf_s(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
								(langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24,
								(langD & 0xff0000) >> 16, csEntry.c_str());
						}
						else
						{
							unsigned long lang = GetUserDefaultLangID();
							sprintf_s(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", lang, csEntry.c_str());
						}

						if( VerQueryValue(versionCopy, fileEntry, &retbuf, (UINT *)&vLen) )
						{
							strcpy_s(fileEntry, (const char*)retbuf);
							csRet = fileEntry;
						}
					}
				}

				UnlockResource(hGlobal);
				FreeResource(hGlobal);
			}

			return csRet;
		}

		void SetModule(HMODULE module)
		{
			s_versionModule = module;
		}
	}
}