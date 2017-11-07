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

#include "../Blam/Cache/StringIdCache.hpp"

#include "../Blam/Math/RealVector3D.hpp"

#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Globals/CacheFileGlobalTags.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"

#include "../Patches/Core.hpp"
#include "../Patches/Weapon.hpp"

#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"

namespace
{
	int __cdecl DualWieldHook(unsigned short objectIndex);
	void DualWieldSprintInputHook();
	void DualWieldScopeLevelHook();
	int DualWieldEquipmentCountHook(uint32_t unitIndex, short equipmentIndex);

	void weapon_apply_firing_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex);
	void weapon_apply_movement_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex);
	void weapon_apply_turning_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex);
	bool weapon_update_bloom_hook(uint32_t weaponObjectIndex, int barrelIndex);
}

namespace Patches::Weapon
{
	using Blam::Math::RealVector3D;
	using Blam::Tags::TagInstance;

	bool IsNotMainMenu;

	std::string JSONName;
	std::map<std::string, uint16_t> weaponIndices;
	std::map<std::string, RealVector3D> weaponOffsetsDefault;
	std::map<std::string, RealVector3D> weaponOffsetsModified;

	//Callback for when the loading screen back to the main menu finishes. We use this to determine when to start a new vote.
	void MapLoadedCallback(const char *mapPath)
	{
		std::string currentMap = mapPath;
		auto separatorIndex = currentMap.find_first_of("\\/");
		auto mapName = currentMap.substr(separatorIndex + 1);

		if (mapName == "mainmenu")
		{
			IsNotMainMenu = false;
		}
		else
		{
			IsNotMainMenu = true;

			SetDefaultOffsets();

			if (Modules::ModuleWeapon::Instance().VarAutoSaveOnMapLoad->ValueInt == 1 && !weaponOffsetsModified.empty())
			{
				Config::SaveJSON(JSONName);
			}

			Modules::CommandMap::Instance().ExecuteCommand("Weapon.JSON.Load");
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

		// fix recoil
		Pointer(0x0B603E7 + 5).Write<int>(0x214);

		// disable bloom for non-p2w weapons
		Hook(0x75F0F1, weapon_apply_firing_penalty_hook, HookFlags::IsCall).Apply();
		Hook(0x761197, weapon_apply_movement_penalty_hook, HookFlags::IsCall).Apply();
		Hook(0x74A8B7, weapon_apply_turning_penalty_hook, HookFlags::IsCall).Apply();
		Hook(0x7611A1, weapon_update_bloom_hook, HookFlags::IsCall).Apply();
	}

	void ApplyAfterTagsLoaded()
	{
		using Blam::Tags::Game::Globals;
		using Blam::Tags::Game::MultiplayerGlobals;
		using Blam::Tags::Globals::CacheFileGlobalTags;

		auto *cfgt = TagInstance(0x0000).GetDefinition<CacheFileGlobalTags>();

		auto matgTagIndex = -1;

		for (auto &entry : cfgt->GlobalsTags)
		{
			if (entry.Tag.GroupTag == 'matg')
			{
				matgTagIndex = entry.Tag.TagIndex;
				break;
			}
		}

		if (matgTagIndex == -1)
			throw std::exception("globals tag (matg) not reference in cache_file_global_tags!");

		auto *matg = TagInstance(matgTagIndex).GetDefinition<Globals>();
		auto *mulg = TagInstance(matg->MultiplayerGlobals.TagIndex).GetDefinition<MultiplayerGlobals>();

		for (auto &element : mulg->Universal->GameVariantWeapons)
		{
			auto string = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name));
			auto index = (uint16_t)element.Weapon.TagIndex;

			if (index != 0xFFFF)
				weaponIndices.emplace(string, index);
		}
	}

	std::map<std::string, uint16_t> GetIndices()
	{
		return weaponIndices;
	}

	uint16_t GetIndex(std::string &weaponName)
	{
		auto result = weaponIndices.find(weaponName);
		if (result == weaponIndices.end())
			return 0xFFFF;

		return result->second;
	}

	std::string GetName(uint16_t & weaponIndex)
	{
		std::string weaponName;

		bool indexExists = false;

		for (auto &element : weaponIndices)
		{
			auto name = element.first;
			auto index = element.second;

			if (weaponIndex == index) {
				weaponName = name;
				indexExists = true;
			}
		}
		if (indexExists == false)
			return "NotFoundInMULG";

		return weaponName;
	}

	uint16_t GetEquippedWeaponIndex()
	{
		uint16_t equippedIndex = 0xFFFF;

		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			return equippedIndex;
		}

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
					equippedIndex = Pointer(equippedWeaponObjectPtr).Read<uint32_t>();
			}
		}
		return equippedIndex;
	}

	std::string GetEquippedWeaponName()
	{
		auto Index = GetEquippedWeaponIndex();
		return GetName(Index);
	}

	RealVector3D GetOffsetByIndex(bool isDefault, uint16_t &weaponIndex)
	{
		for each (auto element in weaponIndices)
		{
			std::string weaponName = element.first;

			if (weaponIndex == element.second)
			{
				if (isDefault)
				{
					auto result = weaponOffsetsDefault.find(weaponName);
					if (result != weaponOffsetsDefault.end())
						return result->second;
				}
				else
				{
					auto weap = Blam::Tags::TagInstance(weaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
					if(weap)
						return weap->FirstPersonWeaponOffset;
				}
			}
		}

		return { -0xFFF, -0xFFF, -0xFFF };
	}

	RealVector3D GetOffsetByName(bool isDefault, std::string &weaponName)
	{
		auto index = Patches::Weapon::GetIndex(weaponName);
		return GetOffsetByIndex(isDefault, index);
	}

	void SetDefaultOffsets()
	{
		if (weaponOffsetsDefault.empty()) {
			for (auto &element : weaponIndices) {
				std::string selected = element.first;
				auto weapIndex = Patches::Weapon::GetIndex(selected);
				if (weapIndex != 0xFFFF)
				{
					auto *weapon = TagInstance(weapIndex).GetDefinition<Blam::Tags::Items::Weapon>();
					if(weapon)
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
				}
			}
		}
	}

	bool SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset)
	{
		if (!IsNotMainMenu)
			return false;

		auto result = weaponOffsetsDefault.find(weaponName);

		if (IsOffsetModified(weaponName))
		{
			weaponOffsetsModified.try_emplace(weaponName, weaponOffset);
			ApplyOffsetByName(weaponName, weaponOffset);
		}
		else
		{
			weaponOffsetsModified.erase(weaponName);
			ApplyOffsetByName(weaponName, weaponOffset);
		}

		return true;
	}

	void ApplyOffsetByIndex(uint16_t &weaponIndex, RealVector3D &weaponOffset)
	{
		if (IsNotMainMenu)
		{
			auto *weapon = TagInstance(weaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
			if(weapon)
				weapon->FirstPersonWeaponOffset = weaponOffset;
		}
	}

	void ApplyOffsetByName(std::string &weaponName, RealVector3D &weaponOffset)
	{
		if (IsNotMainMenu)
		{

			auto weapIndex = Patches::Weapon::GetIndex(weaponName);
			if (weapIndex != 0xFFFF)
			{
				auto *weapon = TagInstance(weapIndex).GetDefinition<Blam::Tags::Items::Weapon>();
				if(weapon)
					weapon->FirstPersonWeaponOffset = weaponOffset;
			}
		}
	}

	bool IsOffsetModified(const std::string &weaponName)
	{
		auto result = weaponOffsetsDefault.find(weaponName);
		if (result == weaponOffsetsDefault.end())
			return false;
		return true;
	}

	namespace Config
	{
		bool CreateList()
		{
			Modules::ModuleWeapon::Instance().WeaponsJSONList.clear();

			if (!boost::filesystem::exists("mods/weapons/offsets.json"))
			{
				std::ofstream out("mods/weapons/offsets.json", std::ios::out | std::ios::binary);
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
			std::ifstream in("mods/weapons/offsets/" + Name + ".json", std::ios::in | std::ios::binary);
			if (!in || !in.is_open())
				return false;

			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize((unsigned int)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			weaponOffsetsModified.clear();

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
					RealVector3D weapOffset = { std::stof(offsets[0].GetString()), std::stof(offsets[1].GetString()), std::stof(offsets[2].GetString()) };

					SetOffsetModified(weapName, weapOffset);
				}
			}

			return true;
		}

		bool SaveJSON(std::string Name)
		{
			CreateList();

			Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString = Name;

			bool IsCreated = false;
			for each (std::string offsetName in Modules::ModuleWeapon::Instance().WeaponsJSONList)
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
						if (!document.HasMember("offsets"))
							return false;

						std::ofstream out("mods/weapons/offsets.json", std::ios::out | std::ios::binary);
						if (!out.is_open())
							return false;

						rapidjson::StringBuffer jsonBuffer;
						rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
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

			std::ofstream out("mods/weapons/offsets/" + Name + ".json", std::ios::out | std::ios::binary);
			if (!out.is_open())
				return false;

				rapidjson::StringBuffer jsonBuffer;
				rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
				jsonWriter.StartObject();
				jsonWriter.Key("Weapons");

				jsonWriter.StartArray();
				for (auto &weaponParams : (Utils::String::ToLower(Name) == "default" ? weaponOffsetsDefault : weaponOffsetsModified))
				{
					std::string weaponName = weaponParams.first;
					RealVector3D weaponOffset = GetOffsetByName(false, weaponName);

					jsonWriter.StartObject();

					jsonWriter.Key("Name");
					jsonWriter.String(weaponName.c_str());

					jsonWriter.Key("Offset");
					jsonWriter.StartArray();
					jsonWriter.String(std::to_string(weaponOffset.I).c_str());
					jsonWriter.String(std::to_string(weaponOffset.J).c_str());
					jsonWriter.String(std::to_string(weaponOffset.K).c_str());
					jsonWriter.EndArray();

					jsonWriter.EndObject();
				}

				jsonWriter.EndArray();
				jsonWriter.EndObject();

				out << jsonBuffer.GetString();
				if (out.fail())
					return false;

				out.flush();
				out.close();

			return true;
		}
	}
}

namespace
{
	bool UnitIsDualWielding(Blam::DatumIndex unitIndex)
	{
		if (!unitIndex)
			return false;
		auto objectHeaderArrayPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		auto unitDatumPtr = objectHeaderArrayPtr(0x44)[0](unitIndex.Index() * 0x10)(0xC)[0];

		if (!unitDatumPtr)
			return false;

		auto dualWieldWeaponIndex = unitDatumPtr(0x2CB).Read<int8_t>();

		if (dualWieldWeaponIndex < 0 || dualWieldWeaponIndex >= 4)
			return false;

		typedef uint32_t(*UnitGetWeaponPtr)(uint32_t unitObject, short weaponIndex);
		auto UnitGetWeapon = reinterpret_cast<UnitGetWeaponPtr>(0xB454D0);

		return UnitGetWeapon(unitIndex, dualWieldWeaponIndex) != 0xFFFFFFFF;
	}

	bool PlayerIsDualWielding(Blam::DatumIndex playerIndex)
	{
		auto &players = Blam::Players::GetPlayers();
		return UnitIsDualWielding(players[playerIndex].SlaveUnit);
	}

	bool LocalPlayerIsDualWielding()
	{
		auto localPlayer = Blam::Players::GetLocalPlayer(0);
		if (localPlayer == Blam::DatumIndex::Null)
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
		auto *weapon = TagInstance(index).GetDefinition<Weapon>();

		return ((int32_t)weapon->WeaponFlags1 & (int32_t)Weapon::Flags1::CanBeDualWielded) != 0;
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
		enable:
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
		noscope:
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
		const auto weapon_is_being_dual_wielded = (bool(*)(uint32_t weaponObjectIndex))(0xB64050);
		const auto weapon_get_parent_unit = (uint32_t(*)(uint32_t weaponObjectIndex))(0x00B63030);

		using Blam::Tags::Items::Weapon;

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

		/* dual-wield blocks are always empty
		if (isDualWielding)
			return (&weaponDefinition->Barrels[barrelIndex].DualFiringPenaltyFunction)[functionIndex].Count > 0;*/
		return (&weaponDefinition->Barrels[barrelIndex].FiringPenaltyFunction)[functionIndex].Count > 0;
	}

	inline void weapon_set_bloom(uint32_t weaponObjectIndex, int barrelIndex, float value)
	{
		if (value > 1.0f) value = 1.0f;
		if (value < 0) value = 0;

		auto weaponObject = Blam::Objects::Get(weaponObjectIndex);
		if (weaponObject)
			*(float*)((uint8_t*)weaponObject + 0x220 + 0x34 * barrelIndex) = value;
	}

	void __cdecl weapon_apply_firing_penalty_hook(uint32_t weaponObjectIndex, int barrelIndex)
	{
		const auto weapon_apply_firing_penalty = (void(*)(uint32_t weaponObjectIndex, int barrelIndex))(0x00B5C2E0);
		if (weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Firing))
			return weapon_apply_firing_penalty(weaponObjectIndex, barrelIndex);
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

	bool weapon_update_bloom_hook(uint32_t weaponObjectIndex, int barrelIndex)
	{
		const auto weapon_update_bloom = (bool(*)(uint32_t weaponObjectIndex, int barrelIndex))(0x00B5C870);

		auto weaponObject = Blam::Objects::Get(weaponObjectIndex);

		if (!weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Firing) &&
			!weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Moving) &&
			!weapon_should_apply_bloom(weaponObjectIndex, barrelIndex, eProjectilePenalty_Turning))
		{
			weapon_set_bloom(weaponObjectIndex, barrelIndex, 1.0f);
			return true;
		}

		return weapon_update_bloom(weaponObjectIndex, barrelIndex);
	}
}
