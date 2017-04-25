#include "HttpRequest.hpp"
#include <sstream>

HttpRequest::HttpRequest(const std::wstring &userAgent, const std::wstring &proxyIp, const std::wstring &proxyPort) :
_userAgent(userAgent)
//,_proxyIp(proxyIp)
//,_proxyPort(proxyPort)
{
}

BOOL HttpRequest::SendRequest(const std::wstring &uri, const std::wstring &method, const std::wstring &username, const std::wstring &password, const std::wstring &headers, void *body, DWORD bodySize)
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

	URL_COMPONENTSW urlComp = {};
	urlComp.dwStructSize = sizeof(urlComp);

	// Set required component lengths to non-zero so that they are cracked.
	urlComp.dwSchemeLength = static_cast<DWORD>(-1);
	urlComp.dwHostNameLength = static_cast<DWORD>(-1);
	urlComp.dwUrlPathLength = static_cast<DWORD>(-1);

	if (!WinHttpCrackUrl(uri.c_str(), uri.length(), 0, &urlComp))
		return FALSE;

	std::wstring scheme;
	std::wstring hostname;
	std::wstring path;
	scheme.resize(urlComp.dwSchemeLength);
	hostname.resize(urlComp.dwHostNameLength);
	path.resize(urlComp.dwUrlPathLength);
	if (!urlComp.lpszScheme)
		scheme = L"http";
	else
		memcpy((void*)scheme.data(), urlComp.lpszScheme, urlComp.dwSchemeLength * sizeof(wchar_t));

	if (!urlComp.lpszHostName)
		hostname = L"localhost";
	else
		memcpy((void*)hostname.data(), urlComp.lpszHostName, urlComp.dwHostNameLength * sizeof(wchar_t));

	if (!urlComp.lpszUrlPath)
		path = L"/";
	else
		memcpy((void*)path.data(), urlComp.lpszUrlPath, urlComp.dwUrlPathLength * sizeof(wchar_t));
	
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
			std::wstring fullUrl = scheme + L"://" + hostname + L":" + std::to_wstring(urlComp.nPort);
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

	WinHttpSetTimeouts(hSession, 5 * 1000, 5 * 1000, 5 * 1000, 5 * 1000);

	hConnect = WinHttpConnect(hSession, hostname.c_str(), urlComp.nPort, 0);
	if (!hConnect)
	{
		lastError = 80003;
		return false;
	}

	hRequest = WinHttpOpenRequest(hConnect, method.c_str(), path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

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
	LPCWSTR addtHdrs = WINHTTP_NO_ADDITIONAL_HEADERS;
	DWORD length = 0;
	if (headers.length() > 0)
	{
		addtHdrs = headers.c_str();
		length = (DWORD)-1;
	}

	bResults = WinHttpSendRequest(hRequest, addtHdrs, length, body, bodySize, bodySize, 0);

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
				std::stringstream ss;
				ss << "Error " << GetLastError() << "in WinHttpQueryDataAvailable.";
				OutputDebugString(ss.str().c_str());
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
					std::stringstream ss;
					ss << "Error " << GetLastError() << " in WinHttpReadData.";
					OutputDebugString(ss.str().c_str());
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