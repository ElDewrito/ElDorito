#include "ElPatches.h"

#include "ElDorito.h"
#include "Patches\Core.h"
#include "Patches\Logging.h"
#include "Patches\Mouse.h"
#include "Patches\Network.h"
#include "Patches\Scoreboard.h"
#include "Patches\Ui.h"
#include "Patches\VirtualKeyboard.h"
#include "Patches\Armor.h"
#include "Patches\RawInput.h"
#include "Patches\ContentItems.h"
#include "Patches\PlayerUid.h"

#include "ElPreferences.h"

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
	}
	
	void ApplyOnFirstTick()
	{
		Ui::ApplyMapNameFixes();
		Network::StartRemoteConsole();
		if (ElPreferences::Instance().getCrosshairCentered())
			Patches::Ui::EnableCenteredCrosshairPatch(true);
	}

	void ApplyAfterTagsLoaded()
	{
		Armor::RefreshUiPlayer();
	}

	void PreferencesUpdated()
	{
		Armor::RefreshUiPlayer();
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