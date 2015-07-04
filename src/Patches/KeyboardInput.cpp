#include "KeyboardInput.hpp"

#include "../Patch.hpp"
#include "../ElDorito.hpp"

namespace
{
	bool swallowedKeys[Blam::eKeyCodes_Count];
	std::vector<Patches::KeyboardInput::KeyboardUpdatedCallback> updateCallbacks;

	void KeyboardUpdateHook();
	void KeyTestHook();
}

namespace Patches
{
	namespace KeyboardInput
	{
		void ApplyAll()
		{
			Hook(0x11299D, KeyboardUpdateHook).Apply();
			Hook(0x111B66, KeyTestHook, HookFlags::IsCall).Apply();
			Hook(0x111CE6, KeyTestHook, HookFlags::IsCall).Apply();
		}

		void RegisterUpdateCallback(KeyboardUpdatedCallback callback)
		{
			updateCallbacks.push_back(callback);
		}

		uint8_t GetKeyTicks(Blam::KeyCodes key, Blam::InputType type)
		{
			typedef uint8_t(*EngineGetKeyTicksPtr)(Blam::KeyCodes, Blam::InputType);
			auto EngineGetKeyTicks = reinterpret_cast<EngineGetKeyTicksPtr>(0x511B60);
			return EngineGetKeyTicks(key, type);
		}

		uint16_t GetKeyMs(Blam::KeyCodes key, Blam::InputType type)
		{
			typedef uint8_t(*EngineGetKeyMsPtr)(Blam::KeyCodes, Blam::InputType);
			auto EngineGetKeyMs = reinterpret_cast<EngineGetKeyMsPtr>(0x511CE0);
			return EngineGetKeyMs(key, type);
		}

		void BlockInput(Blam::InputType type, bool block)
		{
			typedef uint8_t(*EngineBlockInputPtr)(Blam::InputType, bool);
			auto EngineBlockInput = reinterpret_cast<EngineBlockInputPtr>(0x511CE0);
			EngineBlockInput(type, block);
		}

		void Swallow(Blam::KeyCodes key)
		{
			if (key >= 0 && key < Blam::eKeyCodes_Count)
				swallowedKeys[key] = true;
		}
	}
}

namespace
{
	void KeyboardUpdateHandler()
	{
		memset(swallowedKeys, 0, sizeof(swallowedKeys));
		for (const auto &callback : updateCallbacks)
			callback();
	}

	// Hook for the keyboard update function to call our update handler after
	// the function returns
	__declspec(naked) void KeyboardUpdateHook()
	{
		__asm
		{
			// Epilogue of hooked function
			mov esp, ebp
			pop ebp

			jmp KeyboardUpdateHandler
		}
	}

	// Hook for the engine's "get key ticks" and "get key ms" functions to
	// block the key if it's been swallowed. If the zero flag is not set when
	// this returns, the key will be blocked.
	__declspec(naked) void KeyTestHook()
	{
		__asm
		{
			// Replaced code (checks if the input type is blocked)
			cmp byte ptr ds:0x238DBEB[eax], 0
			jnz done

			// Also block the key if it's been swallowed
			movsx eax, word ptr [ebp + 8] // Get key code
			cmp byte ptr swallowedKeys[eax], 0

		done:
			// The replaced instruction is 7 bytes long, so get the return
			// address and add 2 without affecting the flags
			pop eax
			lea eax, [eax + 2]
			jmp eax
		}
	}
}