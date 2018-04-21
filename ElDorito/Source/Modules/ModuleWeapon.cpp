#include "ModuleWeapon.hpp"
#include "../ElDorito.hpp"
#include "../Patches/Weapon.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Tags/Items/DefinitionWeapon.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"

namespace
{
	using Blam::Cache::StringIDCache;
	using Blam::Math::RealVector3D;
	using Blam::Tags::TagInstance;
	using Blam::Tags::Items::Weapon;

	auto IsMainMenu = (bool(*)())(0x531E90);

	RealVector3D ToOffset(std::string I, std::string J, std::string K);
	void GetTypes(Blam::Tags::Items::Weapon *WeaponDefinition, std::string MultiplayerWeaponType, std::string WeaponType, std::string TrackingType, std::string SpecialHUDType);

	bool CommandWeaponOffset(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1) {
			returnInfo = "Invalid argument, for a list of weapon name use the 'Weapon.List' command.";
			return false;
		}

		Patches::Weapon::WeaponInfo weapon;

		if (Utils::String::ToLower(Arguments[0]) == "equipped") {
			weapon = Patches::Weapon::GetEquippedWeapon();
		}
		else
		{
			auto name = Arguments[0];
			weapon = Patches::Weapon::GetWeaponByName(name);
		}

		if (Arguments.size() != 4) {
			std::stringstream ss;
			ss << "Weapon: " << weapon.Name << ", I: " << weapon.Offset.Modified.I << ", J: " << weapon.Offset.Modified.J << ", K: " << weapon.Offset.Modified.K;
			returnInfo = ss.str();
			return true;
		}

		auto offset = ToOffset(Arguments[1], Arguments[2], Arguments[3]);

		// update offset
		Patches::Weapon::SetOffsetModified(weapon.Name, offset);

		std::stringstream ss;
		ss << "Weapon: " << weapon.Name << ", I: " << weapon.Offset.Modified.I << ", J: " << weapon.Offset.Modified.J << ", K: " << weapon.Offset.Modified.K;
		returnInfo = ss.str();

		return true;
	}

	bool CommandWeaponOffsetReset(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (IsMainMenu())
		{
			returnInfo = "This command is unavailable at the moment, please try again when not on the mainmenu.";
			return false;
		}

		if (Arguments.size() < 1) {
			returnInfo = "Invalid argument, for a list of weapon name use the 'Weapon.List' command.";
			return false;
		}

		Patches::Weapon::WeaponInfo weapon;

		if (Utils::String::ToLower(Arguments[0]) == "equipped") {
			weapon = Patches::Weapon::GetEquippedWeapon();
		}
		else
		{
			auto name = Arguments[0];
			weapon = Patches::Weapon::GetWeaponByName(name);
		}

		auto *weaponDefinition = TagInstance(weapon.Index).GetDefinition<Blam::Tags::Items::Weapon>();

		weaponDefinition->FirstPersonWeaponOffset = Patches::Weapon::GetOffsetByName(true, weapon.Name);
		Patches::Weapon::SetOffsetModified(weapon.Name, Patches::Weapon::GetOffsetByName(true, weapon.Name));

		std::stringstream ss;
		ss << "Weapon: " << weapon.Name << " offset reset to default.";
		returnInfo = ss.str();

		return true;
	}

	bool CommandLoadWeaponsJSON(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		bool changeJson = false;
		if (Arguments.size() > 0)
			for (auto&& name : Modules::ModuleWeapon::Instance().WeaponsJSONList)
				if (Arguments[0] == name)
					changeJson = true;

		if (changeJson)
			Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString = Arguments[0];

		auto file = Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString;
		if (Utils::String::ToLower(file) == "default")
			Patches::Weapon::SetOffsetDefaultAll();
		else
			Patches::Weapon::Config::LoadJSON(file);

		returnInfo = "Weapons json loaded successfully.";
		return true;
	}

	bool CommandSaveWeaponsJSON(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (IsMainMenu())
		{
			returnInfo = "This command is unavailable at the moment, please try again when not on the mainmenu.";
			return false;
		}

		Patches::Weapon::Config::SaveJSON(Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString);

		returnInfo = "Weapons json saved successfully.";
		return true;
	}

	bool CommandWeaponList(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (IsMainMenu())
		{
			returnInfo = "This command is unavailable at the moment, please try again when not on the mainmenu.";
			return false;
		}

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
			returnInfo = "This command is unavailable at the moment, please try again when not on the mainmenu.";
			return false;
		}

		auto equippedWeaponIndex = Patches::Weapon::GetEquippedWeaponIndex();

		if (equippedWeaponIndex == 0xFFFF)
		{
			returnInfo = "Equipped weapon not supported.";
			return false;
		}

		auto equippedWeaponName = Patches::Weapon::GetEquippedWeaponName();

		auto *weaponDefinition = TagInstance(equippedWeaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
		auto equippedWeaponOffset = weaponDefinition->FirstPersonWeaponOffset;

		std::stringstream index_;
		index_ << "0x" << std::hex << equippedWeaponIndex;
		std::string Index = index_.str();

		std::string MultiplayerWeaponType, WeaponType, TrackingType, SpecialHUDType;
		GetTypes(weaponDefinition, MultiplayerWeaponType, WeaponType, TrackingType, SpecialHUDType);

		std::string format;
		if (Arguments.size() == 0)
			format = "normal";
		else
			format = Utils::String::ToLower(Arguments[0]);

		if (format == "normal")
		{
			std::stringstream ss;

			ss << "Name: " << equippedWeaponName << std::endl;
			//ss << "Class: " << std::string(StringIDCache::Instance.GetString(weapon->WeaponClass)) << std::endl;
			//ss << "Acceleration Scale: " << weapon->AccelerationScale << std::endl;
			ss << "Index: 0x" << std::hex << equippedWeaponIndex << std::endl;
			ss << "First Person Weapon Offset: { " << equippedWeaponOffset.I << ", " << equippedWeaponOffset.J << ", " << equippedWeaponOffset.K << " }" << std::endl;
			//ss << "Acceleration Scale: " << weapon->AccelerationScale << std::endl;
			//ss << "Autoaim Angle: " << weapon->AutoaimAngle.Value << std::endl;
			//ss << "Magnetism Angle: " << weapon->MagnetismAngle.Value << std::endl;

			returnInfo = ss.str();
		}
		else if (format == "json")
		{
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
		}
		else if (format == "csv")
		{
			std::stringstream ss;

			ss << equippedWeaponName.c_str() << ",";
			ss << Index.c_str() << ",";
			ss << std::to_string(equippedWeaponOffset.I).c_str() << ",";
			ss << std::to_string(equippedWeaponOffset.J).c_str() << ",";
			ss << std::to_string(equippedWeaponOffset.K).c_str() << ",";
			ss << MultiplayerWeaponType.c_str() << ",";
			ss << WeaponType.c_str() << ",";
			ss << TrackingType.c_str() << ",";
			ss << SpecialHUDType.c_str();

			returnInfo = ss.str();
		}

		return true;
	}

	bool CommandGetEquippedWeaponJSON(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (IsMainMenu())
		{
			returnInfo = "This command is unavailable at the moment, please try again when not on the mainmenu.";
			return false;
		}

		auto equippedWeaponIndex = Patches::Weapon::GetEquippedWeaponIndex();
		auto equippedWeaponName = Patches::Weapon::GetEquippedWeaponName();

		auto *weaponDefinition = TagInstance(equippedWeaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
		auto equippedWeaponOffset = weaponDefinition->FirstPersonWeaponOffset;

		std::stringstream temp;
		temp << "0x" << std::hex << equippedWeaponIndex;
		std::string Index = temp.str();

		std::string MultiplayerWeaponType, WeaponType, TrackingType, SpecialHUDType;
		GetTypes(weaponDefinition, MultiplayerWeaponType, WeaponType, TrackingType, SpecialHUDType);

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
	
	bool CommandListWeaponsJSON(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		Patches::Weapon::Config::CreateList();

		// Return a comma-separated list
		for (auto&& name : Modules::ModuleWeapon::Instance().WeaponsJSONList)
		{
			if (returnInfo.length() > 0)
				returnInfo += ',';
			returnInfo += name;
		}
		return true;
	}

	RealVector3D ToOffset(std::string I, std::string J, std::string K)
	{
		return RealVector3D(std::stof(I), std::stof(J), std::stof(K));
	}

	void GetTypes(Blam::Tags::Items::Weapon *WeaponDefinition, std::string MultiplayerWeaponType, std::string WeaponType, std::string TrackingType, std::string SpecialHUDType)
	{
		switch (WeaponDefinition->MultiplayerWeaponType)
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

		switch (WeaponDefinition->WeaponType)
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

		switch (WeaponDefinition->TrackingType)
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

		switch (WeaponDefinition->SpecialHUDType)
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
	}
}

namespace Modules
{
	ModuleWeapon::ModuleWeapon() : ModuleBase("Weapon")
	{
		AddCommand("Offset", "weap_off", "This changes weapon offset.", eCommandFlagsNone, CommandWeaponOffset, { "Weapon Name", "I Offset", "J Offset", "K Offset" });
		AddCommand("Offset.Reset", "weap_off_res", "This resets weapon offset to default.", eCommandFlagsNone, CommandWeaponOffsetReset, { "Weapon Name" });
		//VarAutoSaveOnMapLoad = AddVariableInt("AutoSaveOnMapLoad", "weap_auto_save", "This determines if the offsets get saved on map load.", eCommandFlagsArchived, 0);
		VarWeaponJSON = AddVariableString("JSON.File", "weap_json_file", "The file weapon changes are saved to.", eCommandFlagsArchived, "default");
		AddCommand("JSON.Load", "weap_json_load", "This loads weapon offset.", eCommandFlagsNone, CommandLoadWeaponsJSON);
		AddCommand("JSON.Save", "weap_json_save", "This saves weapon offset.", eCommandFlagsNone, CommandSaveWeaponsJSON);
		AddCommand("JSON.List", "weap_json_list", "This lists all available weapon offset configs.", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsHidden), CommandListWeaponsJSON);
		AddCommand("List", "weap_list", "Lists all weapons available in the mulg tag.", eCommandFlagsNone, CommandWeaponList);
		AddCommand("Equipped", "weap_equipped", "Gives info on the currently equipped weapon.", eCommandFlagsNone, CommandGetEquippedWeaponInfo, { "Format: null, json, csv" });
	}
}