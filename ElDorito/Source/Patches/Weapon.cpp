#include <iomanip>
#include <fstream>
#include <map>

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
}

namespace Patches
{
	namespace Weapon
	{
		using Blam::Math::RealVector3D;
		using Blam::Tags::TagInstance;

		bool IsNotMainMenu;

		//std::string ConfigPath;

		std::string JSONPath;
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

				if (!weaponOffsetsDefault.empty())
				{
					for (auto &element : weaponIndices)
					{
						std::string selected = element.first;
						auto *weapon = TagInstance(Patches::Weapon::GetIndex(selected)).GetDefinition<Blam::Tags::Items::Weapon>();
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
					}
				}

				if (Modules::ModuleWeapon::Instance().VarAutoSaveOnMapLoad->ValueInt == 1 && !weaponOffsetsModified.empty())
				{
					Config::SaveJSON(JSONPath);
				}
				else
				{
					for (auto &weaponParams : weaponOffsetsModified)
					{
						std::string weaponName = weaponParams.first;
						RealVector3D weaponOffset = weaponParams.second;
						ApplyOffsetByName(weaponName, weaponOffset);
					}
				}
			}
		}

		void ApplyAll()
		{
			JSONPath = Modules::ModuleWeapon::Instance().VarWeaponJSON->ValueString;
			Config::LoadJSON(JSONPath);

			Patches::Core::OnMapLoaded(MapLoadedCallback);

			// Force descoping for all weapons
			Pointer::Base(0x73F1E6).Write<uint8_t>(0);

			Hook(0x761550, DualWieldHook).Apply();
			Hook(0x06DFBB, DualWieldSprintInputHook).Apply();
			Hook(0x1D50CB, DualWieldScopeLevelHook).Apply();
			Hook(0x7A21D4, DualWieldEquipmentCountHook, HookFlags::IsCall).Apply();
		}

		void ApplyAfterTagsLoaded()
		{
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

		std::map<std::string, uint16_t> GetIndices()
		{
			return weaponIndices;
		}

		uint16_t GetIndex(std::string &weaponName)
		{
			if (weaponIndices.find(weaponName) == weaponIndices.end())
				return 0xFFFF;

			return weaponIndices.find(weaponName)->second;
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
					{
						auto weap = Blam::Tags::TagInstance(Pointer(equippedWeaponObjectPtr).Read<uint32_t>()).GetDefinition<Blam::Tags::Items::Weapon>();
						equippedIndex = Pointer(equippedWeaponObjectPtr).Read<uint32_t>();
					}
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
						return weaponOffsetsDefault.find(weaponName)->second;
					else
						return weaponOffsetsModified.find(weaponName)->second;
				}
			}

			return { -0xFFF, -0xFFF, -0xFFF };
		}

		RealVector3D GetOffsetByName(bool isDefault, std::string &weaponName)
		{
			if (isDefault)
				return weaponOffsetsDefault.find(weaponName)->second;
			else
				return weaponOffsetsModified.find(weaponName)->second;

			return { -0xFFF, -0xFFF, -0xFFF };
		}

		void SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (weaponOffsetsDefault.find(weaponName)->second == weaponOffset)
			{
				weaponOffsetsModified.erase(weaponName);
				ApplyOffsetByName(weaponName, weaponOffset);
			}
			else
			{
				weaponOffsetsModified.try_emplace(weaponName, weaponOffset);
				ApplyOffsetByName(weaponName, weaponOffset);
			}
		}

		void ApplyOffsetByIndex(uint16_t &weaponIndex, RealVector3D &weaponOffset)
		{
			if (IsNotMainMenu)
			{
				auto *weapon = TagInstance(weaponIndex).GetDefinition<Blam::Tags::Items::Weapon>();
				weapon->FirstPersonWeaponOffset = weaponOffset;
			}
		}

		void ApplyOffsetByName(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (IsNotMainMenu)
			{
				auto *weapon = TagInstance(Patches::Weapon::GetIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
				weapon->FirstPersonWeaponOffset = weaponOffset;
			}
		}

		bool IsOffsetModified(const std::string &weapon)
		{
			return weaponOffsetsModified.find(weapon) != weaponOffsetsModified.end();
		}

		namespace Config
		{
			bool LoadJSON(std::string JSONPath)
			{
				std::ifstream in(JSONPath, std::ios::in | std::ios::binary);
				if (!in || !in.is_open())
					return false;

				std::string contents;
				in.seekg(0, std::ios::end);
				contents.resize((unsigned int)in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&contents[0], contents.size());
				in.close();

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

			bool SaveJSON(std::string JSONPath)
			{
				std::ofstream out(JSONPath, std::ios::binary);
				if (!out.is_open())
					return false;

				rapidjson::StringBuffer jsonBuffer;
				rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
				jsonWriter.StartObject();
					jsonWriter.Key("Weapons");

					jsonWriter.StartArray();
					for (auto &weaponParams : weaponOffsetsModified)
					{
						std::string weaponName = weaponParams.first;
						RealVector3D weaponOffset = weaponParams.second;

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

					//jsonWriter.Key("Centered");
					//jsonWriter.String("true");

					jsonWriter.EndArray();
				jsonWriter.EndObject();

				out << jsonBuffer.GetString();

				//out.write(jsonBuffer.GetString(), sizeof(jsonBuffer));
				//if (out.fail())
				//	return false;

				return true;
			}
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
}