#pragma once
#include <string>
#include <vector>

#include "../Modules/ModuleWeapon.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches
{
	namespace Weapon
	{
		void ApplyAll();
		void ApplyAfterTagsLoaded();

		uint16_t GetIndex(std::string &weaponName);
		std::map<std::string, uint16_t> GetIndices();
		Blam::Math::RealVector3D GetOffset(bool isDefault, std::string &weaponName);

		void SetOffsetModified(std::string &weaponName, Blam::Math::RealVector3D &weaponOffset);

		void ApplyOffset(std::string &weaponName, Blam::Math::RealVector3D &weaponOffset);

		bool IsOffsetModified(const std::string &weapon);

		void ConfigLoad(std::string configPath);
		void ConfigSave(std::string configPath);
	}
}