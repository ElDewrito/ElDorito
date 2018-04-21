#include "ElPatches.hpp"

#include "ElDorito.hpp"
#include "Blam\Tags\TagInstance.hpp"
#include "Patches\Core.hpp"
#include "Patches\Audio.hpp"
#include "Patches\Mouse.hpp"
#include "Patches\Network.hpp"
#include "Patches\Scoreboard.hpp"
#include "Patches\Ui.hpp"
#include "Patches\VirtualKeyboard.hpp"
#include "Patches\RawInput.hpp"
#include "Patches\ContentItems.hpp"
#include "Patches\PlayerUid.hpp"
#include "Patches\Input.hpp"
#include "Patches\Forge.hpp"
#include "Patches\CustomPackets.hpp"
#include "Patches\Logging.hpp"
#include "Patches\Sprint.hpp"
#include "Patches\Events.hpp"
#include "Patches\LoadingScreen.hpp"
#include "Patches\Memory.hpp"
#include "Patches\Equipment.hpp"
#include "Patches\PlayerRepresentation.hpp"
#include "Patches\Hf2pExperimental.hpp"
#include "Patches\Weapon.hpp"
#include "Patches\BottomlessClip.hpp"
#include "Patches\Spectate.hpp"
#include "Patches\Tweaks.hpp"
#include "Patches\DirectXHook.hpp"
#include "Patches\Medals.hpp"
#include "Patches\Simulation.hpp"
#include "Patches\Camera.hpp"
#include "Patches\Maps.hpp"
#include "Patches\GameEngineSettings.hpp"
#include "Patches\DamageSystem.hpp"
#include "Patches\PlayerScale.hpp"
#include "Game\Armor.hpp"

#include <fstream>

namespace
{
	void UnprotectMemory();
}

namespace Patches
{
	void ApplyRequired()
	{
		UnprotectMemory();
		Hf2pExperimental::ApplyAll();
		Core::ApplyAll();
		Audio::ApplyAll();
		Memory::ApplyAll();
		Mouse::ApplyAll();
		Network::ApplyAll();
		Scoreboard::ApplyAll();
		Ui::ApplyAll();
		VirtualKeyboard::ApplyAll();
		RawInput::ApplyAll();
		ContentItems::ApplyAll();
		PlayerUid::ApplyAll();
		PlayerScale::ApplyAll();
		Input::ApplyAll();
		Forge::ApplyAll();
		CustomPackets::ApplyAll();
		Logging::ApplyAll();
		Events::ApplyAll();
		LoadingScreen::ApplyAll();
		Equipment::ApplyAll();
		Weapon::ApplyAll();
		Patches::BottomlessClip::ApplyAll();
		Spectate::ApplyAll();
		Medals::ApplyAll();
		Simulation::ApplyAll();
		Camera::ApplyAll();
		Maps::ApplyAll();
		GameEngineSettings::ApplyAll();
		DamageSystem::ApplyAll();

		Network::PlayerPropertiesExtender::Instance().Add(std::make_shared<Game::Armor::ArmorExtension>());

		PlayerRepresentation::ApplyAll();

		//Since these patches are happening before ED gets initalized, we dont know if we are in dedi mode or not.
		bool isdedicated = false;
		int numArgs = 0;
		LPWSTR* szArgList = CommandLineToArgvW(GetCommandLineW(), &numArgs);
		for (int i = 1; i < numArgs; i++)
		{
			std::wstring arg = std::wstring(szArgList[i]);
			if (arg.compare(L"-headless") == 0 || arg.compare(L"-dedicated") == 0)
				isdedicated = true;
		}

		if (!isdedicated)
			DirectXHook::ApplyAll();
	}

	void ApplyOnFirstTick()
	{
	}

	void ApplyAfterTagsLoaded()
	{
		Blam::Tags::TagInstance::LoadTagNames();
		Game::Armor::LoadArmorPermutations();
		Game::Armor::RefreshUiPlayer();
		Ui::ApplyAfterTagsLoaded(); //No UI calls interacting with tags before this!
		Ui::ApplyUIResolution();
		Weapon::ApplyAfterTagsLoaded();
		Tweaks::ApplyAfterTagsLoaded();
	}

	void Tick()
	{
		Sprint::Tick();
		Forge::Tick();
		PlayerScale::Tick();
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