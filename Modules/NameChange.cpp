#include "NameChange.h"

#include "../ElDorito.h"
#include "../ElPreferences.h"

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>

NameChange::NameChange()
{
	memset(this->UserName, 0, sizeof(wchar_t) * 17);
	Pointer::Base(0x42AA1).Write<uint32_t>((uint32_t)&this->UserName);
	GetSavedUsername();
}

NameChange::~NameChange()
{
}

std::string NameChange::Info(const std::string& Topic) const
{
	std::string Info = "Current profile name: ";
	Info += Utils::String::ThinString(std::wstring(this->UserName));
	Info += "\nUsage: name (username)\n"
		"Sets the profile name shown ingame.";

	return Info;
}

std::string NameChange::Suggest(const std::vector<std::string>& Arguments) const
{
	return "";
}

void NameChange::Tick(const std::chrono::duration<double>& Delta)
{
}

bool NameChange::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1].length() > 15) {
			std::cout << "ERROR: Nickname is too long (max 15 chars)!" << std::endl;
			return false;
		}

		std::wstring nameStr = Utils::String::WidenString(Args[1]);
		wcscpy_s(this->UserName, 16, nameStr.c_str());
		std::string actualName = Utils::String::ThinString(std::wstring(this->UserName));

		std::cout
			<< "Set profile name to " << actualName << std::endl;

		ElPreferences::Instance().setPlayerName(actualName);
		ElPreferences::Instance().save();

		return true;
	}

	return false;
}

void NameChange::GetSavedUsername()
{
	wcscpy_s(this->UserName, 16, Utils::String::WidenString(ElPreferences::Instance().getPlayerName()).c_str());

	if (wcslen(this->UserName) > 0)
		return;

	wchar_t* defaultNames[40] = {
		L"Donut", L"Penguin", L"Stumpy", L"Whicker", L"Shadow", L"Howard", L"Wilshire",
		L"Darling", L"Disco", L"Jack", L"The Bear", L"Sneak", L"The Big L", L"Whisp",
		L"Wheezy", L"Crazy", L"Goat", L"Pirate", L"Saucy", L"Hambone", L"Butcher",
		L"Walla Walla", L"Snake", L"Caboose", L"Sleepy", L"Killer", L"Stompy",
		L"Mopey", L"Dopey", L"Weasel", L"Ghost", L"Dasher", L"Grumpy", L"Hollywood",
		L"Tooth", L"Noodle", L"King", L"Cupid", L"Prancer", L"Pyong"
	};

	srand((unsigned int)time(0));
	wcscpy_s(this->UserName, 16, defaultNames[rand() % 40]);
}

void NameChange::PreferencesChanged()
{
	// TODO: Anything we should do here?
}