#include "ModuleWeapon.hpp"
#include "../ElDorito.hpp"
#include "../Patches/Weapon.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"

namespace
{
	using Blam::Math::RealVector3D;
	using Blam::Tags::TagInstance;
	using Blam::Tags::Items::Weapon;

	bool CommandWeaponOffset(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1) {
			returnInfo = "Invalid argument, for a list of weapon name use the 'Weapon.List' command";
			return false;
		}

		auto &weaponVars = Modules::ModuleWeapon::Instance();
		auto weaponName = Arguments[0];

		uint16_t weaponIndex;
		if (Patches::Weapon::GetIndex(weaponName) != 0xFFFF) {
			weaponIndex = Patches::Weapon::GetIndex(weaponName);
		}
		else {
			returnInfo = "Invalid weapon name";
			return false;
		}

		auto *weapon = TagInstance(weaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();

		if (Arguments.size() != 4) {
			std::stringstream ss;
			ss << "Weapon: " << weaponName << ", I: " << weapon->FirstPersonWeaponOffset.I << ", J: " << weapon->FirstPersonWeaponOffset.J << ", K: " << weapon->FirstPersonWeaponOffset.K;
			returnInfo = ss.str();
			return false;
		}

		RealVector3D offset = { std::stof(Arguments[1]), std::stof(Arguments[2]), std::stof(Arguments[3]) }; // I, J, K

		// update offset
		weapon->FirstPersonWeaponOffset = offset;
		Patches::Weapon::SetOffsetModified(weaponName, offset);

		std::stringstream ss;
		ss << "Weapon: " << weaponName << ", I: " << std::stof(Arguments[1]) << ", J: " << std::stof(Arguments[2]) << ", K: " << std::stof(Arguments[3]);
		returnInfo = ss.str();

		return true;
	}

	bool CommandWeaponOffsetReset(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1) {
			returnInfo = "Invalid argument, for a list of weapon name use the 'Player.ListRenderWeapons' command";
			return false;
		}

		auto &weaponVars = Modules::ModuleWeapon::Instance();
		auto weaponName = Arguments[0];

		uint16_t weaponIndex;
		if (Patches::Weapon::GetIndex(weaponName) != 0xFFFF) {
			weaponIndex = Patches::Weapon::GetIndex(weaponName);
		}
		else {
			returnInfo = "Invalid weapon name";
			return false;
		}

		auto *weapon = TagInstance(weaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();

		weapon->FirstPersonWeaponOffset = Patches::Weapon::GetOffset("default", weaponName);
		Patches::Weapon::SetOffsetModified(weaponName, Patches::Weapon::GetOffset("default", weaponName));

		std::stringstream ss;
		ss << "Weapon: " << weaponName << " offset reset to default";
		returnInfo = ss.str();

		return true;
	}

	bool CommandSaveWeaponsConfig(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Weapon::ConfigSave(Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString);

		returnInfo = "Weapons config saved successfully";
		return true;
	}

	bool VariableWeaponConfigUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Weapon::ConfigLoad(Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString);

		returnInfo = "Weapons config file updated successfully";
		return true;
	}

	bool CommandWeaponList(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::stringstream ss;

		for (auto &entry : Patches::Weapon::GetIndices())
			ss << "Index: 0x" << std::hex << entry.second << ", Name: " << entry.first << std::endl;

		returnInfo = ss.str();
		return true;
	}
}

namespace Modules
{
	ModuleWeapon::ModuleWeapon() : ModuleBase("Weapon")
	{
		AddCommand("Offset", "weap_off", "This changes weapon offset", eCommandFlagsNone, CommandWeaponOffset, { "Weapon Name", "I Offset", "J Offset", "K Offset" });
		AddCommand("Offset.Reset", "weap_off_res", "This resets weapon offset to default", eCommandFlagsNone, CommandWeaponOffsetReset, { "Weapon Name" });
		AddCommand("Config.Save", "weap_cfg_sv", "This resets weapon offset to default", eCommandFlagsNone, CommandSaveWeaponsConfig);
		VarWeaponConfig = AddVariableString("Config", "weap_cfg", "The file weapon changes are saved to.", eCommandFlagsArchived, "weapons.cfg", VariableWeaponConfigUpdate);
		AddCommand("List", "weap_list", "Lists weapons", eCommandFlagsNone, CommandWeaponList);
	}
}