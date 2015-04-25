#include "Fov.h"

#include "../ElDorito.h"
#include "../ElPreferences.h"

#include <iostream>
#include <cstdlib> // std::atof
#include <algorithm> // std::min, std::max

namespace
{
	const size_t FOVOffset = 0x1F01D98;
	const size_t InitialFOVOffset = 0x149D42C;

	void SetFov(float newFov)
	{
		Pointer::Base(FOVOffset).Write(newFov);
		Pointer::Base(InitialFOVOffset).Write(newFov);
	}
}

Fov::Fov()
{
	// Load FOV from preferences
	float fov = ElPreferences::Instance().getFieldOfView();
	fov = Utils::Clamp(fov, 0.001f, 150.0f);
	SetFov(fov);
}

Fov::~Fov()
{
}

std::string Fov::Info(const std::string& Topic) const
{
	float currentFov = Pointer::Base(FOVOffset).Read<float>();

	std::string Info = "Field of View: " + std::to_string(currentFov) +
		"\nUsage: fov (0.001-150.0)\n"
		"Sets the field of view for the first person camera.\n"
		"This does not currently effect third-person views.";

	return Info;
}

std::string Fov::Suggest(const std::vector<std::string>& Arguments) const
{
	if( Arguments.size() == 2 )
	{
		if( Arguments[1].empty() )
		{
			return "90.0";
		}
	}
	return "";
}

void Fov::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Fov::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 2 )
	{
		float fov = static_cast<float>(std::atof(Args[1].c_str()));

		// If atof returns 0, it (probably) failed
		if( fov == 0.f )
			return false;

		fov = Utils::Clamp(fov, 0.001f, 150.0f);
		ElPreferences::Instance().setFieldOfView(fov);
		ElPreferences::Instance().save();

		// Write bounded FOV
		SetFov(fov);
		std::cout << "FOV set to " << fov << '.' << std::endl;

		return true;
	}

	return false;
}