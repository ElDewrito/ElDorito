#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "DirectConnect.h"
#include "../ElDorito.h"
#include "../ThirdParty/SHA256.h"
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
	Info += "\nUsage: connect (ip[:port]) [password]\n"
		"Begins a connection to the remote server, if server uses a password then password field must be set.";

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
		std::string password = "";

		if (Args.size() >= 3)
			password = Args[2];

		BYTE passwordHash[0x20];
		uint32_t rawIpaddr = 0;
		uint16_t rawPort = 11774;
		memset(passwordHash, 0x11, 0x20);

		size_t portOffset = address.find(':');
		std::string host = address;
		if (portOffset != std::string::npos && portOffset + 1 < address.size())
		{
			std::string port = address.substr(portOffset + 1);
			rawPort = (uint16_t)atoi(port.c_str());
			host = address.substr(0, portOffset);
		}

		struct addrinfo* info = NULL;
		INT retval = getaddrinfo(host.c_str(), NULL, NULL, &info);
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
		for (ptr = info; ptr != NULL; ptr = ptr->ai_next)
		{
			if (ptr->ai_family != AF_INET)
				continue; // not ipv4

			rawIpaddr = ntohl(((sockaddr_in*)ptr->ai_addr)->sin_addr.S_un.S_addr);
			break;
		}
		if (!rawIpaddr)
		{
			std::cout << "Unable to lookup " << address << ": No records found.\n" << std::endl;
			return true;
		}

		if (password.length() > 0)
		{
			SHA256_CTX ctx;
			sha256_init(&ctx);
			sha256_update(&ctx, (unsigned char*)password.c_str(), password.length());
			sha256_final(&ctx, passwordHash);
		}

		// set up our syslink data struct
		memset(syslinkData, 0, 0x176);
		*(uint32_t*)syslinkData = 1;

		memcpy(syslinkData + 0x9E, passwordHash, 0x20);

		*(uint32_t*)(syslinkData + 0x170) = rawIpaddr;
		*(uint16_t*)(syslinkData + 0x174) = rawPort;

		// set syslink stuff to point at our syslink data
		Pointer::Base(0x1E8E6D8).Write<uint32_t>(1);
		Pointer::Base(0x1E8E6DC).Write<uint32_t>((uint32_t)&syslinkData);

		// tell the game to start joining
		Pointer::Base(0x1E40BA8).Write<int64_t>(-1);
		Pointer::Base(0x1E40BB0).Write<uint32_t>(1);
		Pointer::Base(0x1E40BB4).Write(passwordHash, 0x10);
		Pointer::Base(0x1E40BD4).Write(passwordHash + 0x10, 0x10);
		Pointer::Base(0x1E40BE4).Write<uint32_t>(1);

		std::cout << "Attempting connection to " << address << "...\n" << std::endl;
		return true;
	}

	return false;
}

void DirectConnect::PreferencesChanged()
{
}