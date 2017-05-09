#include "Events.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Patch.hpp"
#include "../Web/Ui/WebScoreboard.hpp"
#include "../Web/Ui/ScreenLayer.hpp"

#include <vector>

using namespace Blam::Events;
using namespace Patches::Events;

namespace
{
	void RunEventHook(Blam::DatumIndex player, const Event *event, const EventDefinition *definition);
	void __cdecl UpdateScoreboardEventHookHost(int a1, unsigned int a2, int a3, int a4);
	char __cdecl UpdateScoreboardEventHook(int a1, int a2, int a3);
	std::vector<EventCallback> OnEventCallbacks;

}

namespace Patches
{
	namespace Events
	{
		void ApplyAll()
		{
			Hook(0x16669C, RunEventHook, HookFlags::IsCall).Apply();
			Hook(0x165CE2, RunEventHook, HookFlags::IsCall).Apply();
			Hook(0x165D0D, RunEventHook, HookFlags::IsCall).Apply();
			Hook(0x2E5A24, UpdateScoreboardEventHookHost, HookFlags::IsCall).Apply();
			Hook(0xC654D, UpdateScoreboardEventHook, HookFlags::IsCall).Apply();

		}

		void OnEvent(EventCallback callback)
		{
			OnEventCallbacks.push_back(callback);
		}
	}
}

namespace
{
	void __cdecl UpdateScoreboardEventHookHost(int a1, unsigned int a2,  int a3, int a4)
	{
		Web::Ui::ScreenLayer::Notify("scoreboard", Web::Ui::WebScoreboard::getScoreboard(), true);

		typedef char(*UpdateScoreboard)(int a1, unsigned int a2, int a3, int a4);
		UpdateScoreboard Update = reinterpret_cast<UpdateScoreboard>(0x5704A0);
		Update(a1, a2, a3, a4);

	}
	char __cdecl UpdateScoreboardEventHook(int a1, int a2, int a3)
	{
		Web::Ui::ScreenLayer::Notify("scoreboard", Web::Ui::WebScoreboard::getScoreboard(), true);

		typedef char(*UpdateScoreboard)(int a1, int a2, int a3);
		UpdateScoreboard Update = reinterpret_cast<UpdateScoreboard>(0x566070);
		return Update(a1, a2, a3);

	}

	void RunEventHook(Blam::DatumIndex player, const Event *event, const EventDefinition *definition)
	{
		// Dispatch the event to handlers
		for (auto &&callback : OnEventCallbacks)
			callback(player, event, definition);

		// Call the original function
		typedef void(*RunEventPtr)(Blam::DatumIndex player, const Event *event, const EventDefinition *definition);
		auto RunEvent = reinterpret_cast<RunEventPtr>(0x5666B0);
		RunEvent(player, event, definition);
	}
}