#pragma once

#include "Modules/ModuleBase.hpp"

namespace Modules
{
	class ElModules : public Utils::Singleton<ElModules>, public ModuleBase
	{
	public:
		ElModules();

	private:
	};
}