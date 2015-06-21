#pragma once

#include <cstdint>
#include <string>

namespace Patches
{
	namespace PlayerUid
	{
		void ApplyAll();
		uint64_t Get();
		std::string GetFormattedPrivKey();
	}
}