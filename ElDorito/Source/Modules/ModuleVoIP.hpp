#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleVoIP : public Utils::Singleton<ModuleVoIP>, public ModuleBase
	{
	public:
		Command* VarPTTEnabled;
		Command* VarPTT;

		ModuleVoIP();
	};
}
