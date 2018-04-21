#pragma once

#include <memory>
#include <functional>
#include <vector>
#include "../Blam/BlamInput.hpp"

namespace Patches::Input
{
	void ApplyAll();
	bool QueueGameAction(int);
	// Base class for objects which override game input.
	// While a context is active, all game input is blocked.
	class InputContext
	{
	public:
		virtual ~InputContext() { }

		// Called when the input context is made active.
		virtual void Activated() = 0;

		// Called when the input context is deactivated.
		virtual void Deactivated() = 0;

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

		//Called before GameInputTick() and UiInputTick()
		virtual void InputBlock() { memset(reinterpret_cast<bool*>(0x238DBEB), 1, Blam::Input::InputType::eInputType_Count); }
		//Called after GameInputTick() and UiInputTick()
		virtual void InputUnblock() { memset(reinterpret_cast<bool*>(0x238DBEB), 0, Blam::Input::InputType::eInputType_Count); };

		//If the Input context should allow the registered handlers to still fire
		bool allowHandlers = false;
	};

	// Pushes a new context onto the input context stack, deactivating the
	// old context and then activating the new one. Once the new context
	// becomes deactivated, the previous context will be re-activated.
	void PushContext(std::shared_ptr<InputContext> context);

	using DefaultInputHandler = void(*)();
	using MenuUIInputHandler = void(*)();
		
	// Registers a function to be called when the default input handler is
	// ticked.
	void RegisterDefaultInputHandler(DefaultInputHandler func);
	void RegisterMenuUIInputHandler(MenuUIInputHandler func);

	// Sets what controls can be configured in the keyboard settings menu.
	void SetKeyboardSettingsMenu(
		const std::vector<Blam::Input::ConfigurableAction> &infantrySettings,
		const std::vector<Blam::Input::ConfigurableAction> &vehicleSettings);

	void TestControllerVibration(float duration);
	void InvalidateBindings();
}
