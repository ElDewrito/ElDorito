#pragma once

#include <functional>

namespace Blam::Input
{
	class VirtualKeyboard;
}

namespace Patches::VirtualKeyboard
{
	void ApplyAll();

	typedef std::function<void(Blam::Input::VirtualKeyboard* keyboard)> KeyboardHandlerCallback;
	void SetKeyboardHandler(KeyboardHandlerCallback callback);
}

