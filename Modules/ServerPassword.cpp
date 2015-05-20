#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "ServerPassword.h"
#include "../ElDorito.h"
#include "../ThirdParty/SHA256.h"
#include <iostream>

extern BYTE passwordHash[0x20];
extern bool usingPassword;

ServerPassword::ServerPassword()
{
}

ServerPassword::~ServerPassword()
{
}

std::string ServerPassword::Info(const std::string& Topic) const
{
	std::string Info = "Server password: " + currentPassword;
	Info += "\nUsage: connect (ip[:port]) [password]\n"
		"Begins a connection to the remote server, if server uses a password then password field must be set.";

	return Info;
}

std::string ServerPassword::Suggest(const std::vector<std::string>& Arguments) const
{
	return "";
}

void ServerPassword::Tick(const std::chrono::duration<double>& Delta)
{
}

bool ServerPassword::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 1)
	{
		currentPassword = "";
		if(Args.size() >= 2)
			currentPassword = Args[1];

		usingPassword = currentPassword.length() > 0;

		if (usingPassword)
		{
			SHA256_CTX ctx;
			sha256_init(&ctx);
			sha256_update(&ctx, (unsigned char*)currentPassword.c_str(), currentPassword.length());
			sha256_final(&ctx, passwordHash);

			std::cout << "Server password set to \"" << currentPassword << "\"." << std::endl;
		}
		else
		{
			std::cout << "Server password unset." << std::endl;
		}

		return true;
	}

	return false;
}

void ServerPassword::PreferencesChanged()
{
}