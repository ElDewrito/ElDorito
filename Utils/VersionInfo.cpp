#include "VersionInfo.h"

#include <windows.h>

namespace Utils
{
  namespace Version
  {
    std::string GetInfo(const std::string &csEntry)
    {
      std::string csRet;

      HMODULE hMod = GetModuleHandle("iphlpapi.dll");

      HRSRC hVersion = FindResource(hMod,
        MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
      if(hVersion != NULL)
      {
        HGLOBAL hGlobal = LoadResource(hMod, hVersion);
        if(hGlobal != NULL)
        {

          LPVOID versionInfo = LockResource(hGlobal);
          if(versionInfo != NULL)
          {
            DWORD vLen, langD;
            BOOL retVal;

            LPVOID retbuf = NULL;

            static char fileEntry[256];

            sprintf_s(fileEntry, "\\VarFileInfo\\Translation");
            retVal = VerQueryValue(versionInfo, fileEntry, &retbuf, (UINT *)&vLen);
            if(retVal && vLen == 4)
            {
              memcpy(&langD, retbuf, 4);
              sprintf_s(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
                (langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24,
                (langD & 0xff0000) >> 16, csEntry.c_str());
            }
            else
              sprintf_s(fileEntry, "\\StringFileInfo\\%04X04B0\\%s",
              GetUserDefaultLangID(), csEntry.c_str());

            if(VerQueryValue(versionInfo, fileEntry, &retbuf, (UINT *)&vLen))
              csRet = (char*)retbuf;
          }
        }

        UnlockResource(hGlobal);
        FreeResource(hGlobal);
      }

      return csRet;
    }

  }
}
