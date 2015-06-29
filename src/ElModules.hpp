#pragma once

#include "Modules/ModuleBase.hpp"
#include "Modules/ModuleGame.hpp"
#include "Modules/ModulePlayer.hpp"
#include "Modules/ModuleServer.hpp"
#include "Modules/ModuleCamera.hpp"
#include "Modules/ModuleInput.hpp"
#include "Modules/ModuleIRC.hpp"
#include "Modules/ModuleVoIP.hpp"

namespace Modules
{
	class ElModules : public Utils::Singleton<ElModules>, public ModuleBase
	{
	public:
		ElModules();

	private:
	};
}