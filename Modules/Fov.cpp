#include "Fov.h"

#include "Fov.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>
#include <cstdlib> // std::atof

static const size_t FOVOffset = 0x1F01D98;

Fov::Fov()
{
}

Fov::~Fov()
{
}

std::string Fov::Info()
{
	float currentFov = Pointer(Pointer::Base()(FOVOffset)).Read<float>();

	std::string Info = "Field of View: " + std::to_string(currentFov) + R"(
Usage: fov (0.1-150.0)
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
	static const size_t LowerBoundsOffset = 0x25FA79;
	static const size_t UpperBoundsOffset = 0x25FA86;

	// Upper Bound
	static const uint8_t upperNOP[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	static const uint8_t upperReset[] = { 0xF3, 0x0F, 0x11, 0x4D, 0x08 };

	// Lower Bound
	static const uint8_t lowerNOP[] = { 0x90, 0x90 , 0x90};
	static const uint8_t lowerReset[] = { 0x0F, 0x28, 0xC1 };

	if(Args.size() >= 2)
	{
		float fov = static_cast<float>(std::atof(Args[1].c_str()));
		if(fov == 0.f)
			return false;

		// Patch FOV Bounds
		std::cout << "FOV set to " << fov << "." << std::endl;

		Pointer(Pointer::Base()(LowerBoundsOffset)).Write(lowerNOP, sizeof(lowerNOP));
		Pointer(Pointer::Base()(UpperBoundsOffset)).Write(upperNOP, sizeof(upperNOP));

		// Write FOV
		Pointer(Pointer::Base()(FOVOffset)).Write<float>(fov);

		return true;
	}

	return false;
}
