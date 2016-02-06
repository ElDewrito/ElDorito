#pragma once

#include <memory>
#include <functional>
#include <vector>
#include "../Blam/BlamInput.hpp"

namespace Patches
{
	namespace Input
	{
		void ApplyAll();

		// Base class for objects which override game input.
		// When an input context is active, game and special input will be
		// blocked.
		class InputContext
		{
		public:
			virtual ~InputContext() { }

			// Called when the input context is made active.
			virtual void InputActivated() = 0;

			// Called when the input context is deactivated.
			virtual void InputDeactivated() = 0;

			// Called on the active context each time the game processes input,
			// overriding the default input processing routine. If this returns
			// false, the input context will be deactivated.
			//
			// The difference between this and UiInputTick() is that this
			// cannot call Blam::Input::ReadKeyEvent() because Windows messages
			// won't have been pumped yet. However, this happens earlier on in
			// the engine update cycle.
			virtual bool GameInputTick() = 0;

			// Called on the active context each time Windows messages are
			// pumped. If this returns false, the input context will be
			// deactivated.
			virtual bool UiInputTick() = 0;
		};

		// Pushes a new context onto the input context stack, deactivating the
		// old context and then activating the new one. Once the new context
		// becomes deactivated, the previous context will be re-activated.
		void PushContext(std::shared_ptr<InputContext> context);

		typedef std::function<void()> DefaultInputHandler;
		
		// Registers a function to be called when the default input handler is
		// ticked.
		void RegisterDefaultInputHandler(DefaultInputHandler func);

		// Sets what controls can be configured in the keyboard settings menu.
		void SetKeyboardSettingsMenu(
			const std::vector<Blam::Input::ConfigurableAction> &infantrySettings,
			const std::vector<Blam::Input::ConfigurableAction> &vehicleSettings);
	}
}
