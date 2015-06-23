#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleIRC : public Utils::Singleton<ModuleIRC>, public ModuleBase
	{
	public:
		Command* VarIRCServer;
		Command* VarIRCServerPort;

		ModuleIRC();
	};
}