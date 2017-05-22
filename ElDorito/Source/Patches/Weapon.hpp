#pragma once
#include <string>
#include <vector>

#include "../Modules/ModuleWeapon.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches
{
	using namespace Blam::Math;

	namespace Weapon
	{
		void Init();
		void ApplyAfterTagsLoaded();

		void ConfigLoad(std::string configPath);
		void ConfigSave(std::string configPath);

		uint16_t GetIndex(std::string &weaponName);

		RealVector3D GetOffset(std::string &weaponName);
		RealVector3D GetOffsetDefault(std::string &weaponName);

		void SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset);
		bool IsOffsetModified(const std::string &weapon);
	}
}