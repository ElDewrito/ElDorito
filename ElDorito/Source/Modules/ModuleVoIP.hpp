#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleVoIP : public Utils::Singleton<ModuleVoIP>, public ModuleBase
	{
	public:
		Command* VarVoipEnabled;
		Command* VarPTTEnabled;
		Command* VarPTT;
		Command* VarMicrophone;
		Command* VarEchoCancellation;
		Command* VarAGC;
		Command* VarNoiseSupress;
		Command* VarPTTSoundEnabled;

		bool voipConnected = false;
		bool voiceDetected = false;

		ModuleVoIP();
	};
}
