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

		uint16_t GetIndex(std::string &weaponName);
		std::map<std::string, uint16_t> GetIndices();
		RealVector3D GetOffset(std::string mapped, std::string &weaponName);

		void SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset);

		void ApplyOffset(std::string &weaponName, RealVector3D &weaponOffset);

		bool IsOffsetModified(const std::string &weapon);

		void ConfigLoad(std::string configPath);
		void ConfigSave(std::string configPath);
	}
}