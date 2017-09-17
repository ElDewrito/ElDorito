#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleTime : public Utils::Singleton<ModuleTime>, public ModuleBase
	{
	public:
		Command* VarSpeed;
		Command* VarSpeedClient;

		// TODO: experimental fps with buggy havok physics - give hkWorld initialization a second chance in hopes of fixing it
		//Command* VarFps;

		ModuleTime();
	};
}