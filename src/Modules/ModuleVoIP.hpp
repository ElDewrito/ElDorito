#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleVoIP : public Utils::Singleton<ModuleVoIP>, public ModuleBase
	{
	public:
		Command* VarVoIPPushToTalk;
		Command* VarVoIPVolumeModifier;
		Command* VarVoIPAGC;
		Command* VarVoIPEchoCancellation;
		Command* VarVoIPVADLevel;

		ModuleVoIP();
	};
}