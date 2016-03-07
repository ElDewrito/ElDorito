#pragma once
#include <cstdint>

namespace Patches
{
	namespace WebOverlay
	{
		void ApplyAll();
		void Tick();
		void Resize();
		void Show(bool show);
		uint16_t GetPingId();
	}
}