#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleVoIP : public Utils::Singleton<ModuleVoIP>, public ModuleBase
	{
	public:
		Command* VarVoIPPushToTalk;
		Command* VarVoIPVolumeModifier;

		ModuleVoIP();
	};
}