#include <windows.h>
#include <IPTypes.h> // for proxy
#include "Utils/VersionInfo.hpp"
#include "ElDorito.hpp"
#include "ElPatches.hpp"

/*LONG WINAPI TopLevelExceptionHandler(unsigned int code, EXCEPTION_POINTERS *pExceptionInfo)
{
	const char *error_type = "Unknown";
	switch( code )
	{
	case EXCEPTION_ACCESS_VIOLATION:
		error_type = "EXCEPTION_ACCESS_VIOLATION";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		error_type = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		break;
	case EXCEPTION_BREAKPOINT:
		error_type = "EXCEPTION_BREAKPOINT";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		error_type = "EXCEPTION_DATATYPE_MISALIGNMENT";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		error_type = "EXCEPTION_FLT_DENORMAL_OPERAND";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		error_type = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		error_type = "EXCEPTION_FLT_INEXACT_RESULT";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		error_type = "EXCEPTION_FLT_INVALID_OPERATION";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		error_type = "EXCEPTION_FLT_OVERFLOW";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		error_type = "EXCEPTION_FLT_STACK_CHECK";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		error_type = "EXCEPTION_FLT_UNDERFLOW";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		error_type = "EXCEPTION_ILLEGAL_INSTRUCTION";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		error_type = "EXCEPTION_IN_PAGE_ERROR";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		error_type = "EXCEPTION_INT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_INT_OVERFLOW:
		error_type = "EXCEPTION_INT_OVERFLOW";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		error_type = "EXCEPTION_INVALID_DISPOSITION";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		error_type = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		error_type = "EXCEPTION_PRIV_INSTRUCTION";
		break;
	case EXCEPTION_SINGLE_STEP:
		error_type = "EXCEPTION_SINGLE_STEP";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		error_type = "EXCEPTION_STACK_OVERFLOW";
		break;
	}

	// Format exception string
	std::cout << "Exception: 0x" << std::uppercase << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << std::nouppercase << " - " << error_type << std::endl;

	return EXCEPTION_CONTINUE_SEARCH;
}*/

bool CanAccessFolder(LPCTSTR folderName, DWORD genericAccessRights)
{
	bool bRet = false;
	DWORD length = 0;
	if (!::GetFileSecurity(folderName, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
		| DACL_SECURITY_INFORMATION, NULL, NULL, &length) &&
		ERROR_INSUFFICIENT_BUFFER == ::GetLastError()) {
		PSECURITY_DESCRIPTOR security = static_cast< PSECURITY_DESCRIPTOR >(::malloc(length));
		if (security && ::GetFileSecurity(folderName, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
			| DACL_SECURITY_INFORMATION, security, length, &length)) {
			HANDLE hToken = NULL;
			if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_IMPERSONATE | TOKEN_QUERY |
				TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, &hToken)) {
				HANDLE hImpersonatedToken = NULL;
				if (::DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken)) {
					GENERIC_MAPPING mapping = { 0xFFFFFFFF };
					PRIVILEGE_SET privileges = { 0 };
					DWORD grantedAccess = 0, privilegesLength = sizeof(privileges);
					BOOL result = FALSE;

					mapping.GenericRead = FILE_GENERIC_READ;
					mapping.GenericWrite = FILE_GENERIC_WRITE;
					mapping.GenericExecute = FILE_GENERIC_EXECUTE;
					mapping.GenericAll = FILE_ALL_ACCESS;

					::MapGenericMask(&genericAccessRights, &mapping);
					if (::AccessCheck(security, hImpersonatedToken, genericAccessRights,
						&mapping, &privileges, &privilegesLength, &grantedAccess, &result)) {
						bRet = (result == TRUE);
					}
					::CloseHandle(hImpersonatedToken);
				}
				::CloseHandle(hToken);
			}
			::free(security);
		}
	}

	return bRet;
}

bool relaunch = false;

BOOL InitInstance(HINSTANCE hModule)
{
	//Disable Windows DPI scaling
	SetProcessDPIAware();

	//Make sure our working directory is in the ED root folder
	char appPath[MAX_PATH];
	GetModuleFileNameA(hModule, appPath, MAX_PATH);
	std::string EDPath = std::string(appPath);
	EDPath = EDPath.substr(0, EDPath.find_last_of("\\"));
	SetCurrentDirectoryA(EDPath.c_str());

	//Check for read/write priveledges in the current directory
	if (!CanAccessFolder(".", GENERIC_READ | GENERIC_WRITE))
	{
		MessageBox(NULL, "Invalid permissions for the current directory, your game will be relaunched as Administrator.\nPlease set eldorado.exe to run as administrator or move your game to a different directory to avoid this message in the future.", "Invalid Directory Permissions", MB_OK | MB_ICONERROR);
		relaunch = true;
		return true;
	}

	DisableThreadLibraryCalls(hModule);

	ElDorito::SetMainThreadID(GetCurrentThreadId());

	Patches::ApplyRequired();
	ElDorito::Instance().Initialize();

	Utils::LogEthernetAdapterInfo();

	return true;
}

BOOL ExitInstance()
{
	return true;
}

// TODO: remove this export, removing it needs us to use a new mtndew-enabled eldorado.exe though
DWORD GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
	return 0;
}

int GetEDVersion()
{
	return Utils::Version::GetVersionInt();
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD Reason, LPVOID Misc)
{
	switch( Reason )
	{
	case DLL_PROCESS_ATTACH: return InitInstance(hModule);
	case DLL_PROCESS_DETACH: return ExitInstance();
	case DLL_THREAD_ATTACH:
		return true;
	case DLL_THREAD_DETACH:
		//Relaunch here to avoid the loader lock
		if (relaunch)
		{
			char *cmd = GetCommandLine();
			int offset = 0;
			//Seperate the *.exe from the arguments
			if (cmd[offset] == '"')
				while (cmd[++offset] != '"');
			while (cmd[++offset] != ' ');

			//Relaunch the game with the same arguments
			auto str = static_cast<std::string>(GetCommandLine());
			ShellExecuteA(nullptr, "runas", str.substr(0, offset).c_str(), str.substr(offset).c_str(), nullptr, SW_SHOWNORMAL);

			//Close the current instance
			std::exit(0);
		}
		return true;
	}

	return false;
}