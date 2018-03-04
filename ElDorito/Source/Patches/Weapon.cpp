#include <iomanip>
#include <fstream>
#include <map>

#include <wchar.h>
#include <stdio.h>
#include <boost\filesystem.hpp>

#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Console.hpp"

#include "../Modules/ModuleWeapon.hpp"
#include "../Modules/ModuleServer.hpp"

#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTime.hpp"

#include "../Blam/Cache/StringIdCache.hpp"

#include "../Blam/Math/RealVector3D.hpp"

#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Globals/CacheFileGlobalTags.hpp"
#include "../Blam/Tags/Items/DefinitionWeapon.hpp"

#include "../Patches/Core.hpp"
#include "../Patches/Weapon.hpp"

#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/prettywriter.h"

namespace
{
	int __cdecl DualWieldHook(unsigned short objectIndex);
	void DualWieldSprintInputHook();
	void DualWieldScopeLevelHook();
	int DualWieldEquipmentCountHook(uint32_t unitIndex, short equipmentIndex);

	void weapon_apply_firing_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex);
	void weapon_apply_movement_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex);
	void weapon_apply_turning_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex);

	bool SupportWeaponStartHook(int weaponObjectIndex);
}

namespace Patches::Weapon
{
	using Blam::Math::RealVector3D;
	using Blam::Tags::TagInstance;

	bool IsMainMenu;

	bool AddSupportedWeapons();
	std::vector<WeaponInfo> weapon_infos;

	std::string JSONName;

	//Callback for when the loading screen back to the main menu finishes. We use this to determine when to start a new vote.
	void MapLoadedCallback(const char *mapPath)
	{
		std::string currentMap = mapPath;
		auto separatorIndex = currentMap.find_first_of("\\/");
		auto mapName = currentMap.substr(separatorIndex + 1);

		if (mapName == "mainmenu")
		{
			IsMainMenu = true;
		}
		else
		{
			IsMainMenu = false;

			AddSupportedWeapons();
			SetOffsetDefaultAll();

			Modules::CommandMap::Instance().ExecuteCommand("Weapon.JSON.Load");

			//if (Modules::ModuleWeapon::Instance().VarAutoSaveOnMapLoad->ValueInt == 1)
			//	Config::SaveJSON(JSONName);
		}
	}

	void ApplyAll()
	{
		JSONName = Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString;
		Config::CreateList();
		Config::LoadJSON(JSONName);

		Patches::Core::OnMapLoaded(MapLoadedCallback);

		// Force descoping for all weapons
		Pointer::Base(0x73F1E6).Write<uint8_t>(0);

		Hook(0x761550, DualWieldHook).Apply();
		Hook(0x06DFBB, DualWieldSprintInputHook).Apply();
		Hook(0x1D50CB, DualWieldScopeLevelHook).Apply();
		Hook(0x7A21D4, DualWieldEquipmentCountHook, HookFlags::IsCall).Apply();

		// fix recoil - use fire rate acceleration instead. TODO: find correct decay for h3
		Pointer(0x0B603E7 + 5).Write<int>(0x214);

		// disable bloom for non-p2w weapons
		Hook(0x75F0F1, weapon_apply_firing_penalty_hook, HookFlags::IsCall).Apply();
		Hook(0x761197, weapon_apply_movement_penalty_hook, HookFlags::IsCall).Apply();
		Hook(0x74A8B7, weapon_apply_turning_penalty_hook, HookFlags::IsCall).Apply();

		// allow spawning with primary support weapon and keep secondary weapon
		Hook(0x13809E, SupportWeaponStartHook, HookFlags::IsCall).Apply();
	}

	std::string OffsetToString(RealVector3D offset)
	{
		return std::to_string(offset.I) + ", " + std::to_string(offset.J) + ", " + std::to_string(offset.K);
	}
	void PrintDebugText(WeaponInfo weapon)
	{
		Console::WriteLine(weapon.Name);
		Console::WriteLine("{");
		Console::WriteLine("\t" + std::to_string(weapon.Index) + ",");
		Console::WriteLine("\t" + OffsetToString(weapon.Offset.Default) + ",");
		Console::WriteLine("\t" + OffsetToString(weapon.Offset.Modified));
		Console::WriteLine("}");
	}

	void ApplyAfterTagsLoaded()
	{
		//// Debug info
		//for (auto weapon : weaponNames)
		//	Console::WriteLine(weapon.first + ", " + weapon.second);
	}

	std::map<std::string, uint16_t> GetIndices()
	{
		std::map<std::string, uint16_t> weaponIndices;
		for (auto &weapon : weapon_infos)
			weaponIndices.emplace(weapon.Name, weapon.Index);
		return weaponIndices;
	}

	WeaponInfo GetWeaponByName(std::string &weaponName)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weaponName)
				return weapon;
		return WeaponInfo();
	}

	WeaponInfo GetWeaponByIndex(uint16_t &weaponIndex)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Index == weaponIndex)
				return weapon;
		return WeaponInfo();
	}

	WeaponInfo GetWeaponByTagName(std::string &weaponTagName)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.TagName == weaponTagName)
				return weapon;
		return WeaponInfo();
	}
	
	WeaponInfo GetWeapon(WeaponInfo &weapon_)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weapon_.Name || weapon.Index == weapon_.Index || weapon.TagName == weapon_.TagName)
				return weapon;
		return WeaponInfo();
	}

	uint16_t GetIndex(WeaponInfo &weapon_)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weapon_.Name || weapon.Index == weapon_.Index || weapon.TagName == weapon_.TagName)
				return weapon.Index;
		return 0xFFFF;
	}
	
	std::string GetName(WeaponInfo &weapon_)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weapon_.Name || weapon.Index == weapon_.Index || weapon.TagName == weapon_.TagName)
				return weapon.Name;
		return "Weapon not supported.";
	}

	std::string GetTagName(WeaponInfo &weapon_)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weapon_.Name || weapon.Index == weapon_.Index || weapon.TagName == weapon_.TagName)
				return weapon.TagName;
		return "Weapon\\not\\supported";
	}

	WeaponInfo::Offsets GetOffsets(WeaponInfo &weapon_)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weapon_.Name || weapon.Index == weapon_.Index || weapon.TagName == weapon_.TagName)
				return weapon.Offset;
		return WeaponInfo().Offset;
	}

	WeaponInfo GetEquippedWeapon()
	{
		WeaponInfo weapon;
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
			return weapon;

		int playerIdx = session->MembershipInfo.FindFirstPlayer();
		auto playerDatum = &Blam::Players::GetPlayers()[playerIdx];
		auto playerObject = Pointer(Blam::Objects::GetObjects()[playerDatum->SlaveUnit].Data);
		if (playerObject)
		{
			auto equippedWeaponIndex = playerObject(0x2CA).Read<uint8_t>();
			if (equippedWeaponIndex != -1)
			{
				auto equippedWeaponObjectIndex = playerObject(0x2D0 + 4 * equippedWeaponIndex).Read<uint32_t>();
				auto equippedWeaponObjectPtr = Pointer(Blam::Objects::GetObjects()[equippedWeaponObjectIndex].Data);
				if (equippedWeaponObjectPtr)
				{
					weapon.Index = Pointer(equippedWeaponObjectPtr).Read<uint32_t>();
					weapon.Name = GetName(weapon);
					weapon.TagName = GetTagName(weapon);
					weapon.Offset = GetOffsets(weapon);
				}
			}
		}
		return weapon;
	}

	uint16_t GetEquippedWeaponIndex()
	{
		return GetIndex(GetEquippedWeapon());
	}

	std::string GetEquippedWeaponName()
	{
		return GetName(GetEquippedWeapon());
	}

	std::string GetEquippedWeaponTagName()
	{
		return GetTagName(GetEquippedWeapon());
	}

	WeaponInfo::Offsets GetEquippedWeaponOffsets()
	{
		return GetOffsets(GetEquippedWeapon());
	}

	RealVector3D GetOffsetByName(bool isDefault, std::string &weaponName)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weaponName)
				return (isDefault ? weapon.Offset.Default : weapon.Offset.Modified);

		return { -0xFFF, -0xFFF, -0xFFF };
	}

	RealVector3D GetOffsetByIndex(bool isDefault, uint16_t &weaponIndex)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Index == weaponIndex)
				return (isDefault ? weapon.Offset.Default : weapon.Offset.Modified);

		return { -0xFFF, -0xFFF, -0xFFF };
	}

	RealVector3D GetOffsetByTagName(bool isDefault, std::string &weaponName)
	{
		for (auto &weapon : weapon_infos)
			if (weapon.Name == weaponName)
				return (isDefault ? weapon.Offset.Default : weapon.Offset.Modified);

		return { -0xFFF, -0xFFF, -0xFFF };
	}

	void SetDefaultOffsets()
	{
		for (auto &weapon : weapon_infos)
		{
			if (weapon.Index != 0xFFFF)
			{
				auto *weaponDefinition = TagInstance(weapon.Index).GetDefinition<Blam::Tags::Items::Weapon>();
				if (weaponDefinition)
					weapon.Offset.Default = weaponDefinition->FirstPersonWeaponOffset;
			}
		}
	}

	bool SetOffsetDefaultAll()
	{
		if (IsMainMenu) return false;

		for (auto &weapon : weapon_infos)
			SetOffsetModified(weapon.Name, weapon.Offset.Default);

		return true;
	}

	bool SetOffsetDefault(std::string &weaponName)
	{
		if (IsMainMenu) return false;

		for (auto &weapon : weapon_infos)
			if (weapon.Name == weaponName)
				SetOffsetModified(weapon.Name, weapon.Offset.Default);

		return true;
	}

	bool SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset)
	{

		if (IsMainMenu) return false;

		for (auto &weapon : weapon_infos)
		{
			if (weapon.Name == weaponName)
			{
				auto *weaponDefinition = TagInstance(weapon.Index).GetDefinition<Blam::Tags::Items::Weapon>();
				if (weaponDefinition)
					weaponDefinition->FirstPersonWeaponOffset = weapon.Offset.Modified = weaponOffset;
			}
		}

		return true;
	}

	bool AddSupportedWeapons()
	{
		std::ifstream in("mods/weapons/supported_weapons.json", std::ios::in | std::ios::binary);
		if (in && in.is_open())
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize((unsigned int)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			rapidjson::Document document;
			if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
			{
				if (!document.HasMember("supported_weapons")) return false;
				const rapidjson::Value& weaponNames = document["supported_weapons"];

				for (rapidjson::SizeType i = 0; i < weaponNames.Size(); i++)
				{
					const rapidjson::Value& weaponsObject = weaponNames[i];
					if (!weaponsObject.HasMember("name") || !weaponsObject.HasMember("tagname")) continue;

					std::string weaponname = weaponsObject["name"].GetString();
					std::string tagname = weaponsObject["tagname"].GetString();

					WeaponInfo weapon;
					weapon.Name = weaponname;
					weapon.TagName = tagname;

					try {
						uint16_t tagindex = TagInstance::Find('weap', tagname.c_str()).Index;
						if (tagindex != 0xFFFF) {
							weapon.Index = tagindex;
							weapon_infos.push_back(weapon);
						}
					}
					catch (const std::exception&) {
						std::stringstream ss;
						ss << "Unable to add " << weaponname.c_str() << ", " << tagname.c_str() << " to supported list." << std::endl;
						ss << "Please check the tagname is correct." << std::endl;
						Console::WriteLine(ss.str());
					}
				}
			}
		}

		return true;
	}

	namespace Config
	{
		bool CreateList()
		{
			Modules::ModuleWeapon::Instance().WeaponsJSONList.clear();

			if (!boost::filesystem::exists("mods/weapons/offsets.json"))
			{
				std::ofstream out("mods/weapons/offsets.json", std::ios::trunc);
				if (!out.is_open())
					return false;

				rapidjson::StringBuffer jsonBuffer;
				rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

				jsonWriter.StartObject();
				jsonWriter.Key("offsets");
				jsonWriter.StartArray();
				jsonWriter.StartObject();

				jsonWriter.Key("id");
				jsonWriter.String(JSONName.c_str());

				jsonWriter.EndObject();
				jsonWriter.EndArray();
				jsonWriter.EndObject();

				out << jsonBuffer.GetString();
				if (out.fail())
					return false;

				out.flush();
				out.close();
			}

			std::ifstream in("mods/weapons/offsets.json", std::ios::in | std::ios::binary);
			if (in && in.is_open())
			{
				std::string contents;
				in.seekg(0, std::ios::end);
				contents.resize((unsigned int)in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&contents[0], contents.size());
				in.close();

				rapidjson::Document document;
				if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
				{
					if (!document.HasMember("offsets"))
						return false;

					const rapidjson::Value& offsets = document["offsets"];
					for (rapidjson::SizeType i = 0; i < offsets.Size(); i++)
					{
						const rapidjson::Value& offsetObject = offsets[i];
						if (!offsetObject.HasMember("id"))
							continue;

						std::string offsetId = offsetObject["id"].GetString();
						Modules::ModuleWeapon::Instance().WeaponsJSONList.push_back(offsetId.c_str());
					}
				}
			}

			return true;
		}

		bool LoadJSON(std::string Name)
		{
			// Load offsets from json file
			std::ifstream in("mods/weapons/offsets/" + Name + ".json", std::ios::in | std::ios::binary);
			if (!in || !in.is_open())
				return false;

			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize((unsigned int)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			SetOffsetDefaultAll();

			rapidjson::Document document;
			if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
			{
				if (!document.HasMember("Weapons"))
					return false;

				const rapidjson::Value& weaps = document["Weapons"];
				for (rapidjson::SizeType i = 0; i < weaps.Size(); i++)
				{
					const rapidjson::Value& weapObject = weaps[i];
					if (!weapObject.HasMember("Name") || !weapObject.HasMember("Offset"))
						continue;

					std::string weapName = weapObject["Name"].GetString();
					const rapidjson::Value& offsets = weapObject["Offset"];

					SetOffsetModified(weapName, RealVector3D(offsets[0].GetFloat(), offsets[1].GetFloat(), offsets[2].GetFloat()));
				}
			}

			return true;
		}

		bool SaveJSON(std::string Name)
		{
			CreateList();

			Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString = Name;

			bool IsCreated = false;
			for (std::string offsetName : Modules::ModuleWeapon::Instance().WeaponsJSONList)
				if (Name == offsetName)
					IsCreated = true;

			if (!IsCreated)
			{
				std::ifstream in("mods/weapons/offsets.json", std::ios::in | std::ios::binary);
				if (in && in.is_open())
				{
					std::string contents;
					in.seekg(0, std::ios::end);
					contents.resize((unsigned int)in.tellg());
					in.seekg(0, std::ios::beg);
					in.read(&contents[0], contents.size());
					in.close();

					rapidjson::Document document;
					if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
					{
						if (!document.HasMember("offsets")) return false;

						std::ofstream out("mods/weapons/offsets.json", std::ios::trunc);
						if (!out.is_open()) return false;

						rapidjson::StringBuffer jsonBuffer;
						rapidjson::PrettyWriter<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
						jsonWriter.StartObject();
						jsonWriter.Key("offsets");

						jsonWriter.StartArray();
						const rapidjson::Value& offsets = document["offsets"];
						for (rapidjson::SizeType i = 0; i < offsets.Size(); i++)
						{
							const rapidjson::Value& offsetObject = offsets[i];
							if (!offsetObject.HasMember("id"))
								continue;

							std::string offsetId = offsetObject["id"].GetString();

							jsonWriter.StartObject();
							jsonWriter.Key("id");
							jsonWriter.String(offsetId.c_str());
							jsonWriter.EndObject();
						}

						jsonWriter.StartObject();
						jsonWriter.Key("id");
						jsonWriter.String(Name.c_str());
						jsonWriter.EndObject();

						jsonWriter.EndArray();
						jsonWriter.EndObject();

						out << jsonBuffer.GetString();
						if (out.fail())
							return false;

						out.flush();
						out.close();
					}
				}
			}

			std::ofstream out("mods/weapons/offsets/" + Name + ".json", std::ios::trunc);
			if (!out.is_open() || Utils::String::ToLower(Name) == "default")
				return false;

			rapidjson::StringBuffer jsonBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> jsonWriter(jsonBuffer);


			jsonWriter.StartObject();
			jsonWriter.Key("Weapons");

			jsonWriter.StartArray();
			for (auto &weapon : weapon_infos)
			{
				jsonWriter.StartObject();

				jsonWriter.Key("Name");
				jsonWriter.String(weapon.Name.c_str());

				jsonWriter.Key("Offset");
				jsonWriter.StartArray();
				jsonWriter.SetMaxDecimalPlaces(6);
				jsonWriter.Double(weapon.Offset.Modified.I);
				jsonWriter.Double(weapon.Offset.Modified.J);
				jsonWriter.Double(weapon.Offset.Modified.K);
				jsonWriter.EndArray();

				jsonWriter.EndObject();
			}

			jsonWriter.EndArray();
			jsonWriter.EndObject();

			out << jsonBuffer.GetString();
			if (out.fail()) return false;

			out.flush();
			out.close();

			return true;
		}
	}
}

namespace
{
	bool UnitIsDualWielding(Blam::DatumHandle unitHandle)
	{
		if (!unitHandle)
			return false;
		auto objectHeaderArrayPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		auto unitDatumPtr = objectHeaderArrayPtr(0x44)[0](unitHandle.Index * 0x10)(0xC)[0];

		if (!unitDatumPtr)
			return false;

		auto dualWieldWeaponIndex = unitDatumPtr(0x2CB).Read<int8_t>();

		if (dualWieldWeaponIndex < 0 || dualWieldWeaponIndex >= 4)
			return false;

		typedef uint32_t(*UnitGetWeaponPtr)(uint32_t unitObject, short weaponIndex);
		auto UnitGetWeapon = reinterpret_cast<UnitGetWeaponPtr>(0xB454D0);

		return UnitGetWeapon(unitHandle, dualWieldWeaponIndex) != 0xFFFFFFFF;
	}

	bool PlayerIsDualWielding(Blam::DatumHandle playerIndex)
	{
		auto &players = Blam::Players::GetPlayers();
		return UnitIsDualWielding(players[playerIndex].SlaveUnit);
	}

	bool LocalPlayerIsDualWielding()
	{
		auto localPlayer = Blam::Players::GetLocalPlayer(0);
		if (localPlayer == Blam::DatumHandle::Null)
			return false;
		return PlayerIsDualWielding(localPlayer);
	}

	uint32_t GetObjectDataAddress(uint32_t objectDatum)
	{
		uint32_t objectIndex = objectDatum & UINT16_MAX;
		Pointer objectHeaderPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		uint32_t objectAddress = objectHeaderPtr(0x44).Read<uint32_t>() + 0xC + objectIndex * 0x10;
		return *(uint32_t*)(objectAddress);
	}

	int __cdecl DualWieldHook(unsigned short objectIndex)
	{
		using Blam::Tags::TagInstance;
		using Blam::Tags::Items::Weapon;

		if (!Modules::ModuleServer::Instance().VarServerDualWieldEnabledClient->ValueInt)
			return 0;

		auto index = *(uint32_t*)GetObjectDataAddress(objectIndex);
		auto *weaponDefinition = TagInstance(index).GetDefinition<Weapon>();

		return ((int32_t)weaponDefinition->WeaponFlags1 & (int32_t)Weapon::Flags1::CanBeDualWielded) != 0;
	}

	__declspec(naked) void DualWieldSprintInputHook()
	{
		__asm
		{
			push	eax
			call	LocalPlayerIsDualWielding
			test	al, al
			pop		eax
			jz		enable // leave sprint enabled(for now) if not dual wielding
			and		ax, 0FEFFh // disable by removing the 8th bit indicating no sprint input press
			enable :
			mov		dword ptr ds : [esi + 8], eax
				mov		ecx, edi
				push	046DFC0h
				ret
		}
	}

	// scope level is an int16 with -1 indicating no scope, 0 indicating first level, 1 indicating second level etc.
	__declspec(naked) void DualWieldScopeLevelHook()
	{
		__asm
		{
			mov		word ptr ds : [edi + esi + 32Ah], 0FFFFh // no scope by default
			push	eax
			push	ecx
			call	LocalPlayerIsDualWielding
			test	al, al
			pop		ecx
			pop		eax
			jnz		noscope // prevent scoping when dual wielding
			mov		word ptr ds : [edi + esi + 32Ah], ax // otherwise use intended scope level
			noscope :
			push	05D50D3h
				ret
		}
	}

	int DualWieldEquipmentCountHook(uint32_t unitIndex, short equipmentIndex)
	{
		typedef int(__cdecl* GetEquipmentCountFunc)(uint32_t unitIndex, short equipmentIndex);
		GetEquipmentCountFunc GetEquipmentCount = reinterpret_cast<GetEquipmentCountFunc>(0xB440F0);
		return GetEquipmentCount(unitIndex, equipmentIndex);
	}

	enum ProjectilePenaltyType
	{
		eProjectilePenalty_Firing,
		eProjectilePenalty_Moving,
		eProjectilePenalty_Turning
	};

	bool weapon_should_apply_bloom(uint32_t weaponObjectIndex, int barrelIndex, ProjectilePenaltyType type)
	{
		const auto weapon_is_held = (bool(*)(uint32_t weaponObjectIndex))(0x00B63EA0);
		const auto weapon_is_being_dual_wielded = (bool(*)(uint32_t weaponObjectIndex))(0xB64050);
		const auto weapon_get_parent_unit = (uint32_t(*)(uint32_t weaponObjectIndex))(0x00B63030);

		using Blam::Tags::Items::Weapon;

		if (!weapon_is_held(weaponObjectIndex))
			return false;

		auto weaponObject = Blam::Objects::Get(weaponObjectIndex);
		if (!weaponObject)
			return false;

		auto unitObject = Blam::Objects::Get(weapon_get_parent_unit(weaponObjectIndex));
		if (!unitObject)
			return false;

		auto isCrouching = *(float*)((uint8_t*)unitObject + 0x418) > 0;
		// auto isDualWielding = weapon_is_being_dual_wielded(weaponObjectIndex);

		auto weaponDefinition = Blam::Tags::TagInstance(weaponObject->TagIndex).GetDefinition<Weapon>();

		auto functionIndex = 0;
		switch (type)
		{
		case eProjectilePenalty_Firing:
			functionIndex = isCrouching ? 0 : 1;
			break;
		case eProjectilePenalty_Moving:
			functionIndex = 2;
			break;
		case eProjectilePenalty_Turning:
			functionIndex = 3;
			break;
		}

		if (barrelIndex < 0 || barrelIndex >= weaponDefinition->Barrels.Count)
			return false;

		/* dual-wield blocks are always empty
		if (isDualWielding)
			return (&weaponDefinition->Barrels[barrelIndex].DualFiringPenaltyFunction)[functionIndex].Count > 0;*/
		return (&weaponDefinition->Barrels[barrelIndex].FiringPenaltyFunction)[functionIndex].Count > 0;
	}

	void __cdecl weapon_apply_firing_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex)
	{
		const auto weapon_apply_firing_penalty = (void(*)(uint32_t weaponObjectIndex, int barrelIndex))(0x00B5C2E0);
		if (weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Firing))
		{
			weapon_apply_firing_penalty(weaponObjectIndex, barrelIndex);
		}
		else
		{
			auto weaponObject = Blam::Objects::Get(weaponObjectIndex);
			if (weaponObject)
			{
				auto weaponDefinition = Blam::Tags::TagInstance(weaponObject->TagIndex).GetDefinition<Blam::Tags::Items::Weapon>();
				if (barrelIndex < weaponDefinition->Barrels.Count)
				{
					auto &error = *(float*)((uint8_t*)weaponObject + 0x220 + 0x34 * barrelIndex);
					error += weaponDefinition->Barrels[barrelIndex].Unknown2 * Blam::Time::GetSecondsPerTick();
					if (error > 1.0f) error = 1.0f;
					if (error < 0.0f) error = 0.0f;
				}
			}
		}
	}

	void __cdecl weapon_apply_movement_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex)
	{
		const auto weapon_apply_movement_penalty = (void(*)(uint32_t weaponObjectIndex, int barrelIndex))(0x00B5C450);
		if (weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Moving))
			return weapon_apply_movement_penalty(weaponObjectIndex, barrelIndex);
	}

	void __cdecl weapon_apply_turning_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex)
	{
		const auto weapon_apply_turning_penalty = (void(*)(uint32_t weaponObjectIndex, int barrelIndex))(0x00B5C630);
		if (weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Turning))
			return weapon_apply_turning_penalty(weaponObjectIndex, barrelIndex);
	}

	bool SupportWeaponStartHook(int weaponObjectIndex)
	{
		return false;
	}
}
