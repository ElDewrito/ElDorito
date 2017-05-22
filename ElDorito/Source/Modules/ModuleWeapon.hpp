#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleWeapon : public Utils::Singleton<ModuleWeapon>, public ModuleBase
	{
	public:
		Command* VarWeaponConfig;

		ModuleWeapon();
	};
}