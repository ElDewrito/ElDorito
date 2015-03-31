#include "Globals.h"

#include "../ElDorito.h"

// STL
#include <inttypes.h>
#include <iostream>


// Physics
static const size_t PhysicsGlobalsOffset = 0x32C;
static const unsigned GravityIndex = 0;
static const float DefualtGravity = 4.17126f;

// Graphics
static const size_t GraphicsGlobalsOffset = 0x3C0;
static const unsigned GraphicsOverrideIndex = 0;
static const unsigned SaturationIndex = 4;
static const unsigned ColorIndex = 8;

// Misc.
static const size_t TimingGlobalsOffset = 0x50;
static const size_t DTInverseIndex = 8;
static const size_t GameSpeedIndex = 16;

Globals::Globals()
{
	setupPhysicsGlobals();
	setupGraphicsGlobals();
	setupMiscGlobals();
}

Globals::~Globals()
{

}

std::string Globals::Info()
{
	std::string info("Allows interaction with globals.  Available globals:\n");
	for(auto &i : commands)
	{
		info += "  -" + i.first + ":" + i.second.Info();
	}

	return info;
}

void Globals::Tick(const std::chrono::duration<double>& dt)
{

}

bool Globals::Run(const std::vector<std::string>& args)
{
	// Enable graphics overrides
	ElDorito::GetMainTls(GraphicsGlobalsOffset)[GraphicsOverrideIndex].Write(1);

	if(args.size() >= 3) // Only accept "global <commmand> <args>
	{
		auto &commandItr = commands.find(args[1]);
		if(commandItr != commands.end()) // If our command exists
		{
			CommandLineArgs commandArgs(args.begin() + 2, args.end());

			// Run our command 
			return commandItr->second.Run(commandArgs);
		}
	}
 
	return false;
}

void Globals::setupGraphicsGlobals()
{
	///
	// Color
	///
	commands["color"] = {
		[this](const CommandLineArgs &args) // Run
		{
			if(args.size() >= 1)
			{
				Pointer &rPtr = ElDorito::GetMainTls(GraphicsGlobalsOffset)[ColorIndex];
				Pointer &gPtr = ElDorito::GetMainTls(GraphicsGlobalsOffset)[ColorIndex + 4];
				Pointer &bPtr = ElDorito::GetMainTls(GraphicsGlobalsOffset)[ColorIndex + 8];

				if(args[0] == "reset")
				{
					float rgb[3] = { 1.f, 1.f, 1.f };

					rPtr.Write(1.f);
					gPtr.Write(1.f);
					bPtr.Write(1.f);

					std::cout << "Color reset to " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << "." << std::endl;
				}
				else if(args.size() >= 3)
				{
					try
					{
						float rgb[3];
						for(unsigned i = 0; i < 3; ++i)
							rgb[i] = std::stof(args[i]);

						rPtr.Write(rgb[0]);
						gPtr.Write(rgb[1]);
						bPtr.Write(rgb[2]);

						std::cout << "Color set to " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << "." << std::endl;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
					return true;
				}
			}

			return false;
		},
			[]() // Info
		{
			return  R"(
	Usage: color (r|'reset') (g) (b)
	Sets the color values of the screen to given
	RGB values (0.0 - 1.0).
	Pass "reset" to reset to the default color.
)";
		}
	};
}

void Globals::setupPhysicsGlobals()
{
	///
	// Gravity
	///
	commands["gravity"] = {
		[this](const CommandLineArgs &args) // Run
		{
			if(args.size() >= 1)
			{
				Pointer &gravityPtr = ElDorito::GetMainTls(PhysicsGlobalsOffset)[GravityIndex];

				if(args[0] == "reset")
				{
					gravityPtr.Write(DefualtGravity);
					std::cout << "Gravity reset to " << DefualtGravity << "." << std::endl;
				}
				else
				{
					try
					{
						float newGravity = std::stof(args[0]);
						gravityPtr.Write(newGravity);

						std::cout << "Gravity set to " << newGravity << "." << std::endl;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
				}
				return true;
			}

			return false;
		},
			[]() // Info
		{
			return  R"(
	Usage: gravity (number|'reset')
	Sets the gravity of the game to a given number.
	Pass "reset" to reset to the default gravity.
)";
		}
	};
}

void Globals::setupMiscGlobals()
{

}
