#pragma once

namespace Patches
{
	namespace Forge
	{
		void ApplyAll();

		// Signal to delete the current item, if any
		void SignalDelete();
	}
}