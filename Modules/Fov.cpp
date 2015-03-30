#include "Fov.h"

#include "../ElDorito.h"

#include <iostream>
#include <cstdlib> // std::atof
#include <algorithm> // std::min, std::max

static const size_t FOVOffset = 0x1F01D98;

static const float MinFOV = 0.001f;
static const float MaxFOV = 150.f;

Fov::Fov()
{
	Patch::NopFill(Pointer::Base(0x25FA79), 5);
	Patch::NopFill(Pointer::Base(0x25FA86), 5);

	/*
	const size_t LowerBoundsOffset = 0x25FA79;
	const size_t UpperBoundsOffset = 0x25FA86;

	// Upper Bound
	const uint8_t upperNOP[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	const uint8_t upperReset[] = { 0xF3, 0x0F, 0x11, 0x4D, 0x08 };

	// Lower Bound
	const uint8_t lowerNOP[] = { 0x90, 0x90, 0x90 };
	const uint8_t lowerReset[] = { 0x0F, 0x28, 0xC1 };

	// Patch FOV Bounds checking
	Pointer::Base(LowerBoundsOffset).Write(lowerNOP, sizeof(lowerNOP));
	Pointer::Base(UpperBoundsOffset).Write(upperNOP, sizeof(upperNOP));
	*/
}

Fov::~Fov()
{
}

std::string Fov::Info()
{
	float currentFov = Pointer::Base(FOVOffset).Read<float>();

	std::string Info = "Field of View: " + std::to_string(currentFov) +
		R"(
Usage: fov (0.001-150.0)
Sets the field of view for the first person camera.
This does not currently effect third-person views.
)";

	return Info;
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
		if(fov == 0.f)
			return false;

		fov = std::min(std::max(MinFOV, fov), MaxFOV);

		// Write bounded FOV
		Pointer::Base(FOVOffset).Write(fov);
		std::cout << "FOV set to " << fov << "." << std::endl;

		return true;
	}

	return false;
}