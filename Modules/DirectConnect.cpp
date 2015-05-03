#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "DirectConnect.h"
#include "../ElDorito.h"
#include <iostream>

DirectConnect::DirectConnect()
{
}

DirectConnect::~DirectConnect()
{
}

std::string DirectConnect::Info(const std::string& Topic) const
{
	std::string Info = "Direct connect: Initiates a direct connection to a remote server";
	Info += "\nUsage: connect (ip[:port]) (xnkid) (xnaddr)\n"
		"Begins a connection to the remote server, xnkid and xnaddr must match up with the servers values.";

	return Info;
}

std::string DirectConnect::Suggest(const std::vector<std::string>& Arguments) const
{
	return "";
}

void DirectConnect::Tick(const std::chrono::duration<double>& Delta)
{
}

bool DirectConnect::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		std::string address = Args[1];
		std::string xnkid = "11223344556677889900112233445566";
		std::string xnaddr = "11223344556677889900112233445566";

		if (Args.size() >= 3)
			xnkid = Args[2];

		if (Args.size() >= 4)
			xnaddr = Args[3];

		BYTE rawXnkid[0x10];
		BYTE rawXnaddr[0x10];
		BYTE rawIpaddr[0x4];
		uint16_t rawPort = 11774;
		memset(rawXnkid, 0, 0x10);
		memset(rawXnaddr, 0, 0x10);
		memset(rawIpaddr, 0, 4);

		size_t portOffset = address.find(':');
		std::string host = address;
		if (portOffset != std::string::npos && portOffset + 1 < address.size())
		{
			std::string port = address.substr(portOffset + 1);
			rawPort = (uint16_t)atoi(port.c_str());
			host = address.substr(0, portOffset);
		}

		struct addrinfo* info = NULL;
		INT retval = getaddrinfo(host.c_str(), "11774", NULL, &info);
		if (retval != 0)
		{
			int error = WSAGetLastError();
			std::cout << "Unable to lookup " << address << " (" << retval << "): ";
			if (error != 0)
			{
				if (error == WSAHOST_NOT_FOUND)
					std::cout << "Host not found.\n" << std::endl;
				else if (error == WSANO_DATA)
					std::cout << "No data record found.\n" << std::endl;
				else
					std::cout << "Function failed with error " << error << ".\n" << std::endl;
			}
			else
				std::cout << "Unknown error.\n" << std::endl;
			return true;
		}
		struct addrinfo *ptr = NULL;
		bool foundAddr = false;
		for (ptr = info; ptr != NULL; ptr = ptr->ai_next)
		{
			if (ptr->ai_family != AF_INET)
				continue; // not ipv4

			BYTE flippedIpaddr[0x4];
			memcpy(flippedIpaddr, &((sockaddr_in*)ptr->ai_addr)->sin_addr, 4);
			rawIpaddr[3] = flippedIpaddr[0];
			rawIpaddr[2] = flippedIpaddr[1];
			rawIpaddr[1] = flippedIpaddr[2];
			rawIpaddr[0] = flippedIpaddr[3];

			foundAddr = true;
			break;
		}
		if (!foundAddr)
		{
			std::cout << "Unable to lookup " << address << ": No records found.\n" << std::endl;
			return true;
		}
		Utils::String::HexStringToBytes(xnkid, rawXnkid, 0x10);
		Utils::String::HexStringToBytes(xnaddr, rawXnaddr, 0x10);

		static BYTE syslinkData[0x176];
		memset(syslinkData, 0, 0x176);

		*(uint32_t*)syslinkData = 1;

		memcpy(syslinkData + 0x9E, rawXnkid, 0x10);
		memcpy(syslinkData + 0xAE, rawXnaddr, 0x10);

		memcpy(syslinkData + 0x170, rawIpaddr, 0x4);

		*(uint16_t*)(syslinkData + 0x174) = rawPort;

		Pointer::Base(0x1E8E6D8).Write<uint32_t>(1);
		Pointer::Base(0x1E8E6DC).Write<uint32_t>((uint32_t)&syslinkData);

		Pointer::Base(0x1E40BA8).Write<int64_t>(-1);
		Pointer::Base(0x1E40BB0).Write<uint32_t>(1);
		Pointer::Base(0x1E40BB4).Write(rawXnkid, 0x10);
		Pointer::Base(0x1E40BD4).Write(rawXnaddr, 0x10);
		Pointer::Base(0x1E40BE4).Write<uint32_t>(1);

		std::cout << "Attempting connection to " << address << "...\n" << std::endl;
		return true;
	}

	return false;
}

void DirectConnect::PreferencesChanged()
{
}