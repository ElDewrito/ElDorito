#include "FpsCounter.hpp"
#include "../ElDorito.hpp"
#include "../Patch.hpp"

namespace
{
	float fps_value = Pointer(0x22B47F8).Read<float>(); // for future use

	Patch FpsTextMinimalPatch(0x12503E0, { 0x25, 0x32, 0x2E, 0x32, 0x66, 0x00 }); // show fps as "%2.2f" and not "t[%2.2f] r[%2.2f]|nmin%2.2f max%2.2f|ngfx %d dip %d max %d|n%s|n"
}

namespace Patches::FpsCounter
{
	void Enable(bool enabled)
	{
		Pointer(0x22B47FC).WriteFast<bool>(enabled);
	}

	void ShortForm(bool enabled)
	{
		if (enabled) {
			FpsTextMinimalPatch.Apply();
		}
		else {
			FpsTextMinimalPatch.Reset();
		}
	}
}
