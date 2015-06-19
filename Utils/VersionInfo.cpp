#include "VersionInfo.h"

#include <windows.h>

static HMODULE s_versionModule = NULL;

namespace Utils
{
	namespace Version
	{
		DWORD GetVersionInt()
		{
			DWORD retVer;

			HRSRC hVersion = FindResource(s_versionModule,
				MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
			if (hVersion != NULL)
			{
				HGLOBAL hGlobal = LoadResource(s_versionModule, hVersion);
				if (hGlobal != NULL)
				{
					LPVOID versionInfo = LockResource(hGlobal);

					if (versionInfo != NULL)
					{
						DWORD versionSize = SizeofResource(s_versionModule, hVersion);
						LPVOID versionCopy = LocalAlloc(LMEM_FIXED, versionSize);
						CopyMemory(versionCopy, versionInfo, versionSize);
						FreeResource(versionInfo);

						DWORD vLen;
						BOOL retVal;

						LPVOID retbuf = NULL;

						retVal = VerQueryValue(versionCopy, "\\", &retbuf, (UINT *)&vLen);
						if (retVal && vLen == sizeof(VS_FIXEDFILEINFO))
						{
							VS_FIXEDFILEINFO* ffInfo = (VS_FIXEDFILEINFO*)retbuf;
							DWORD majorVer = HIWORD(ffInfo->dwFileVersionMS);
							DWORD minorVer = LOWORD(ffInfo->dwFileVersionMS);
							DWORD buildNum = HIWORD(ffInfo->dwFileVersionLS);
							DWORD buildQfe = LOWORD(ffInfo->dwFileVersionLS);

							retVer =
								((majorVer & 0xFF) << 24) |
								((minorVer & 0xFF) << 16) |
								((buildNum & 0xFF) << 8) |
								(buildQfe & 0xFF);
						}

						LocalFree(versionCopy);
					}
				}

				UnlockResource(hGlobal);
				FreeResource(hGlobal);
			}

			return retVer;
		}

		std::string GetVersionString()
		{
			static std::string versionStr;

			if (versionStr.length() == 0)
			{
				char version[256];
				memset(version, 0, 256);
				DWORD versionInt = GetVersionInt();
				sprintf_s(version, 256, "%d.%d.%d.%d", ((versionInt >> 24) & 0xFF), ((versionInt >> 16) & 0xFF), ((versionInt >> 8) & 0xFF), (versionInt & 0xFF));
#ifdef _DEBUG
				versionStr = std::string(version) + "-debug";
#else
				versionStr = std::string(version);
#endif
			}
			return versionStr;
		}

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

						retVal = VerQueryValue(versionCopy, "\\VarFileInfo\\Translation", &retbuf, (UINT *)&vLen);
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

						LocalFree(versionCopy);
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