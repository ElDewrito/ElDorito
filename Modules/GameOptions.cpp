#include "GameOptions.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

TODO("Find how to get game options to show by pressing X, like on 360")

GameOptions::GameOptions()
	:
	enabled(false),
	EnablePatch(0x701360, 0x90, 2)
{
}

GameOptions::~GameOptions()
{
}

std::string GameOptions::Info(const std::string& Topic) const
{
	std::string Info = "Redirecting start game to show game options: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "\nUsage: game_options (on|off)\n"
		"Very hacky method of showing the game options UI\n"
		"Start a lobby, turn this on, then press start game\n"
		"Options UI should appear, make sure to run game_options off after your done";

	return Info;
}

std::string GameOptions::Suggest(const std::vector<std::string>& Arguments) const
{
	if (Arguments.size() == 2)
	{
		if (Arguments[1].empty())
		{
			return (enabled ? "off" : "on");
		}
	}
	return "";
}

void GameOptions::Tick(const std::chrono::duration<double>& Delta)
{
}

bool GameOptions::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1].compare("off") == 0)
		{
			std::cout << "Disabling game options UI" << std::endl;
			enabled = false;

			EnablePatch.Reset();

			return true;
		}
		else if (Args[1].compare("on") == 0)
		{
			std::cout
				<< "Enabling game options UI, choose start game to show options UI\n"
				"Make sure to run game_options off once you're finished"
				<< std::endl;
			enabled = true;

			EnablePatch.Apply();

			return true;
		}
	}

	return false;
}