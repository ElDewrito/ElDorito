
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
				using Blam::Math::RealVector3D;
				using Blam::Tags::TagInstance;
				using Blam::Tags::Items::Weapon;

				if (!weaponOffsetsDefault.empty()) {
					for (auto &element : weaponIndices) {
						std::string selected = element.first;
						auto *weapon = TagInstance(Patches::Weapon::Get_Index(selected)).GetDefinition<Blam::Tags::Items::Weapon>();
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
					}
				}

				if (!weaponOffsetsModified.empty()) {
					Save_Config(ConfigPath);
				}
				else {
					for (auto &weaponParams : weaponOffsetsModified) {
						std::string weaponName = weaponParams.first;
						auto *weapon = TagInstance(Patches::Weapon::Get_Index(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
						weapon->FirstPersonWeaponOffset = weaponParams.second;
					}
				}
			}
		}

		void Init()
		{
			ConfigPath = Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString;
			Load_Config(ConfigPath);
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

		void Save_Config(std::string configPath)
		{
			using Blam::Math::RealVector3D;
			using Blam::Tags::TagInstance;
			using Blam::Tags::Items::Weapon;

			std::ofstream outFile(configPath, std::ios::trunc);

			for (auto &weaponParams : weaponOffsetsModified) {
				std::string weaponName = weaponParams.first;
				auto *weapon = TagInstance(Patches::Weapon::Get_Index(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
				weapon->FirstPersonWeaponOffset = weaponParams.second;
				outFile << weaponName << " " << weaponParams.second.I << " " << weaponParams.second.J << " " << weaponParams.second.K << "\n";
			}
		}

		void Load_Config(std::string configPath)
		{
			std::ifstream inFile(configPath);
			std::vector <std::string> lines;

			std::string str;
			while (std::getline(inFile, str))
				lines.push_back(str);

			for (std::string line : lines) {
				auto weaponParams = Utils::String::SplitString(line, ' ');

				std::string weaponName = weaponParams[0];
				RealVector3D offset = { stof(weaponParams[1]), stof(weaponParams[2]), stof(weaponParams[3]) };

				Update_OffsetsModified(weaponName, offset);
			}
		}

		uint16_t Get_Index(std::string &weaponName)
		{
			if (weaponIndices.find(weaponName) == weaponIndices.end())
				return 0xFFFF;

			return weaponIndices.find(weaponName)->second;
		}

		RealVector3D Get_Offset(std::string &weaponName)
		{
			return weaponOffsetsModified.find(weaponName)->second;
		}

		RealVector3D Get_OffsetDefault(std::string &weaponName)
		{
			return weaponOffsetsDefault.find(weaponName)->second;
		}

		void Update_OffsetsModified(std::string &weaponName, RealVector3D &weaponOffset)
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