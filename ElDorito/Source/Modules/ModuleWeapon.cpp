#include "ModuleWeapon.hpp"
#include "../ElDorito.hpp"
#include "../Patches/Weapon.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"

namespace
{
	using Blam::Cache::StringIDCache;
	using Blam::Math::RealVector3D;
	using Blam::Tags::TagInstance;
	using Blam::Tags::Items::Weapon;

	auto IsMainMenu = (bool(*)())(0x531E90);

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

		auto weaponOffset = Patches::Weapon::GetOffsetByIndex(weaponIndex);

		if (Arguments.size() != 4) {
			std::stringstream ss;
			ss << "Weapon: " << weaponName << ", I: " << weaponOffset.I << ", J: " << weaponOffset.J << ", K: " << weaponOffset.K;
			returnInfo = ss.str();
			return true;
		}

		RealVector3D offset = { std::stof(Arguments[1]), std::stof(Arguments[2]), std::stof(Arguments[3]) }; // I, J, K

		// update offset
		weaponOffset = offset;
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

		weapon->FirstPersonWeaponOffset = Patches::Weapon::GetOffsetByName(weaponName);
		Patches::Weapon::SetOffsetModified(weaponName, Patches::Weapon::GetOffsetByName(weaponName));

		std::stringstream ss;
		ss << "Weapon: " << weaponName << " offset reset to default";
		returnInfo = ss.str();

		return true;
	}

	bool CommandSaveWeaponsConfig(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Weapon::Config::Save(Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString);

		returnInfo = "Weapons config saved successfully";
		return true;
	}

	bool VariableWeaponConfigUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Weapon::Config::Load(Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString);

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

	bool CommandGetEquippedWeaponInfo(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (IsMainMenu())
		{
			returnInfo = "This command is unavailable at the moment, please try again on another map";
			return false;
		}

		std::stringstream ss;

		auto equippedWeaponIndex = Patches::Weapon::GetEquippedWeaponIndex();
		auto equippedWeaponName = Patches::Weapon::GetEquippedWeaponName();

		auto *weapon = TagInstance(equippedWeaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
		auto equippedWeaponOffset = RealVector3D(weapon->FirstPersonWeaponOffset);

		ss << "Name: " << equippedWeaponName << std::endl;
		//ss << "Class: " << std::string(StringIDCache::Instance.GetString(weapon->WeaponClass)) << std::endl;
		//ss << "Acceleration Scale: " << weapon->AccelerationScale << std::endl;
		ss << "Index: 0x" << std::hex << equippedWeaponIndex << std::endl;
		ss << "First Person Weapon Offset: { " << equippedWeaponOffset.I << ", " << equippedWeaponOffset.J << ", " << equippedWeaponOffset.K << " }" << std::endl;
		//ss << "Acceleration Scale: " << weapon->AccelerationScale << std::endl;
		//ss << "Autoaim Angle: " << weapon->AutoaimAngle.Value << std::endl;
		//ss << "Magnetism Angle: " << weapon->MagnetismAngle.Value << std::endl;

		returnInfo = ss.str();
		return true;
	}

	bool CommandGetEquippedWeaponJSON(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (IsMainMenu())
		{
			returnInfo = "This command is unavailable at the moment, please try again on another map";
			return false;
		}

		auto equippedWeaponIndex = Patches::Weapon::GetEquippedWeaponIndex();
		auto equippedWeaponName = Patches::Weapon::GetEquippedWeaponName();

		auto *weapon = TagInstance(equippedWeaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
		auto equippedWeaponOffset = RealVector3D(weapon->FirstPersonWeaponOffset);

		std::stringstream temp;
		temp << "0x" << std::hex << equippedWeaponIndex;
		std::string Index = temp.str();

		std::string MultiplayerWeaponType, WeaponType, TrackingType, SpecialHUDType;

		switch (weapon->MultiplayerWeaponType)
		{
		case Blam::Tags::Items::Weapon::MultiplayerType::None:
			MultiplayerWeaponType = "None";
			break;
		case Blam::Tags::Items::Weapon::MultiplayerType::CTFFlag:
			MultiplayerWeaponType = "CTFFlag";
			break;
		case Blam::Tags::Items::Weapon::MultiplayerType::OddballBall:
			MultiplayerWeaponType = "OddballBall";
			break;
		case Blam::Tags::Items::Weapon::MultiplayerType::HeadhunterHead:
			MultiplayerWeaponType = "HeadhunterHead";
			break;
		case Blam::Tags::Items::Weapon::MultiplayerType::JuggernautPowerup:
			MultiplayerWeaponType = "JuggernautPowerup";
			break;
		}

		switch (weapon->WeaponType)
		{
		case Blam::Tags::Items::Weapon::Type::Undefined:
			WeaponType = "Undefined";
			break;
		case Blam::Tags::Items::Weapon::Type::Shotgun:
			WeaponType = "Shotgun";
			break;
		case Blam::Tags::Items::Weapon::Type::Needler:
			WeaponType = "Needler";
			break;
		case Blam::Tags::Items::Weapon::Type::PlasmaPistol:
			WeaponType = "PlasmaPistol";
			break;
		case Blam::Tags::Items::Weapon::Type::PlasmaRifle:
			WeaponType = "PlasmaRifle";
			break;
		case Blam::Tags::Items::Weapon::Type::RocketLauncher:
			WeaponType = "RocketLauncher";
			break;
		case Blam::Tags::Items::Weapon::Type::EnergySword:
			WeaponType = "EnergySword";
			break;
		case Blam::Tags::Items::Weapon::Type::SpartanLaser:
			WeaponType = "SpartanLaser";
			break;
		case Blam::Tags::Items::Weapon::Type::AssaultRifle:
			WeaponType = "AssaultRifle";
			break;
		case Blam::Tags::Items::Weapon::Type::BattleRifle:
			WeaponType = "BattleRifle";
			break;
		case Blam::Tags::Items::Weapon::Type::DMR:
			WeaponType = "DMR";
			break;
		case Blam::Tags::Items::Weapon::Type::Magnum:
			WeaponType = "Magnum";
			break;
		case Blam::Tags::Items::Weapon::Type::SniperRifle:
			WeaponType = "SniperRifle";
			break;
		case Blam::Tags::Items::Weapon::Type::SMG:
			WeaponType = "SMG";
			break;
		}

		switch (weapon->TrackingType)
		{
		case Blam::Tags::Items::Weapon::TrackingType::NoTracking:
			TrackingType = "NoTracking";
			break;
		case Blam::Tags::Items::Weapon::TrackingType::HumanTracking:
			TrackingType = "HumanTracking";
			break;
		case Blam::Tags::Items::Weapon::TrackingType::PlasmaTracking:
			TrackingType = "PlasmaTracking";
			break;
		}

		switch (weapon->SpecialHUDType)
		{
		case Blam::Tags::Items::Weapon::SpecialHUDType::NoOutline:
			SpecialHUDType = "NoOutline";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::Default:
			SpecialHUDType = "Default";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::Ammo:
			SpecialHUDType = "Ammo";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::Damage:
			SpecialHUDType = "Damage";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::Accuracy:
			SpecialHUDType = "Accuracy";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::RateOfFire:
			SpecialHUDType = "RateOfFire";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::Range:
			SpecialHUDType = "Range";
			break;
		case Blam::Tags::Items::Weapon::SpecialHUDType::Power:
			SpecialHUDType = "Power";
			break;
		}

		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("Name");
		jsonWriter.String(equippedWeaponName.c_str());
		jsonWriter.Key("Index");
		jsonWriter.String(Index.c_str());
		jsonWriter.Key("FirstPersonWeaponOffset");
		jsonWriter.StartArray();
		jsonWriter.String(std::to_string(equippedWeaponOffset.I).c_str());
		jsonWriter.String(std::to_string(equippedWeaponOffset.J).c_str());
		jsonWriter.String(std::to_string(equippedWeaponOffset.K).c_str());
		jsonWriter.EndArray();
		jsonWriter.Key("MultiplayerWeaponType");
		jsonWriter.String(MultiplayerWeaponType.c_str());
		jsonWriter.Key("WeaponType");
		jsonWriter.String(WeaponType.c_str());
		jsonWriter.Key("TrackingType");
		jsonWriter.String(TrackingType.c_str());
		jsonWriter.Key("SpecialHUDType");
		jsonWriter.String(SpecialHUDType.c_str());
		jsonWriter.EndObject();

		returnInfo = jsonBuffer.GetString();
		return true;
	}
}

namespace Modules
{
	ModuleWeapon::ModuleWeapon() : ModuleBase("Weapon")
	{
		AddCommand("Offset", "weap_off", "This changes weapon offset.", eCommandFlagsNone, CommandWeaponOffset, { "Weapon Name", "I Offset", "J Offset", "K Offset" });
		AddCommand("Offset.Reset", "weap_off_res", "This resets weapon offset to default.", eCommandFlagsNone, CommandWeaponOffsetReset, { "Weapon Name" });
		AddCommand("Config.Save", "weap_cfg_sv", "This saves weapon offset.", eCommandFlagsNone, CommandSaveWeaponsConfig);
		VarWeaponConfig = AddVariableString("Config", "weap_cfg", "The file weapon changes are saved to.", eCommandFlagsArchived, "weapons.cfg", VariableWeaponConfigUpdate);
		AddCommand("List", "weap_list", "Lists all weapons available in the mulg tag.", eCommandFlagsNone, CommandWeaponList);
		AddCommand("Equipped", "weap_equipped", "Gives info on the currently equipped weapon.", eCommandFlagsNone, CommandGetEquippedWeaponInfo);
		AddCommand("JSON", "weap_json", "Gives JSON formatted info on the currently equipped weapon.", eCommandFlagsHidden, CommandGetEquippedWeaponJSON);
	}
}