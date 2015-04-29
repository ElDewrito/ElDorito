#pragma once

#include <string>
#include <cstdint>

namespace Optic {

struct Version {
	std::uint16_t major;
	std::uint16_t minor;
	std::uint16_t revision;
};

enum VERSION_RELATION {
	VERSION_IN_FUTURE,
	OKAY,
	TENTATIVE_OKAY,
	OUT_OF_DATE,
	UNKNOWN_VERSION
};

int versionCheck(const std::string& version, const Version& apiVersion);

}