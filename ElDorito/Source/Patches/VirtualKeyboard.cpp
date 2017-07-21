#include "VirtualKeyboard.hpp"

#include "../Blam/BlamInput.hpp"
#include "../Patch.hpp"

namespace
{
	Blam::Input::VirtualKeyboard *CreateVirtualKeyboard(const char *file, int unk1, int unk2, int unk3, int unk4, int unk5, int maxLength, int unk7, int unk8);
	void __fastcall SetVirtualKeyboardDefaultValue(Blam::Input::VirtualKeyboard *keyboard, void *unused, wchar_t *newDefaultValue);
	void __fastcall SetVirtualKeyboardTitle(Blam::Input::VirtualKeyboard *keyboard, void *unused, wchar_t *newTitle);
	void __fastcall SetVirtualKeyboardDescription(Blam::Input::VirtualKeyboard *keyboard, void *unused, wchar_t *newDescription);
	bool ShowVirtualKeyboard(Blam::Input::VirtualKeyboard *keyboard, const char *file, int line);

	Patches::VirtualKeyboard::KeyboardHandlerCallback KeyboardHandler = [](Blam::Input::VirtualKeyboard* keyboard) { keyboard->Reset(); };
}

namespace Patches::VirtualKeyboard
{
	void ApplyAll()
	{
		// Implement virtual keyboard stubs
		Hook(0xE1840, CreateVirtualKeyboard).Apply();
		Hook(0xE19A0, SetVirtualKeyboardDefaultValue).Apply();
		Hook(0xE19D0, SetVirtualKeyboardTitle).Apply();
		Hook(0xE19B0, SetVirtualKeyboardDescription).Apply();
		Hook(0x1A8F40, ShowVirtualKeyboard).Apply();
	}

	void SetKeyboardHandler(KeyboardHandlerCallback callback)
	{
		KeyboardHandler = callback;
	}
}

namespace
{
	Blam::Input::VirtualKeyboard *CreateVirtualKeyboard(const char *file, int line, int unk2, int unk3, int unk4, int unk5, int maxLength, int unk7, int unk8)
	{
		return new Blam::Input::VirtualKeyboard(file, line, unk2, unk3, unk4, unk5, maxLength, unk7, unk8);
	}

	// Technically thiscall
	void __fastcall SetVirtualKeyboardDefaultValue(Blam::Input::VirtualKeyboard *keyboard, void *unused, wchar_t *newDefaultValue)
	{
		keyboard->SetDefaultValue(newDefaultValue);
	}

	// Technically thiscall
	void __fastcall SetVirtualKeyboardTitle(Blam::Input::VirtualKeyboard *keyboard, void *unused, wchar_t *newTitle)
	{
		keyboard->SetTitle(newTitle);
	}

	// Technically thiscall
	void __fastcall SetVirtualKeyboardDescription(Blam::Input::VirtualKeyboard *keyboard, void *unused, wchar_t *newDescription)
	{
		keyboard->SetDescription(newDescription);
	}

	bool ShowVirtualKeyboard(Blam::Input::VirtualKeyboard *keyboard, const char *file, int line)
	{
		KeyboardHandler(keyboard);
		return true;
	}
}