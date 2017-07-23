#include "Assassination.hpp"
#include "../ElDorito.hpp"
#include "../Patch.hpp"

namespace
{
	bool assassinationEnabled = true;
	Patch disableAssassinationsPatch(0x138557, { 0x90, 0xE9 });
}

namespace Patches::Assassination
{
	void Enable(bool enabled)
	{
		assassinationEnabled = enabled;
		if (enabled) {
			disableAssassinationsPatch.Reset();
		}
		else {
			disableAssassinationsPatch.Apply();
		}
	}
}
