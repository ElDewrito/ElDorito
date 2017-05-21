
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

using namespace Blam::Objects;
using namespace Blam::Math;

namespace
{
	std::string ConfigPath = Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString;

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
				using Blam::Math::RealVector3D;
				using Blam::Tags::TagInstance;
				using Blam::Tags::Items::Weapon;

				if (!weaponOffsetsDefault.empty()) {
					for (auto &element : weaponIndices) {
						std::string selected = element.first;
						auto *weapon = TagInstance(Patches::Weapon::Get_WeaponIndex(selected)).GetDefinition<Blam::Tags::Items::Weapon>();
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
					}
				}

				if (!weaponOffsetsModified.empty()) {
					std::ofstream outFile(ConfigPath, std::ios::trunc);

					for (auto &weaponParams : weaponOffsetsModified) {
						std::string weaponName = weaponParams.first;
						auto *weapon = TagInstance(Patches::Weapon::Get_WeaponIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
						weapon->FirstPersonWeaponOffset = weaponParams.second;
						outFile << weaponName << " " << weaponParams.second.I << " " << weaponParams.second.J << " " << weaponParams.second.K << "\n";
					}
				}
				else {
					std::ifstream inFile(ConfigPath);
					std::vector <std::string> lines;

					std::string str;
					while (std::getline(inFile, str))
						lines.push_back(str);

					for(std::string line : lines) {
						auto weaponParams = Utils::String::SplitString(line, ' ');

						std::string weaponName = weaponParams[0];
						RealVector3D offset = { stof(weaponParams[1]), stof(weaponParams[2]), stof(weaponParams[3]) };

						Update_WeaponOffsetsModified(weaponName, offset);

						Console::WriteLine(line);
					}

					for (auto &weaponParams : weaponOffsetsModified) {
						std::string weaponName = weaponParams.first;
						auto *weapon = TagInstance(Patches::Weapon::Get_WeaponIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
						weapon->FirstPersonWeaponOffset = weaponParams.second;
					}
				}
			}
		}

		void Init()
		{
			Patches::Core::OnMapLoaded(MapLoadedCallback);
		}

		void ApplyAfterTagsLoaded()
		{
			using Blam::Tags::TagInstance;
			using Blam::Tags::Game::Globals;
			using Blam::Tags::Game::MultiplayerGlobals;

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

		uint16_t Get_WeaponIndex(std::string &weaponName)
		{
			if (weaponIndices.find(weaponName) == weaponIndices.end())
				weaponName = "assault_rifle";

			return weaponIndices.find(weaponName)->second;
		}

		RealVector3D Get_WeaponOffset(std::string &weaponName, bool isDefault)
		{
			if (isDefault) {
				return weaponOffsetsDefault.find(weaponName)->second;
			}
			else {
				return weaponOffsetsModified.find(weaponName)->second;
			}
		}

		void Update_WeaponOffsetsModified(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (weaponOffsetsDefault.find(weaponName)->second == weaponOffset) {
				weaponOffsetsModified.erase(weaponName);
			}
			else {
				weaponOffsetsModified.try_emplace(weaponName, weaponOffset);
			}
		}

		bool Is_OffsetModified(const std::string &weapon)
		{
			return weaponOffsetsModified.find(weapon) != weaponOffsetsModified.end();
		}
	}
}