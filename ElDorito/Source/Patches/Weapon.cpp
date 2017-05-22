
#include <map>
#include <fstream>

#include "../Modules/ModuleWeapon.hpp"
#include "../ElDorito.hpp"

#include "../Console.hpp"
#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"
#include "../Patches/Core.hpp"
#include "Weapon.hpp"
#include <iomanip>

using namespace Blam::Math;
using namespace Blam::Objects;
using namespace Blam::Tags;
using namespace Blam::Tags::Game;
using namespace Blam::Tags::Items;

namespace
{
	std::string ConfigPath;
	std::map<std::string, uint16_t> weaponIndices;
	std::map<std::string, RealVector3D> weaponOffsetsDefault;
	std::map<std::string, RealVector3D> weaponOffsetsModified;
}

namespace Patches
{
	namespace Weapon
	{
		//Callback for when the loading screen back to the main menu finishes. We use this to determine when to start a new vote.
		void MapLoadedCallback(const char *mapPath)
		{
			std::string currentMap = mapPath;
			auto separatorIndex = currentMap.find_first_of("\\/");
			auto mapName = currentMap.substr(separatorIndex + 1);

			if (mapName != "mainmenu")
			{
				if (!weaponOffsetsDefault.empty()) {
					for (auto &element : weaponIndices) {
						std::string selected = element.first;
						auto *weapon = TagInstance(Patches::Weapon::Get::Index(selected)).GetDefinition<Blam::Tags::Items::Weapon>();
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
					}
				}

				if (!weaponOffsetsModified.empty()) {
					Config::Save(ConfigPath);
				}
				else {
					for (auto &weaponParams : weaponOffsetsModified) {
						std::string weaponName = weaponParams.first;
						auto *weapon = TagInstance(Patches::Weapon::Get::Index(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
						weapon->FirstPersonWeaponOffset = weaponParams.second;
					}
				}
			}
		}

		void Init()
		{
			ConfigPath = Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString;
			Config::Load(ConfigPath);
			Patches::Core::OnMapLoaded(MapLoadedCallback);
		}

		void ApplyAfterTagsLoaded()
		{
			auto *matg = TagInstance(0x0016).GetDefinition<Globals>();
			auto *mulg = TagInstance(matg->MultiplayerGlobals.TagIndex).GetDefinition<MultiplayerGlobals>();

			for (auto &element : mulg->Universal->GameVariantWeapons)
			{
				auto string = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name));
				auto index = (uint16_t)element.Weapon.TagIndex;

				if (index != 0xFFFF)
					weaponIndices.emplace(string, index);
			}
		}
	}
}

uint16_t Patches::Weapon::Get::Index(std::string & weaponName)
{
	if (weaponIndices.find(weaponName) == weaponIndices.end())
		return 0xFFFF;

	return weaponIndices.find(weaponName)->second;
}

std::map<std::string, uint16_t> Patches::Weapon::Get::Indices()
{
	return weaponIndices;
}

RealVector3D Patches::Weapon::Get::Offset(std::string mapped, std::string & weaponName)
{
	if (mapped == "default")
		return weaponOffsetsDefault.find(weaponName)->second;

	if (mapped == "modified")
		return weaponOffsetsModified.find(weaponName)->second;

	return { 0, 0, 0 };
}

void Patches::Weapon::Set::OffsetModified(std::string & weaponName, RealVector3D & weaponOffset)
{
	if (weaponOffsetsDefault.find(weaponName)->second == weaponOffset)
		weaponOffsetsModified.erase(weaponName);
	else
		weaponOffsetsModified.try_emplace(weaponName, weaponOffset);
}

bool Patches::Weapon::Check::OffsetModified(const std::string & weapon)
{
	return weaponOffsetsModified.find(weapon) != weaponOffsetsModified.end();
}

void Patches::Weapon::Config::Load(std::string configPath)
{
	std::ifstream inFile(configPath);
	std::vector <std::string> lines;
	std::string str;

	while (std::getline(inFile, str))
		lines.push_back(str);

	for (std::string line : lines) {
		auto weaponParams = Utils::String::SplitString(line, ' ');

		if ((weaponParams.size() < 4 || weaponParams.size() > 4))
			Console::WriteLine(line);
		else if (line[0] != '#')
		{
			std::string weaponName = weaponParams[0];
			RealVector3D offset = { stof(weaponParams[1]), stof(weaponParams[2]), stof(weaponParams[3]) };

			Set::OffsetModified(weaponName, offset);
		}
#if _DEBUG
		else {
			Console::WriteLine(line);
		}
#endif
	}
}

void Patches::Weapon::Config::Save(std::string configPath)
{
	std::ofstream outFile(configPath, std::ios::trunc);

	for (auto &weaponParams : weaponOffsetsModified) {
		std::string weaponName = weaponParams.first;
		auto *weapon = TagInstance(Patches::Weapon::Get::Index(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
		weapon->FirstPersonWeaponOffset = weaponParams.second;
		outFile << weaponName << " " << weaponParams.second.I << " " << weaponParams.second.J << " " << weaponParams.second.K << "\n";
	}
}