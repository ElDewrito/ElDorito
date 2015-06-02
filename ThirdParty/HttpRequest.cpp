#include "HttpRequest.h"

HttpRequest::HttpRequest(const std::wstring &userAgent, const std::wstring &proxyIp, const std::wstring &proxyPort) :
_userAgent(userAgent)
//,_proxyIp(proxyIp)
//,_proxyPort(proxyPort)
{
}

BOOL HttpRequest::SendRequest(const std::wstring &url, INTERNET_PORT port, const std::wstring &method, const std::wstring &username, const std::wstring &password, void *body, DWORD bodySize)
{
	DWORD dwSize;
	DWORD dwDownloaded;
	DWORD headerSize = 0;
	BOOL  bResults = FALSE;
	HINTERNET hSession;
	HINTERNET hConnect;
	HINTERNET hRequest;

	responseHeader.resize(0);
	responseBody.resize(0);

	DWORD dwAccessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
	LPCWSTR pszProxyW = WINHTTP_NO_PROXY_NAME;
	LPCWSTR pszProxyBypassW = WINHTTP_NO_PROXY_BYPASS;
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG iecfg;
	if (WinHttpGetIEProxyConfigForCurrentUser(&iecfg))
	{
		if (iecfg.fAutoDetect)
		{
			hSession = WinHttpOpen(_userAgent.c_str(), WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!hSession)
			{
				lastError = 80001;
				return false;
			}
			WINHTTP_AUTOPROXY_OPTIONS proxy;
			proxy.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
			proxy.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
			proxy.fAutoLogonIfChallenged = true;
			std::wstring fullUrl = L"http://" + url + L":" + std::to_wstring(port);
			WINHTTP_PROXY_INFO info;
			if (WinHttpGetProxyForUrl(hSession, fullUrl.c_str(), &proxy, &info))
			{
				pszProxyW = info.lpszProxy;
				pszProxyBypassW = info.lpszProxyBypass;
				dwAccessType = info.dwAccessType;
			}
		}
		else
		{
			dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			pszProxyW = iecfg.lpszProxy;
			pszProxyBypassW = iecfg.lpszProxyBypass;
		}
	}
	if (pszProxyW == 0 || wcslen(pszProxyW) <= 0)
	{
		// no proxy server set
		dwAccessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
		pszProxyW = WINHTTP_NO_PROXY_NAME;
		pszProxyBypassW = WINHTTP_NO_PROXY_BYPASS;
	}

	hSession = WinHttpOpen(_userAgent.c_str(), dwAccessType, pszProxyW, pszProxyBypassW, 0);
	if (!hSession)
	{
		lastError = 80002;
		return false;
	}

	hConnect = WinHttpConnect(hSession, url.c_str(), port, 0);
	if (!hConnect)
	{
		lastError = 80003;
		return false;
	}

	hRequest = WinHttpOpenRequest(hConnect, method.c_str(), NULL, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

	if (!username.empty() || !password.empty())
	{
		if (!WinHttpSetCredentials(hRequest, WINHTTP_AUTH_TARGET_SERVER, WINHTTP_AUTH_SCHEME_BASIC, username.c_str(), password.c_str(), 0))
		{
			lastError = 80004;
			return false;
		}
	}

	if (!hRequest)
	{
		lastError = 80005;
		return false;
	}

	bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, body, bodySize, 0, 0);

	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults)
	{
		bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, NULL, WINHTTP_NO_OUTPUT_BUFFER, &headerSize, WINHTTP_NO_HEADER_INDEX);
		if ((!bResults) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
		{
			responseHeader.resize(headerSize / sizeof(wchar_t));
			if (responseHeader.empty())
			{
				bResults = TRUE;
			}
			else
			{
				bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, NULL, &responseHeader[0], &headerSize, WINHTTP_NO_HEADER_INDEX);
				if (!bResults) headerSize = 0;
				responseHeader.resize(headerSize / sizeof(wchar_t));
			}
		}
	}
	if (bResults)
	{
		do
		{
			// Check for available data.
			dwSize = 0;
			bResults = WinHttpQueryDataAvailable(hRequest, &dwSize);
			if (!bResults)
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (dwSize == 0)
				break;

			do
			{
				// Allocate space for the buffer.
				DWORD dwOffset = responseBody.size();
				responseBody.resize(dwOffset + dwSize);

				// Read the data.
				bResults = WinHttpReadData(hRequest, &responseBody[dwOffset], dwSize, &dwDownloaded);
				if (!bResults)
				{
					printf("Error %u in WinHttpReadData.\n", GetLastError());
					dwDownloaded = 0;
				}

				responseBody.resize(dwOffset + dwDownloaded);

				if (dwDownloaded == 0)
					break;

				dwSize -= dwDownloaded;
			} while (dwSize > 0);
		} while (true);
	}

	if (!bResults)
		lastError = GetLastError();

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return bResults;
}