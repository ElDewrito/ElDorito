#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "ServerPassword.h"
#include "../ElDorito.h"
#include "../ElPreferences.h"
#include "../ThirdParty/SHA256.h"
#include <iostream>

extern BYTE passwordHash[0x20];
extern bool usingPassword;

ServerPassword::ServerPassword()
{
	SetPassword(ElPreferences::Instance().getServerPassword());
}

ServerPassword::~ServerPassword()
{
}

std::string ServerPassword::Info(const std::string& Topic) const
{
	std::string Info = "Server password: " + ElPreferences::Instance().getServerPassword();
	Info += "\nUsage: password [password]\n"
		"Sets the server password, clients must specify this password to connect.\n"
		"If command is run with no parameters the password will be unset.\n"
		"This command can only be used if the game isn't run with the -lan parameter.\n";

	return Info;
}

std::string ServerPassword::Suggest(const std::vector<std::string>& Arguments) const
{
	return ElPreferences::Instance().getServerPassword();
}

void ServerPassword::Tick(const std::chrono::duration<double>& Delta)
{
}

bool ServerPassword::Run(const std::vector<std::string>& Args)
{
	if (ElPreferences::Instance().getServerLanMode())
	{
		std::cout << "The password command can't be used as the game was run with the -lan parameter.";
		return true;
	}

	if (Args.size() >= 1)
	{
		std::string password = "";
		if(Args.size() >= 2)
			password = Args[1];

		SetPassword(password, true);

		return true;
	}

	return false;
}

void ServerPassword::PreferencesChanged()
{
	SetPassword(ElPreferences::Instance().getServerPassword());
}

void ServerPassword::SetPassword(const std::string &password, bool printInfo)
{
	usingPassword = password.length() > 0;

	if (usingPassword)
	{
		SHA256_CTX ctx;
		sha256_init(&ctx);
		sha256_update(&ctx, (unsigned char*)password.c_str(), password.length());
		sha256_final(&ctx, passwordHash);

		if (printInfo)
			std::cout << "Server password set to \"" << password << "\"." << std::endl;
	}
	else
	{
		if (printInfo)
			std::cout << "Server password unset." << std::endl;
	}

	if (printInfo) // if user is setting the password
	{
		ElPreferences::Instance().setServerPassword(password);
		ElPreferences::Instance().save();
	}
}