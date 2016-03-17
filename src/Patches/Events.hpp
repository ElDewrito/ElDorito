#pragma once
#include "../Blam/BlamData.hpp"
#include <functional>

namespace Blam
{
	namespace Events
	{
		struct Event;
		struct EventDefinition;
	}
}

namespace Patches
{
	namespace Events
	{
		void ApplyAll();

		// Callback for a Blam event handler.
		typedef std::function<void(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)> EventCallback;

		// Registers a callback to be run when a Blam event fires.
		void OnEvent(EventCallback callback);
	}
}