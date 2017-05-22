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

		void Save_Config(std::string configPath);
		void Load_Config(std::string configPath);

		uint16_t Get_Index(std::string &weaponName);

		RealVector3D Get_Offset(std::string &weaponName);
		RealVector3D Get_OffsetDefault(std::string &weaponName);

		void Update_OffsetsModified(std::string &weaponName, RealVector3D &weaponOffset);
		bool Is_OffsetModified(const std::string &weapon);
	}
}