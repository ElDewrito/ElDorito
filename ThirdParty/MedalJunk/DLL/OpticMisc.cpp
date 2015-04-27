#include "OpticMisc.h"
#include <boost/lexical_cast.hpp>
#include <vector>
#include <cstdint>

namespace Optic {

std::vector<std::uint16_t> split(const std::string &s, char delim) {
	std::vector<std::uint16_t> tokens;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
		tokens.push_back(boost::lexical_cast<std::uint16_t>(item));
    }

    return tokens;
}

int versionCheck(const std::string& version, const Version& apiVersion) try {
	std::vector<std::uint16_t> tokens = split(version, '.');
		
	if(tokens.size() != 3) {
		return VERSION_RELATION::UNKNOWN_VERSION;
	}

	if(tokens[0] == apiVersion.major) {
		if(tokens[1] == apiVersion.minor) {
			return VERSION_RELATION::OKAY;
		} else if(tokens[1] > apiVersion.minor) {
			return VERSION_RELATION::VERSION_IN_FUTURE;
		} else {
			return VERSION_RELATION::TENTATIVE_OKAY;
		}
	} else if(tokens[0] > apiVersion.major) {
		return VERSION_RELATION::VERSION_IN_FUTURE;
	} else {
		return VERSION_RELATION::OUT_OF_DATE;
	}
} catch(boost::bad_lexical_cast) {
	return VERSION_RELATION::UNKNOWN_VERSION;
}

}