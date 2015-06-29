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

		VarVoIPVolumeModifier = AddVariableInt("VolumeModifier", "voip_volume_modifier", "Modify the volume of other speakers (db)."
			"0 = no modification, negative values make the signal quieter,"
			"greater than zero boost the signal louder. High positive values = really bad audio quality.", eCommandFlagsArchived, 6);
		VarVoIPVolumeModifier->ValueIntMin = -15;
		VarVoIPVolumeModifier->ValueIntMax = 30;


	}
}
