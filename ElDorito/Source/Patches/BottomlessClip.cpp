#include "BottomlessClip.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamTypes.hpp"

namespace
{
	bool bottomlessClipEnabled = true;
	//nop ammo depletion.
	Patch enableBottomlessClip = Patch(0x75FACD, 0x90, 8);
	Patch enableBottomlessBattery = Patch(0x75BE42, 0x90, 8); //Weapons with battery, like the plasma pistol.
}

namespace Patches::BottomlessClip
{
	void Toggle(bool enabled)
	{
		bottomlessClipEnabled = enabled;
		if (enabled)
		{
			enableBottomlessClip.Apply();
			enableBottomlessBattery.Apply();
		}
		else
		{
			enableBottomlessClip.Reset();
			enableBottomlessBattery.Reset();
		}
	}
}