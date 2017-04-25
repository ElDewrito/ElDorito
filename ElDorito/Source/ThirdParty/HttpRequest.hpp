#include <windows.h>
#include <winhttp.h>
#include <string>
#include <vector>

#pragma comment(lib, "winhttp.lib")

class HttpRequest
{
private:
	std::wstring _userAgent;
	//std::wstring _proxyIp;
	//std::wstring _proxyPort;
public:
	int lastError;

	HttpRequest(const std::wstring &userAgent, const std::wstring &proxyIp, const std::wstring &proxyPort);
	BOOL SendRequest(const std::wstring &uri, const std::wstring &method, const std::wstring &username, const std::wstring &password, const std::wstring &headers, void *body, DWORD bodySize);
	std::wstring responseHeader;
	std::vector<BYTE> responseBody;
};