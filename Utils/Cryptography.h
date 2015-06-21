#pragma once

#include <string>
#include <vector>

namespace Utils
{
	namespace Cryptography
	{
		bool GenerateRSAKeyPair(int numBits, std::string& privKey, std::string& pubKey);
	}
}