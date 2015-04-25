#include "Countdown.h"

#include "../ElDorito.h"
#include "../ElPreferences.h"

#include <iostream>
#include <cstdlib>
#include <algorithm>

const int MaxCountdownSeconds = 20;

static void SetCountdownTimer(int seconds)
{
	Pointer::Base(0x153708).Write<uint8_t>(seconds + 0); // player control
	Pointer::Base(0x153738).Write<uint8_t>(seconds + 4); // camera position
	Pointer::Base(0x1521D1).Write<uint8_t>(seconds + 4); // ui timer

	// Fix team notification
	if (seconds == 4)
		Pointer::Base(0x1536F0).Write<uint8_t>(2);
	else
		Pointer::Base(0x1536F0).Write<uint8_t>(3);
}

static int GetCountdownTimer()
{
	return Pointer::Base(0x153708).Read<uint8_t>();
}

Countdown::Countdown()
{
	// Load countdown timer from preferences
	int countdown = ElPreferences::Instance().getCountdownTimer();
	countdown = Utils::Clamp(countdown, 0, MaxCountdownSeconds);
	SetCountdownTimer(countdown);
}

Countdown::~Countdown()
{
}

std::string Countdown::Info(const std::string& Topic) const
{
	int currentTimer = GetCountdownTimer();

	std::string Info = "Match Countdown Timer: " + std::to_string(currentTimer) + "s"
		"\nUsage: countdown (0-" + std::to_string(MaxCountdownSeconds) + ")\n"
		"Sets the countdown timer for the beginning of matches and rounds in seconds.";

	return Info;
}

std::string Countdown::Suggest(const std::vector<std::string>& Arguments) const
{
	if (Arguments.size() == 2)
	{
		if (Arguments[1].empty())
		{
			return "5";
		}
	}
	return "";
}

void Countdown::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Countdown::Run(const std::vector<std::string>& Args)
{
	if (Args.size() < 2)
		return false;

	// Clamp the time value and update the timer
	int newTime = std::atoi(Args[1].c_str());
	newTime = Utils::Clamp(newTime, 0, MaxCountdownSeconds);
	SetCountdownTimer(newTime);
	ElPreferences::Instance().setCountdownTimer(newTime);
	ElPreferences::Instance().save();

	std::cout << "Countdown timer set to " << newTime << "s." << std::endl;
	return true;
}