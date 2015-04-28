#include "VirtualKeyboard.h"

#include "../ElDorito.h"
#include "../Patch.h"

#include <ShlObj.h>

namespace
{
	class VirtualKeyboard;
	VirtualKeyboard *CreateKeyboard(const char *file, int unk1, int unk2, int unk3, int unk4, int unk5, int maxLength, int unk7, int unk8);
	void __fastcall SetKeyboardDefaultValue(VirtualKeyboard *keyboard, void *unused, wchar_t *newDefaultValue);
	void __fastcall SetKeyboardTitle(VirtualKeyboard *keyboard, void *unused, wchar_t *newTitle);
	void __fastcall SetKeyboardDescription(VirtualKeyboard *keyboard, void *unused, wchar_t *newDescription);
	bool ShowKeyboard(VirtualKeyboard *keyboard, const char *file, int line);

	TODO("move these into a different cpp file, or rename this to GameData.cpp ?");
	bool IsProfileAvailable();
	char __stdcall PackageCreateHook(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
	char __stdcall PackageMountHook(int a1, int a2, int a3, int a4);
	wchar_t* __stdcall GetContentMountPathHook(wchar_t* destPtr, int size, int unk);
	char* __cdecl AllocateGameGlobalMemory2Hook(char *Src, int a2, int a3, char a4, void* a5);
	bool __fastcall SaveFileGetNameHook(void *thisPtr, void* unused, int a2, wchar_t *Src, size_t MaxCount);
}

namespace Patches
{
	namespace VirtualKeyboard
	{
		void ApplyAll()
		{
			// Implement keyboard stubs
			Hook(0xE1840, CreateKeyboard).Apply();
			Hook(0xE19A0, SetKeyboardDefaultValue).Apply();
			Hook(0xE19D0, SetKeyboardTitle).Apply();
			Hook(0xE19B0, SetKeyboardDescription).Apply();
			Hook(0x1A8F40, ShowKeyboard).Apply();

			// Allow saving content without a profile (todo, move this elsewhere once we fully figure out content saving)
			Hook(0x67DCA0, IsProfileAvailable).Apply();

			// Fix storage device checks, so storage device funcs return 0 instead of 0xCACACACA
			Patch(0x1A7A80, { 0x31, 0xc0, 0xC3 }).Apply();
			Patch(0x34D570, { 0x31, 0xc0, 0xC3 }).Apply();

			// Hook this AllocateGameGlobalMemory to use a different one (this one is outdated mb? crashes when object is added to "content items" global without this hook)
			Hook(0x15B010, AllocateGameGlobalMemory2Hook).Apply();

			// Hook GetContentMountPath to actually return a dest folder
			Hook(0x34CC00, GetContentMountPathHook).Apply();

			// Hook (not patch, important otherwise stack gets fucked) content_catalogue_create_new_XContent_item stub to return true
			Hook(0x34CBE0, PackageCreateHook).Apply();

			// Hook (not patch, like above) content package mount stub to return true
			Hook(0x34D010, PackageMountHook).Apply();

			Patch(0x12708E, { 0x8B, 0x4C, 0x24, 0x10 }).Apply(); // patch to pass the map/game variant data from [esp+0x10] to our func

			Hook(0x127092, SaveFileGetNameHook, HookFlags::IsCall).Apply();

			Patch(0x127097, 0x90, 2).Apply();
		}
	}
}

namespace
{
	bool __fastcall SaveFileGetNameHook(void *thisPtr, void* unused, int a2, wchar_t *Src, size_t MaxCount)
	{
		TODO("get the filename (variant.ctf, sandbox.map, feature.film etc) somehow, and maybe put maps in mods\\maps\\, games in mods\\games\\ etc");
		TODO("actually, moving this to Documents\\My Games\\Halo 3\\Saves\\Maps / Games / etc would be better, no need to worry about admin perms");
		wchar_t currentDir[256];
		memset(currentDir, 0, 256 * sizeof(wchar_t));
		GetCurrentDirectoryW(256, currentDir);

		char* blfStart = (char*)thisPtr;
		wchar_t* variantName = (wchar_t*)(blfStart + 0x8); // when saving forge maps we only get the variant name, not the blf data :s
		if (memcmp(blfStart, "_blf", 4) == 0)
			variantName = (wchar_t*)((char*)thisPtr + 0x48);

		swprintf_s(Src, MaxCount, L"%ls\\mods\\saves\\%ls\\", currentDir, variantName);

		SHCreateDirectoryExW(NULL, Src, NULL);

		swprintf_s(Src, MaxCount, L"%ls\\mods\\saves\\%ls\\data.bin", currentDir, variantName);
		return 1;
	}

	char __stdcall PackageCreateHook(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
	{
		return 1;
	}

	char __stdcall PackageMountHook(int a1, int a2, int a3, int a4)
	{
		return 1;
	}

	wchar_t* __stdcall GetContentMountPathHook(wchar_t* destPtr, int size, int unk)
	{
		TODO("move this to temp folder, or find a way to disable it (game uses path returned by this func to create a 0 byte sandbox.map)");
		wchar_t currentDir[256];
		memset(currentDir, 0, 256 * sizeof(wchar_t));
		GetCurrentDirectoryW(256, currentDir);

		swprintf_s(destPtr, size, L"%ls\\mods\\temp\\", currentDir);

		SHCreateDirectoryExW(NULL, destPtr, NULL);
		return destPtr;
	}

	char* __cdecl AllocateGameGlobalMemory2Hook(char *Src, int a2, int a3, char a4, void* a5)
	{
		typedef char*(__cdecl *AllocateGameGlobalMemory)(char *Src, int a2, int a3, char a4, void* a5);
		AllocateGameGlobalMemory allocateGlobal = (AllocateGameGlobalMemory)0x55AFA0;
		char* retData = allocateGlobal(Src, a2, a3, a4, a5);

		*(uint16_t*)(retData + 0x2A) = 0x3;

		if (Src == (char*)0x165B170) // "content items"
		{
			// set some byte thats needed in the contentfile related funcs
			*(uint8_t*)(retData + 0x29) = 1;
		}

		return retData;
	}
	// Based off of H3 Epsilon's code
	class VirtualKeyboard
	{
	public:
		VirtualKeyboard(const char *file, int line, int arg2, int arg3, int arg4, int arg5, int maxLength, int arg7, int arg8)
		{
			unk4 = 0;
			unk6 = 0;
			unk8 = 0;
			unkC = 0;
			unk10 = 0;
			unk14 = 0;
			memset(text, 0, sizeof(text));
			memset(defaultValue, 0, sizeof(defaultValue));
			memset(title, 0, sizeof(title));
			memset(description, 0, sizeof(description));
			this->maxLength = Utils::Clamp(maxLength, 0, 0x100);
		}

		virtual int Stub0() { return 0; }
		virtual int Stub4() { return 0; }
		virtual int Stub8() { return 0; }
		virtual int StubC() { return 0; }
		virtual int Stub10() { return 0; }
		virtual int Stub14() { return 0; }
		virtual int Stub18() { return 0; }
		virtual void Reset() { unk6 = 0; }
		virtual int Stub20() { return 0; }

		void SetDefaultValue(wchar_t *newDefaultValue)
		{
			wcscpy_s(defaultValue, newDefaultValue);
		}

		void SetTitle(wchar_t *newTitle)
		{
			wcscpy_s(title, newTitle);
		}

		void SetDescription(wchar_t *newDescription)
		{
			wcscpy_s(description, newDescription);
		}

		int16_t unk4;
		int16_t unk6;
		int32_t unk8;
		int32_t unkC;
		int32_t unk10;
		int32_t unk14;
		wchar_t text[0x100];
		wchar_t defaultValue[0x100];
		wchar_t title[0x40];
		wchar_t description[0x100];
		int32_t maxLength;
	};

	VirtualKeyboard *CreateKeyboard(const char *file, int line, int unk2, int unk3, int unk4, int unk5, int maxLength, int unk7, int unk8)
	{
		// Does this need to be allocated properly by using the game's malloc?
		return new VirtualKeyboard(file, line, unk2, unk3, unk4, unk5, maxLength, unk7, unk8);
	}

	// Technically thiscall
	void __fastcall SetKeyboardDefaultValue(VirtualKeyboard *keyboard, void *unused, wchar_t *newDefaultValue)
	{
		keyboard->SetDefaultValue(newDefaultValue);
	}

	// Technically thiscall
	void __fastcall SetKeyboardTitle(VirtualKeyboard *keyboard, void *unused, wchar_t *newTitle)
	{
		keyboard->SetTitle(newTitle);
	}

	// Technically thiscall
	void __fastcall SetKeyboardDescription(VirtualKeyboard *keyboard, void *unused, wchar_t *newDescription)
	{
		keyboard->SetDescription(newDescription);
	}

	bool ShowKeyboard(VirtualKeyboard *keyboard, const char *file, int line)
	{
		// This is kinda shitty, but just prompt the user for input by using the console
		// Might be better to actually show a dialog or something
		SetForegroundWindow(GetConsoleWindow());
		std::wcout << std::endl;
		std::wcout << keyboard->title << std::endl;
		std::wcout << keyboard->description << std::endl;
		std::wcout << L"> ";
		std::wstring text;
		std::getline(std::wcin, text);
		wcscpy_s(keyboard->text, text.c_str());

		// Not 100% sure on this next line,
		// it's some sort of state value and 4 seems to mean "done"
		// because it makes the Forge code copy the text somewhere
		keyboard->unk6 = 4;

		return true;
	}

	bool IsProfileAvailable()
	{
		return true;
	}
}