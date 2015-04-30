#include "Fps.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"

#include <iostream>
#include <cstdlib>
#include <algorithm>

const int MinFps = 24;
const int MaxFps = 144;

static void SetMaxFps(int fps)
{
	// enables fps persistence
	// mov     word [ecx+4], 0XXXXh
	const uint8_t data[] = { 0x66, 0xC7, 0x41, 0x04 };
	Pointer::Base(0x164F6D).Write(data, sizeof(data));
	Pointer::Base(0x164F71).Write<int16_t>(fps);

	// Adjust game time globals
	Pointer timeGlobals = ElDorito::GetMainTls(GameGlobals::Time::TLSOffset)[0];
	timeGlobals(GameGlobals::Time::FpsIndex).Write<int16_t>(fps);
	timeGlobals(GameGlobals::Time::DTInverseIndex).Write<float>(1.f / fps);
}

static int GetMaxFps()
{
	// Load the value in game time globals
	Pointer timeGlobals = ElDorito::GetMainTls(GameGlobals::Time::TLSOffset)[0];
	return timeGlobals(GameGlobals::Time::FpsIndex).Read<int16_t>();
}

Fps::Fps()
{
}

Fps::~Fps()
{
}

std::string Fps::Info(const std::string& Topic) const
{
	int currentLimit = GetMaxFps();

	std::string Info = "FPS Limit: " + std::to_string(currentLimit) +
		"\nUsage: fps (" + std::to_string(MinFps) + "-" + std::to_string(MaxFps) + ")\n"
		"Sets the game's FPS limit.\n"
		"It is not recommended to set this higher than your monitor's refresh rate.";

	return Info;
}

std::string Fps::Suggest(const std::vector<std::string>& Arguments) const
{
	if (Arguments.size() == 2)
	{
		if (Arguments[1].empty())
		{
			return "120";
		}
	}
	return "";
}

void Fps::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Fps::Run(const std::vector<std::string>& Args)
{
	if (Args.size() < 2)
		return false;

	int newLimit = std::atoi(Args[1].c_str());
	if (newLimit == 0)
		return false; // Parsing probably failed

	// trololol
	Pointer cinematicPtr = ElDorito::GetMainTls(GameGlobals::Cinematic::TLSOffset)[0];
	Pointer letterboxPtr = cinematicPtr(GameGlobals::Cinematic::LetterboxIndex);
	letterboxPtr.Write(newLimit == 24);

	newLimit = Utils::Clamp(newLimit, MinFps, MaxFps);

	SetMaxFps(newLimit);
	std::cout << "FPS limit set to " << newLimit << "." << std::endl;
	return true;
}

void Fps::PreferencesChanged()
{
}