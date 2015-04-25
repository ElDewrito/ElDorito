#include "ElPreferences.h"
#include "Utils\Macros.h"

#include <iostream>

const char *PreferencesFileName = "dewrito_prefs.json";

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
	holoColor(0)
{
}

bool ElPreferences::load()
{
	return false;
}

bool ElPreferences::save() const
{
	return false;
}