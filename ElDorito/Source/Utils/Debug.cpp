#include "Debug.hpp"
#include <vector>
#include <cstdint>
#include <windows.h>
#include "Logger.hpp"
#include <iphlpapi.h>

namespace Utils
{
	std::string GetStackTraceString(int framesToSkip, int framesToCapture)
	{
		std::stringstream ss;
		std::vector<uint32_t> backTrace(framesToCapture);
		int capturedFrames = CaptureStackBackTrace(framesToSkip + 1, framesToCapture, reinterpret_cast<void**>(backTrace.data()), nullptr);
		for (int i = capturedFrames - 1; i >= 0; i--)
		{
			if (i == capturedFrames - 1)
			{
				ss << "0x" << std::uppercase << std::hex << backTrace[i];
			}
			else
			{
				ss << "->" << "0x" << std::uppercase << std::hex << backTrace[i];
			}
		}
		return ss.str();
	}

	void LogEthernetAdapterInfo()
	{
		Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "GetAdaptersInfo");

		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa365917(v=vs.85).aspx
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter;
		DWORD dwRetVal = 0;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = static_cast<IP_ADAPTER_INFO *>(malloc(sizeof(IP_ADAPTER_INFO)));
		if (pAdapterInfo == nullptr)
		{
			Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "Error allocating memory needed to call GetAdaptersinfo");
			return;
		}

		// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			free(pAdapterInfo);
			pAdapterInfo = static_cast<IP_ADAPTER_INFO *>(malloc(ulOutBufLen));
			if (pAdapterInfo == nullptr)
			{
				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "Error allocating memory needed to call GetAdaptersinfo");
				return;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
		{
			pAdapter = pAdapterInfo;
			while (pAdapter)
			{
				// don't bother listing adapters without address information
				if (std::string(pAdapter->IpAddressList.IpAddress.String).find("0.0.0.0") != std::string::npos)
				{
					pAdapter = pAdapter->Next;
					continue;
				}

				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tAdapter Desc: \t%s", pAdapter->Description);
				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tMAC Address: \t%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);

				switch (pAdapter->Type)
				{
				case MIB_IF_TYPE_ETHERNET:
					Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tAdapter Type: \tEthernet");
					break;
				case IF_TYPE_IEEE80211:
					Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tAdapter Type: \tWireless Ethernet");
					break;
				default:
					Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tAdapter Type: \tOther type %ld", pAdapter->Type);
					break;
				}

				// prints the first (and very likely only) entry in each list
				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tIP Address: \t%s", pAdapter->IpAddressList.IpAddress.String);
				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tIP Mask: \t%s", pAdapter->IpAddressList.IpMask.String);
				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "\tGateway: \t%s", pAdapter->GatewayList.IpAddress.String);
				Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "");

				pAdapter = pAdapter->Next;
			}
		}
		else
		{
			Logger::Instance().Log(LogTypes::Network, LogLevel::Info, "GetAdaptersInfo failed with error: %d", dwRetVal);
		}

		if (pAdapterInfo)
		{
			free(pAdapterInfo);
		}
	}
}
