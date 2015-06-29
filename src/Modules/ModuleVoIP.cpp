#include "ModuleVoIP.hpp"
#include <sstream>
#include "../ElDorito.hpp"

namespace Modules
{
	ModuleVoIP::ModuleVoIP() : ModuleBase("VoIP")
	{
		VarVoIPPushToTalk = AddVariableInt("PushToTalk", "voip_ptt", "If push to talk is enabled.", eCommandFlagsArchived, 1);
		VarVoIPPushToTalk->ValueIntMin = 0;
		VarVoIPPushToTalk->ValueIntMax = 1;
	}
}
