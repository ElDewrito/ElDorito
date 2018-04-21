#pragma once
#include <string>
#include <vector>

#include "../Modules/ModuleWeapon.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches::Weapon
{
	void ApplyAll();
	void ApplyAfterTagsLoaded();
	
	struct WeaponInfo
	{
		struct Offsets
		{
			Blam::Math::RealVector3D Default;
			Blam::Math::RealVector3D Modified;
		};

		std::string Name;
		std::string TagName;
		uint16_t Index;
		Offsets Offset;
	};

	std::map<std::string, uint16_t> GetIndices();

	WeaponInfo GetWeaponByName(std::string &weaponName);
	WeaponInfo GetWeaponByIndex(uint16_t &weaponIndex);
	WeaponInfo GetWeaponByTagName(std::string &weaponTagName);

	WeaponInfo GetWeapon(WeaponInfo &weapon);
	uint16_t GetIndex(WeaponInfo &weapon);
	std::string GetName(WeaponInfo &weapon);
	std::string GetTagName(WeaponInfo &weapon);
	WeaponInfo::Offsets GetOffsets(WeaponInfo &weapon);

	WeaponInfo GetEquippedWeapon();
	uint16_t GetEquippedWeaponIndex();
	std::string GetEquippedWeaponName();
	std::string GetEquippedWeaponTagName();
	WeaponInfo::Offsets GetEquippedWeaponOffsets();

	Blam::Math::RealVector3D GetOffsetByName(bool isDefault, std::string &weaponName);
	Blam::Math::RealVector3D GetOffsetByIndex(bool isDefault, uint16_t &weaponIndex);
	Blam::Math::RealVector3D GetOffsetByTagName(bool isDefault, std::string &weaponName);

	void SetDefaultOffsets();
	bool SetOffsetDefaultAll();
	bool SetOffsetDefault(std::string &weaponName);
	bool SetOffsetModified(std::string &weaponName, Blam::Math::RealVector3D &weaponOffset);

	namespace Config
	{
		bool CreateList();
		bool LoadJSON(std::string Name);
		bool SaveJSON(std::string Name);
	}
}