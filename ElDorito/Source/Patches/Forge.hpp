#pragma once

namespace Patches
{
	namespace Forge
	{
		void ApplyAll();
		void Tick();

		// Signal to delete the current item, if any
		void SignalDelete();

		void DeleteAll();
		void CanvasMap();
	}
}