#pragma once
#include <cstdint>
#include <string>

namespace Patches
{
	namespace WebOverlay
	{
		void ApplyAll();
		void Tick();
		void Resize();
		void Show(bool show);
		void ShowScreen(const std::string &id, const std::string &data);
		void CaptureInput(bool capture);
		uint16_t GetPingId();
	}
}
