#pragma once

#include "ModuleBase.hpp"


namespace Modules
{
	class ModuleDiscord : public Utils::Singleton<ModuleDiscord>, public ModuleBase
	{
	public:
		ModuleDiscord();
		Command* TestPresence;
		static void DiscordInit();
	};
}