#include "ElPreferences.h"
#include "Utils\Macros.h"
#include <yaml-cpp\yaml.h>

#include <iostream>
#include <fstream>

const char *PreferencesFileName = "dewrito_prefs.yaml";

namespace
{
	void parsePlayerData(ElPreferences *prefs, const YAML::Node &player);
	void parseArmorData(ElPreferences *prefs, const YAML::Node &armor);
	void parseColorData(ElPreferences *prefs, const YAML::Node &colors);
	void parseVideoData(ElPreferences *prefs, const YAML::Node &video);
	void parseHostData(ElPreferences *prefs, const YAML::Node &host);
	uint32_t parseColor(const YAML::Node &color);
	void emitColor(YAML::Emitter &out, uint32_t color);
}

ElPreferences::ElPreferences()
	: playerName(""),
	helmetName("base"),
	chestName("base"),
	shouldersName("base"),
	armsName("base"),
	legsName("base"),
	accessoryName("base"),
	pelvisName("base"),
	primaryColor(0),
	secondaryColor(0),
	visorColor(0),
	lightsColor(0),
	holoColor(0),
	countdownTimer(5),
	fov(90.f)
{
}

bool ElPreferences::load()
{
	try
	{
		YAML::Node prefs = YAML::LoadFile(PreferencesFileName);
		if (prefs["player"])
			parsePlayerData(this, prefs["player"]);
		if (prefs["video"])
			parseVideoData(this, prefs["video"]);
		if (prefs["host"])
			parseHostData(this, prefs["host"]);
	}
	catch (YAML::Exception &ex)
	{
		std::cout << "Unable to load " << PreferencesFileName << ": " << ex.mark.line << "," << ex.mark.column << " " << ex.msg << std::endl;
		return false;
	}
	return true;
}

bool ElPreferences::save() const
{
	std::ofstream outFile(PreferencesFileName, std::ios::trunc);
	if (outFile.fail())
		return false;
	try
	{
		YAML::Emitter out(outFile);
		out.SetIndent(4);
		out << YAML::BeginMap;
		out << YAML::Key << "player" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "name" << YAML::Value << playerName;
		out << YAML::Key << "armor" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "helmet" << YAML::Value << helmetName;
		out << YAML::Key << "chest" << YAML::Value << chestName;
		out << YAML::Key << "shoulders" << YAML::Value << shouldersName;
		out << YAML::Key << "arms" << YAML::Value << armsName;
		out << YAML::Key << "legs" << YAML::Value << legsName;
		out << YAML::Key << "accessory" << YAML::Value << accessoryName;
		out << YAML::Key << "pelvis" << YAML::Value << pelvisName;
		out << YAML::EndMap;
		out << YAML::Key << "colors" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "primary" << YAML::Value;
		emitColor(out, primaryColor);
		out << YAML::Key << "secondary" << YAML::Value;
		emitColor(out, secondaryColor);
		out << YAML::Key << "lights" << YAML::Value;
		emitColor(out, lightsColor);
		out << YAML::Key << "visor" << YAML::Value;
		emitColor(out, visorColor);
		out << YAML::Key << "holo" << YAML::Value;
		emitColor(out, holoColor);
		out << YAML::EndMap;
		out << YAML::EndMap;
		out << YAML::Key << "video" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "fov" << YAML::Value << fov;
		out << YAML::EndMap;
		out << YAML::Key << "host" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "countdown" << YAML::Value << countdownTimer;
		out << YAML::EndMap;
		out << YAML::EndMap;
	}
	catch (YAML::Exception&)
	{
		return false;
	}
	return true;
}

namespace
{
	void parsePlayerData(ElPreferences *prefs, const YAML::Node &player)
	{
		if (player["name"])
			prefs->setPlayerName(player["name"].as<std::string>());
		if (player["armor"])
			parseArmorData(prefs, player["armor"]);
		if (player["colors"])
			parseColorData(prefs, player["colors"]);
	}

	void parseArmorData(ElPreferences *prefs, const YAML::Node &armor)
	{
		if (armor["helmet"])
			prefs->setHelmetName(armor["helmet"].as<std::string>());
		if (armor["chest"])
			prefs->setChestName(armor["chest"].as<std::string>());
		if (armor["shoulders"])
			prefs->setShouldersName(armor["shoulders"].as<std::string>());
		if (armor["arms"])
			prefs->setArmsName(armor["arms"].as<std::string>());
		if (armor["legs"])
			prefs->setLegsName(armor["legs"].as<std::string>());
		if (armor["accessory"])
			prefs->setAccessoryName(armor["accessory"].as<std::string>());
		if (armor["pelvis"])
			prefs->setPelvisName(armor["pelvis"].as<std::string>());
	}

	void parseColorData(ElPreferences *prefs, const YAML::Node &colors)
	{
		if (colors["primary"])
			prefs->setPrimaryColor(parseColor(colors["primary"]));
		if (colors["secondary"])
			prefs->setSecondaryColor(parseColor(colors["secondary"]));
		if (colors["visor"])
			prefs->setVisorColor(parseColor(colors["visor"]));
		if (colors["lights"])
			prefs->setLightsColor(parseColor(colors["lights"]));
		if (colors["holo"])
			prefs->setHoloColor(parseColor(colors["holo"]));
	}

	void parseVideoData(ElPreferences *prefs, const YAML::Node &video)
	{
		if (video["fov"])
			prefs->setFieldOfView(video["fov"].as<float>());
	}

	void parseHostData(ElPreferences *prefs, const YAML::Node &host)
	{
		if (host["countdown"])
			prefs->setCountdownTimer(host["countdown"].as<int>());
	}

	uint32_t parseColor(const YAML::Node &color)
	{
		uint8_t r = 0, g = 0, b = 0;
		if (color["r"])
			r = color["r"].as<int>();
		if (color["g"])
			g = color["g"].as<int>();
		if (color["b"])
			b = color["b"].as<int>();
		return (r << 16) | (g << 8) | b;
	}

	void emitColor(YAML::Emitter &out, uint32_t color)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "r" << YAML::Value << static_cast<int>((color >> 16) & 0xFF);
		out << YAML::Key << "g" << YAML::Value << static_cast<int>((color >> 8) & 0xFF);
		out << YAML::Key << "b" << YAML::Value << static_cast<int>(color & 0xFF);
		out << YAML::EndMap;
	}
}