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
	void RunEventHook(Blam::DatumHandle player, const Event *event, const EventDefinition *definition);
	std::vector<EventCallback> OnEventCallbacks;

}

namespace Patches::Events
{
	void ApplyAll()
	{
		Hook(0x16669C, RunEventHook, HookFlags::IsCall).Apply();
		Hook(0x165CE2, RunEventHook, HookFlags::IsCall).Apply();
		Hook(0x165D0D, RunEventHook, HookFlags::IsCall).Apply();
	}

	void OnEvent(EventCallback callback)
	{
		OnEventCallbacks.push_back(callback);
	}
}

namespace
{
	void RunEventHook(Blam::DatumHandle player, const Event *event, const EventDefinition *definition)
	{
		// Dispatch the event to handlers
		for (auto &&callback : OnEventCallbacks)
			callback(player, event, definition);

		// Call the original function
		typedef void(*RunEventPtr)(Blam::DatumHandle player, const Event *event, const EventDefinition *definition);
		auto RunEvent = reinterpret_cast<RunEventPtr>(0x5666B0);
		RunEvent(player, event, definition);
	}
}