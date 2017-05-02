#include "Pancam.hpp"
#include "../ElDorito.hpp"
#include "../Patch.hpp"

namespace
{
	bool pancamEnabled = false;
	Patch enablePancamPatch(0x192064, { 0x00 });
}

namespace Patches
{
	namespace Pancam
	{
		void Enable(bool enabled)
		{
			pancamEnabled = enabled;
			if (enabled) {
				enablePancamPatch.Apply();
			}
			else {
				enablePancamPatch.Reset();
			}
		}
	}
}