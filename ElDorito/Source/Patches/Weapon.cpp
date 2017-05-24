
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

using Blam::Math::RealVector3D;
using Blam::Tags::TagInstance;
using Blam::Tags::Game::Globals;
using Blam::Tags::Game::MultiplayerGlobals;
using Blam::Tags::Items::Weapon;

namespace
{
	bool IsNotMainMenu;
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

			if (mapName == "mainmenu") {
				IsNotMainMenu = false;
			}
			else {
				IsNotMainMenu = true;

				if (!weaponOffsetsDefault.empty()) {
					for (auto &element : weaponIndices) {
						std::string selected = element.first;
						auto *weapon = TagInstance(Patches::Weapon::GetIndex(selected)).GetDefinition<Blam::Tags::Items::Weapon>();
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
					}
				}

				if (!weaponOffsetsModified.empty()) {
					ConfigSave(ConfigPath);
				}
				else {
					for (auto &weaponParams : weaponOffsetsModified) {
						std::string weaponName = weaponParams.first;
						RealVector3D weaponOffset = weaponParams.second;
						ApplyOffset(weaponName, weaponOffset);
					}
				}
			}
		}

		void Init()
		{
			ConfigPath = Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString;
			ConfigLoad(ConfigPath);
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

		uint16_t GetIndex(std::string &weaponName)
		{
			if (weaponIndices.find(weaponName) == weaponIndices.end())
				return 0xFFFF;

			return weaponIndices.find(weaponName)->second;
		}

		std::map<std::string, uint16_t> GetIndices()
		{
			return weaponIndices;
		}

		RealVector3D GetOffset(bool default, std::string &weaponName)
		{
			if (default)
				return weaponOffsetsDefault.find(weaponName)->second;
			else
				return weaponOffsetsModified.find(weaponName)->second;
		}

		void SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (weaponOffsetsDefault.find(weaponName)->second == weaponOffset) {
				weaponOffsetsModified.erase(weaponName);
				ApplyOffset(weaponName, weaponOffset);
			}
			else {
				weaponOffsetsModified.try_emplace(weaponName, weaponOffset);
				ApplyOffset(weaponName, weaponOffset);
			}
		}

		void ApplyOffset(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (IsNotMainMenu) {
				auto *weapon = TagInstance(Patches::Weapon::GetIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
				weapon->FirstPersonWeaponOffset = weaponOffset;
			}
		}

		bool IsOffsetModified(const std::string &weapon)
		{
			return weaponOffsetsModified.find(weapon) != weaponOffsetsModified.end();
		}

		void ConfigLoad(std::string configPath)
		{
			std::ifstream inFile(configPath);
			std::vector <std::string> lines;
			std::string str;

			while (std::getline(inFile, str))
				lines.push_back(str);

			weaponOffsetsModified.clear();

			for (std::string line : lines) {
				auto weaponParams = Utils::String::SplitString(line, ' ');

				if ((weaponParams.size() < 4 || weaponParams.size() > 4)) {
#if _DEBUG
					Console::WriteLine(line);
#endif
				}
				else if (line[0] != '#')
				{
					std::string weaponName = weaponParams[0];
					RealVector3D offset = { stof(weaponParams[1]), stof(weaponParams[2]), stof(weaponParams[3]) };

					SetOffsetModified(weaponName, offset);
				}
				else {
#if _DEBUG
					Console::WriteLine(line);
#endif
				}
			}
		}

		void ConfigSave(std::string configPath)
		{
			std::ofstream outFile(configPath, std::ios::trunc);

			for (auto &weaponParams : weaponOffsetsModified) {
				std::string weaponName = weaponParams.first;
				auto *weapon = TagInstance(Patches::Weapon::GetIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
				weapon->FirstPersonWeaponOffset = weaponParams.second;
				outFile << weaponName << " " << weaponParams.second.I << " " << weaponParams.second.J << " " << weaponParams.second.K << "\n";
			}
		}
	}
}