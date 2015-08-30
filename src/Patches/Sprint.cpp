#include "Sprint.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"

namespace
{
	bool sprintEnabled = true;
	Patch disableSprintPatch(0x13E26B, { 0x90, 0xE9 });
	Patch unlimitedSprintPatch(0x13E5D1, { 0x00 });
}

namespace Patches
{
	namespace Sprint
	{
		void Enable(bool enabled)
		{
			sprintEnabled = enabled;
			if (enabled)
				disableSprintPatch.Reset();
			else
				disableSprintPatch.Apply();
		}

		void SetUnlimited(bool unlimited)
		{
			if (unlimited)
				unlimitedSprintPatch.Apply();
			else
				unlimitedSprintPatch.Reset();
		}

		void Tick()
		{
			// Show/hide the sprint meter
			auto persistentUserDataChud = ElDorito::GetMainTls(GameGlobals::PersistentUserDataChud::TLSOffset)[0];
			if (persistentUserDataChud)
				persistentUserDataChud(GameGlobals::PersistentUserDataChud::SprintMeterOffset).Write<bool>(sprintEnabled);
		}
	}
}