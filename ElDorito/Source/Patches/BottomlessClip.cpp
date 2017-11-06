#include "BottomlessClip.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"
#include "../Blam/BlamTypes.hpp"

namespace
{
	void AmmoCountInputHook();

	bool bottomlessClipEnabled = true;

	Patch enableBottomlessClip = Patch(0x75FACD, 0x90, 8);
	Patch enableBottomlessBattery = Patch(0x75BE42, 0x90, 8); //Weapons with battery, like the plasma pistol.

	Hook totalAmmoInfiniteSymbol = Hook(0x68B6DF, AmmoCountInputHook, HookFlags::IsCall);
	Hook clipAmmoInfiniteSymbol = Hook(0x68B6C2, AmmoCountInputHook, HookFlags::IsCall);
	Hook batteryInfiniteSymbol = Hook(0x68B72D, AmmoCountInputHook, HookFlags::IsCall);
}

namespace Patches::BottomlessClip
{
	void Toggle(bool enabled)
	{
		bottomlessClipEnabled = enabled;
		if (enabled)
		{
			//Enable bottomless clip.
			enableBottomlessClip.Apply();
			enableBottomlessBattery.Apply();
			//Enabled infinity symbols.
			totalAmmoInfiniteSymbol.Apply();
			clipAmmoInfiniteSymbol.Apply();
			batteryInfiniteSymbol.Apply();
		}
		else
		{
			//Disable bottomless clip.
			enableBottomlessClip.Reset();
			enableBottomlessBattery.Reset();
			//Disable infinity symbols.
			totalAmmoInfiniteSymbol.Reset();
			clipAmmoInfiniteSymbol.Reset();
			batteryInfiniteSymbol.Reset();
		}
	}
}

namespace 
{
	__declspec(naked) void AmmoCountInputHook()
	{
		__asm
		{
			mov ecx, 0x7F7FFFFF
			movd xmm0, ecx
			ret
		}
	}
}