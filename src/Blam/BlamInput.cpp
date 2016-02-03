#include "BlamInput.hpp"

namespace Blam
{
	namespace Input
	{
		uint8_t GetKeyTicks(KeyCodes key, InputType type)
		{
			typedef uint8_t(*EngineGetKeyTicksPtr)(KeyCodes, InputType);
			auto EngineGetKeyTicks = reinterpret_cast<EngineGetKeyTicksPtr>(0x511B60);
			return EngineGetKeyTicks(key, type);
		}

		uint16_t GetKeyMs(KeyCodes key, InputType type)
		{
			typedef uint8_t(*EngineGetKeyMsPtr)(KeyCodes, InputType);
			auto EngineGetKeyMs = reinterpret_cast<EngineGetKeyMsPtr>(0x511CE0);
			return EngineGetKeyMs(key, type);
		}

		bool ReadKeyEvent(KeyEvent* result, InputType type)
		{
			typedef bool(*EngineReadKeyEventPtr)(KeyEvent*, InputType);
			auto EngineReadKeyEvent = reinterpret_cast<EngineReadKeyEventPtr>(0x5118C0);
			return EngineReadKeyEvent(result, type);
		}

		void BlockInput(InputType type, bool block)
		{
			typedef uint8_t(*EngineBlockInputPtr)(InputType, bool);
			auto EngineBlockInput = reinterpret_cast<EngineBlockInputPtr>(0x512530);
			EngineBlockInput(type, block);
		}

		BindingsTable* GetBindings(int localPlayerIndex)
		{
			if (localPlayerIndex < 0 || localPlayerIndex >= 4)
				return nullptr;

			// NOTE: There is reason to believe that the bindings table array
			// is actually part of a larger structure which encompasses other
			// input data too, because there are memsets to this address which
			// zero out 0x14D4 bytes
			return &reinterpret_cast<BindingsTable*>(0x244C9D0)[localPlayerIndex];
		}
	}
}