#pragma once

#include "Utils\Singleton.h"

#include <cstdint>
#include <string>

class ElPreferences : public Utils::Singleton<ElPreferences>
{
public:
	ElPreferences();

	bool load();
	bool save() const;

	std::string getPlayerName() const { return playerName; }
	void setPlayerName(const std::string &name) { playerName = name; }

	std::string getHelmetName() const { return helmetName; }
	std::string getChestName() const { return chestName; }
	std::string getShouldersName() const { return shouldersName; }
	std::string getArmsName() const { return armsName; }
	std::string getLegsName() const { return legsName; }
	std::string getAccesoryName() const { return accessoryName; }
	std::string getPelvisName() const { return pelvisName; }

	void setHelmetName(const std::string &name) { helmetName = name; }
	void setChestName(const std::string &name) { chestName = name; }
	void setShouldersName(const std::string &name) { shouldersName = name; }
	void setArmsName(const std::string &name) { armsName = name; }
	void setLegsName(const std::string &name) { legsName = name; }
	void setAccesoryName(const std::string &name) { accessoryName = name; }
	void setPelvisName(const std::string &name) { pelvisName = name; }

	uint32_t getPrimaryColor() const { return primaryColor; }
	uint32_t getSecondaryColor() const { return secondaryColor; }
	uint32_t getVisorColor() const { return visorColor; }
	uint32_t getLightsColor() const { return lightsColor; }
	uint32_t getHoloColor() const { return holoColor; }

	void setPrimaryColor(uint32_t color) { primaryColor = color; }
	void setSecondaryColor(uint32_t color) { secondaryColor = color; }
	void setVisorColor(uint32_t color) { visorColor = color; }
	void setLightsColor(uint32_t color) { lightsColor = color; }
	void setHoloColor(uint32_t color) { holoColor = color; }

private:
	std::string playerName;
	
	std::string helmetName;
	std::string chestName;
	std::string shouldersName;
	std::string armsName;
	std::string legsName;
	std::string accessoryName;
	std::string pelvisName;

	uint32_t primaryColor;
	uint32_t secondaryColor;
	uint32_t visorColor;
	uint32_t lightsColor;
	uint32_t holoColor;
};