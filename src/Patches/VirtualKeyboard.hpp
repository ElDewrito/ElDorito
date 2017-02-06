#pragma once

#include <functional>

namespace Blam
{
	namespace Input
	{
		class VirtualKeyboard;
	}
}

namespace Patches
{
	namespace VirtualKeyboard
	{
		void ApplyAll();

		typedef std::function<void(Blam::Input::VirtualKeyboard* keyboard)> KeyboardHandlerCallback;
		void SetKeyboardHandler(KeyboardHandlerCallback callback);
	}
}
