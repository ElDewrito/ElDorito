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
		bool ParseUid(const std::string &str, uint64_t *out);
	}
}