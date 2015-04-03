#include "Globals.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"

// STL
#include <inttypes.h>
#include <iostream>


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
		info += "\xC4\xB4";
		std::string Padded(i.first);
		Padded.resize(16, ' ');
		info += Padded;
		info += '\xC3' + std::string(6, '\xC4') + '\n' + i.second.Info();
	}

	return info;
}

void Globals::Tick(const std::chrono::duration<double>& dt)
{

}

bool Globals::Run(const std::vector<std::string>& args)
{
	// Enable graphics overrides
	ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0](GameGlobals::Graphics::GraphicsOverrideIndex).Write<bool>(true);

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
			Pointer GraphicsPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &rPtr = GraphicsPtr(GameGlobals::Graphics::ColorIndex);
				Pointer &gPtr = GraphicsPtr(GameGlobals::Graphics::ColorIndex + 4);
				Pointer &bPtr = GraphicsPtr(GameGlobals::Graphics::ColorIndex + 8);

				if(args[0] == "reset")
				{
					float rgb[3] = { 1.f, 1.f, 1.f };

					rPtr.Write(1.f);
					gPtr.Write(1.f);
					bPtr.Write(1.f);

					std::cout << "Color reset to " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << '.' << std::endl;
					return true;
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

						std::cout << "Color set to " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << '.' << std::endl;
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
			return "Usage: color (r|'reset') (g) (b)\n"
				"Sets the color values of the screen to given\n"
				"RGB values(0.0 - 1.0).\n"
				"Pass \"reset\" to reset to the default color.\n";
		}
	};

	///
	// Saturation
	///
	commands["saturation"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer GraphicsPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &saturationPtr = GraphicsPtr(GameGlobals::Graphics::SaturationIndex);

				if(args[0] == "reset")
				{
					saturationPtr.Write(1.f);
					std::cout << "Saturation reset to 1." << std::endl;
					return true;
				}
				else
				{
					try
					{
						float newSaturation = std::stof(args[0]);
						saturationPtr.Write(newSaturation);
						std::cout << "Saturation reset to " << newSaturation << '.' << std::endl;
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
			return  "Usage: saturation(number | 'reset')\n"
			"Sets the saturation value of the screen to given number (0.0 - 1.0).\n"
			"Pass \"reset\" to reset to the default saturation.\n";
		}
	};

	///
	// Letterbox
	///
	commands["letterbox"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer CinimaticPtr = ElDorito::GetMainTls(GameGlobals::Cinematic::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &saturationPtr = CinimaticPtr(GameGlobals::Cinematic::LetterboxIndex);

				if(args[0] == "on" || args[0] == "true")
				{
					saturationPtr.Write(true);
					std::cout << "Letterboxing enabled." << std::endl;
					return true;
				}
				else if(args[0] == "off" || args[0] == "false")
				{
					saturationPtr.Write(false);
					std::cout << "Letterboxing disabled." << std::endl;
					return true;
				}
			}

			return false;
		},
			[]() // Info
		{
			return  "Usage: letterbox (on|off)\n"
				"Enables letterboxing.\n";
		}
	};

	///
	// DoF
	///
	commands["dof"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer DoFPtr = ElDorito::GetMainTls(GameGlobals::DepthOfField::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &overridePtr = DoFPtr(GameGlobals::DepthOfField::EnableIndex);
				Pointer &intensityPtr = DoFPtr(GameGlobals::DepthOfField::IntensityIndex);

				overridePtr.Write(true);

				if(args[0] == "reset")
				{
					intensityPtr.Write(0.f);

					std::cout << "Depth of field reset." << std::endl;
					return true;
				}
				else
				{
					try
					{
						float newDof = std::stof(args[0]);

						intensityPtr.Write(newDof / 50.f);

						std::cout << "Depth of field set to " << newDof << '.' << std::endl;
						return true;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
				}
			}

			return false;
		},
			[]() // Info
		{
			return  "Usage: dof (number|'reset')\n"
				"Enables depth of field with a given number value.\n"
				"Pass \"reset\" to reset and turn off depth of field.\n";
		}
	};

	///
	// Bloom
	///
	commands["bloom"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer BloomPtr = ElDorito::GetMainTls(GameGlobals::Bloom::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &overridePtr = BloomPtr(GameGlobals::Bloom::EnableIndex);
				Pointer &intensityPtr = BloomPtr(GameGlobals::Bloom::IntensityIndex);
			
				if(args[0] == "reset")
				{
					intensityPtr.Write(0.f);
					overridePtr.Write(size_t(0));
			
					std::cout << "Bloom reset and turned off." << std::endl;
					return true;
				}
				else
				{
					try
					{
						float newBloom = std::stof(args[0]);
			
						intensityPtr.Write(newBloom);
						overridePtr.Write(size_t(1));
			
						std::cout << "Bloom set to " << newBloom << '.' << std::endl;
						return true;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
				}
			}
			return false;
		},
			[]() // Info
		{
			return  "Usage: bloom (number|'reset')\n"
				"Enables bloom with a given number value.\n"
				"Pass \"reset\" to reset and turn off bloom.\n";
		}
	};

#ifdef _ELDEBUG
	///
	// Test
	///
	commands["test"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer GraphicsPtr = ElDorito::GetMainTls(0xC4)[0];
			if(args.size() > 1)
			{

				int index = std::atoi(args[0].c_str());

			}
			else
			{
				for(unsigned i = 0; i < 4; ++i)
				{
					std::cout << "Debug Value " << i << ": " << GraphicsPtr(i * 4).Read<float>() << std::endl;
				}
			}
			return true;
		},
			[]() // Info
		{
			return  R"(
	Usage: you probably shouldn't use this
	)";
		}
	};
#endif
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
				Pointer &gravityPtr = ElDorito::GetMainTls(GameGlobals::Physics::TLSOffset)[0](GameGlobals::Physics::GravityIndex);

				if(args[0] == "reset")
				{
					gravityPtr.Write(GameGlobals::Physics::DefualtGravity);
					std::cout << "Gravity reset to " << GameGlobals::Physics::DefualtGravity << '.' << std::endl;
				}
				else
				{
					try
					{
						float newGravity = std::stof(args[0]);
						gravityPtr.Write(newGravity);

						std::cout << "Gravity set to " << newGravity << '.' << std::endl;
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
			return  "Usage: gravity (number|'reset')\n"
				"Sets the gravity of the game to a given number.\n"
				"Pass \"reset\" to reset to the default gravity.\n";
		}
	};
}

void Globals::setupMiscGlobals()
{

}
