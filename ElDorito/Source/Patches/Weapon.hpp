#pragma once
#include <string>
#include <vector>

#include "../Modules/ModuleWeapon.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches::Weapon
{
	void ApplyAll();
	void ApplyAfterTagsLoaded();

	std::map<std::string, uint16_t> GetIndices();
	uint16_t GetIndex(std::string &weaponName);
	std::string GetName(uint16_t &weaponIndex);
	uint16_t GetEquippedWeaponIndex();
	std::string GetEquippedWeaponName();
	Blam::Math::RealVector3D GetOffsetByIndex(bool isDefault, uint16_t &weaponIndex);
	Blam::Math::RealVector3D GetOffsetByName(bool isDefault, std::string &weaponName);

	void SetDefaultOffsets();
	bool SetOffsetModified(std::string &weaponName, Blam::Math::RealVector3D &weaponOffset);

	void ApplyOffsetByIndex(uint16_t &weaponIndex, Blam::Math::RealVector3D &weaponOffset);
	void ApplyOffsetByName(std::string &weaponName, Blam::Math::RealVector3D &weaponOffset);

	bool IsOffsetModified(const std::string &weaponName);

	namespace Config
	{
		bool CreateList();
		bool LoadJSON(std::string Name);
		bool SaveJSON(std::string Name);
	}
}