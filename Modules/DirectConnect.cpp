#include "DirectConnect.h"

#include "../ElDorito.h"

#include <Windows.h>
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

		uint32_t ipAddr[0x4];
		sscanf_s(address.c_str(), "%d.%d.%d.%d", &ipAddr[3], &ipAddr[2], &ipAddr[1], &ipAddr[0]);
		for (int i = 3; i >= 0; i--)
			rawIpaddr[i] = (BYTE)ipAddr[i];

		if (portOffset != std::string::npos && portOffset + 1 < address.size())
		{
			std::string port = address.substr(portOffset + 1);
			rawPort = (uint16_t)atoi(port.c_str());
		}

		Utils::String::HexStringToBytes(xnkid, rawXnkid, 0x10);
		Utils::String::HexStringToBytes(xnaddr, rawXnaddr, 0x10);

		static BYTE syslinkData[0x176];
		memset(syslinkData, 0, 0x176);

		*(uint32_t*)syslinkData = 1;

		memcpy(syslinkData + 0x9E, rawXnkid, 0x10);
		memcpy(syslinkData + 0xAE, rawXnaddr, 0x10);

		memcpy(syslinkData + 0x170, rawIpaddr, 0x4);

		*(uint16_t*)(syslinkData + 0x174) = 11774;

		Pointer::Base(0x1E8E6D8).Write<uint32_t>(1);
		Pointer::Base(0x1E8E6DC).Write<uint32_t>((uint32_t)&syslinkData);

		Pointer::Base(0x1E40BA8).Write<int64_t>(-1);
		Pointer::Base(0x1E40BB0).Write<uint32_t>(1);
		Pointer::Base(0x1E40BB4).Write(rawXnkid, 0x10);
		Pointer::Base(0x1E40BD4).Write(rawXnaddr, 0x10);
		Pointer::Base(0x1E40BE4).Write<uint32_t>(1);
	}

	return false;
}

void DirectConnect::PreferencesChanged()
{
}