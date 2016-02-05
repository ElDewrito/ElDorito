#include "ElPatches.hpp"

#include "ElDorito.hpp"
#include "Patches\Core.hpp"
#include "Patches\Mouse.hpp"
#include "Patches\Network.hpp"
#include "Patches\Scoreboard.hpp"
#include "Patches\Ui.hpp"
#include "Patches\VirtualKeyboard.hpp"
#include "Patches\Armor.hpp"
#include "Patches\RawInput.hpp"
#include "Patches\ContentItems.hpp"
#include "Patches\PlayerUid.hpp"
#include "Patches\Input.hpp"
#include "Patches\Forge.hpp"
#include "Patches\CustomPackets.hpp"
#include "Patches\Logging.hpp"
#include "Patches\Sprint.hpp"

#include "Modules\ModuleCamera.hpp"

namespace
{
	void UnprotectMemory();
}

namespace Patches
{
	void ApplyRequired()
	{
		UnprotectMemory();
		Core::ApplyAll();
		Mouse::ApplyAll();
		Network::ApplyAll();
		Scoreboard::ApplyAll();
		Ui::ApplyAll();
		VirtualKeyboard::ApplyAll();
		Armor::ApplyAll();
		RawInput::ApplyAll();
		ContentItems::ApplyAll();
		PlayerUid::ApplyAll();
		Input::ApplyAll();
		Forge::ApplyAll();
		CustomPackets::ApplyAll();
		Logging::ApplyAll();
	}
	
	void ApplyOnFirstTick()
	{
		Ui::ApplyMapNameFixes();
		Network::StartRemoteConsole();
	}

	void ApplyAfterTagsLoaded()
	{
		Armor::RefreshUiPlayer();
	}

	void Tick()
	{
		Ui::Tick();
		Sprint::Tick();

		static bool appliedFirstTickPatches = false;
		if (appliedFirstTickPatches)
			return;

		ApplyOnFirstTick();
		appliedFirstTickPatches = true;
	}
}

namespace
{
	void UnprotectMemory()
	{
		// Enable write to all executable memory
		size_t Offset, Total;
		Offset = Total = 0;
		MEMORY_BASIC_INFORMATION MemInfo;

		//printf("\nUnprotecting memory...");
		while (VirtualQuery((uint8_t*)GetBasePointer() + Offset, &MemInfo, sizeof(MEMORY_BASIC_INFORMATION)))
		{
			Offset += MemInfo.RegionSize;
			if (MemInfo.Protect == PAGE_EXECUTE_READ)
			{
				//printf("%0X\n", (size_t)((uint8_t*)GetBasePointer() + Offset));
				Total += MemInfo.RegionSize;
				VirtualProtect(MemInfo.BaseAddress, MemInfo.RegionSize, PAGE_EXECUTE_READWRITE, &MemInfo.Protect);
			}
		}
		//printf("\nDone! Unprotected %u bytes of memory\n", Total);
	}
}