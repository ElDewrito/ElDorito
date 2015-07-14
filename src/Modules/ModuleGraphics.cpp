#include "ModuleGraphics.hpp"
#include <sstream>
#include "../ElDorito.hpp"

namespace
{
	bool VariableSaturationUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto saturation = Modules::ModuleGraphics::Instance().VarSaturation->ValueFloat;
		Pointer &hueSaturationControlPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
		hueSaturationControlPtr(GameGlobals::Graphics::GraphicsOverrideIndex).Write(true);
		hueSaturationControlPtr(GameGlobals::Graphics::SaturationIndex).Write(saturation);

		std::stringstream ss;
		ss << "Set saturation to " << saturation;
		returnInfo = ss.str();

		return true;
	}

	bool VariableRedHueUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto redHue = Modules::ModuleGraphics::Instance().VarRedHue->ValueFloat;
		Pointer &hueSaturationControlPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
		hueSaturationControlPtr(GameGlobals::Graphics::GraphicsOverrideIndex).Write(true);
		hueSaturationControlPtr(GameGlobals::Graphics::ColorIndex + sizeof(float) * 0).Write(redHue);

		std::stringstream ss;
		ss << "Set red hue to " << redHue;
		returnInfo = ss.str();

		return true;
	}

	bool VariableGreenHueUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto greenHue = Modules::ModuleGraphics::Instance().VarGreenHue->ValueFloat;
		Pointer &hueSaturationControlPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
		hueSaturationControlPtr(GameGlobals::Graphics::GraphicsOverrideIndex).Write(true);
		hueSaturationControlPtr(GameGlobals::Graphics::ColorIndex + sizeof(float) * 1).Write(greenHue);

		std::stringstream ss;
		ss << "Set green hue to " << greenHue;
		returnInfo = ss.str();

		return true;
	}

	bool VariableBlueHueUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto blueHue = Modules::ModuleGraphics::Instance().VarBlueHue->ValueFloat;
		Pointer &hueSaturationControlPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
		hueSaturationControlPtr(GameGlobals::Graphics::GraphicsOverrideIndex).Write(true);
		hueSaturationControlPtr(GameGlobals::Graphics::ColorIndex + sizeof(float) * 2).Write(blueHue);

		std::stringstream ss;
		ss << "Set blue hue to " << blueHue;
		returnInfo = ss.str();

		return true;
	}
}

namespace Modules
{
	ModuleGraphics::ModuleGraphics() : ModuleBase("Graphics")
	{
		VarSaturation = AddVariableFloat("Saturation", "saturation", "The saturation", eCommandFlagsArchived, 1.0f, VariableSaturationUpdate);
		VarSaturation->ValueFloatMin = -10.0f;
		VarSaturation->ValueFloatMax = 10.0f;

		VarRedHue = AddVariableFloat("RedHue", "red_hue", "The red hue", eCommandFlagsNone, 1.0f, VariableRedHueUpdate);
		VarRedHue->ValueFloatMin = 0.0f;
		VarRedHue->ValueFloatMax = 1.0f;

		VarGreenHue = AddVariableFloat("GreenHue", "green_hue", "The green hue", eCommandFlagsNone, 1.0f, VariableGreenHueUpdate);
		VarGreenHue->ValueFloatMin = 0.0f;
		VarGreenHue->ValueFloatMax = 1.0f;

		VarBlueHue = AddVariableFloat("BlueHue", "blue_hue", "The blue hue", eCommandFlagsNone, 1.0f, VariableBlueHueUpdate);
		VarBlueHue->ValueFloatMin = 0.0f;
		VarBlueHue->ValueFloatMax = 1.0f;
	}
}