#include "ModuleGraphics.hpp"
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include "../ElDorito.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Patches/Ui.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"

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

	bool VariableBloomUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto bloom = Modules::ModuleGraphics::Instance().VarBloom->ValueFloat;

		Pointer &atmoFogGlobalsPtr = ElDorito::GetMainTls(GameGlobals::Bloom::TLSOffset)[0];
		atmoFogGlobalsPtr(GameGlobals::Bloom::EnableIndex).Write(1L);
		atmoFogGlobalsPtr(GameGlobals::Bloom::IntensityIndex).Write(bloom);

		std::stringstream ss;
		ss << "Set bloom intensity to " << bloom;
		returnInfo = ss.str();

		return true;
	}

	bool VariableDepthOfFieldUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto dof = Modules::ModuleGraphics::Instance().VarDepthOfField->ValueFloat;

		Pointer &dofGlobals = ElDorito::GetMainTls(GameGlobals::DepthOfField::TLSOffset)[0];
		dofGlobals(GameGlobals::DepthOfField::EnableIndex).Write(true);
		dofGlobals(GameGlobals::DepthOfField::IntensityIndex).Write(dof);

		std::stringstream ss;
		ss << "Set depth of field intensity to " << dof;
		returnInfo = ss.str();

		return true;
	}

	bool VariableLetterboxUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto enabled = Modules::ModuleGraphics::Instance().VarLetterbox->ValueInt;

		Pointer &cinematicGlobals = ElDorito::GetMainTls(GameGlobals::Cinematic::TLSOffset)[0];
		cinematicGlobals(GameGlobals::Cinematic::LetterboxIndex).Write(enabled);

		std::stringstream ss;
		ss << (enabled ? "Enabled" : "Disabled") << " letterbox";
		returnInfo = ss.str();

		return true;
	}

	bool VariableUIScalingUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Modules::ModuleGraphics::Instance().VarUIScaling->ValueInt == 0)
			returnInfo = "The changes will apply on game restart";
		else if(ElDorito::Instance().GameHasMenuShown) //If the user comes to their senses and turns UI scaling back on then apply the change. Running before the main menu has shown may result in a crash.
			Patches::Ui::ApplyUIResolution();
		return true;
	}

	bool CommandSupportedResolutions(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		DEVMODE devmode = { 0 };
		devmode.dmSize = sizeof(devmode);
		std::vector<DEVMODE> supportedDevModes;
		std::unordered_set<uint64_t> seen;

		for (auto i = 0; EnumDisplaySettings(NULL, i, &devmode); i++)
		{
			auto key = (uint64_t)devmode.dmPelsWidth << 32 | devmode.dmPelsHeight;
			if (seen.find(key) == seen.end())
			{
				seen.insert(key);
				supportedDevModes.emplace_back(devmode);
			}
		}

		std::sort(supportedDevModes.begin(), supportedDevModes.end(),
			[](const DEVMODE& a, const DEVMODE& b) { return a.dmPelsWidth < b.dmPelsWidth; });

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartArray();
		for (const auto& d : supportedDevModes)
		{
			auto resolutionStr = std::to_string(d.dmPelsWidth) + "x" + std::to_string(d.dmPelsHeight);
			writer.String(resolutionStr.c_str());
		}
		writer.EndArray();

		returnInfo = buffer.GetString();
		return true;
	}

	bool VariableFlatHUDUpdated(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		Patches::Ui::UpdateHUDDistortion();
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

		// TODO: consider breaking some of these out into a separate cinematics module or possibly moving dof to camera

		VarBloom = AddVariableFloat("Bloom", "bloom", "The atmosphere bloom", eCommandFlagsArchived, 0.0f, VariableBloomUpdate);
		VarBloom->ValueFloatMin = 0.0f;
		VarBloom->ValueFloatMax = 5.0f;

		VarDepthOfField = AddVariableFloat("DepthOfField", "dof", "The camera's depth of field", eCommandFlagsNone, 0.0f, VariableDepthOfFieldUpdate);
		VarDepthOfField->ValueFloatMin = 0.0f;
		VarDepthOfField->ValueFloatMax = 1.0f;

		VarLetterbox = AddVariableInt("Letterbox", "letterbox", "A cinematic letterbox.", eCommandFlagsNone, 0, VariableLetterboxUpdate);
		VarLetterbox->ValueIntMin = 0;
		VarLetterbox->ValueIntMax = 1;

		VarUIScaling = AddVariableInt("UIScaling", "uiscaling", "Enables proper UI scaling to match your monitor's resolution.", eCommandFlagsArchived, 1, VariableUIScalingUpdate);
		VarUIScaling->ValueIntMin = 0;
		VarUIScaling->ValueIntMax = 1;

		VarFlatHUD = AddVariableInt("FlatHUD", "flathud", "Removes distortion from the HUD.", eCommandFlagsArchived, 0, VariableFlatHUDUpdated);
		VarFlatHUD->ValueIntMin = 0;
		VarFlatHUD->ValueIntMax = 1;

		AddCommand("SupportedResolutions", "supported_resolutions", "List the supported screen resolutions", eCommandFlagsNone, CommandSupportedResolutions);
	}
}