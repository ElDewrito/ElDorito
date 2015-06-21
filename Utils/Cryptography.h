#pragma once

#include <string>
#include <vector>

namespace Utils
{
	namespace Cryptography
	{
		std::string ReformatKey(bool isPrivateKey, std::string key);
		bool CreateRSASignature(std::string privateKey, void* data, size_t dataSize, std::string& signature);
		bool GenerateRSAKeyPair(int numBits, std::string& privKey, std::string& pubKey);
	}
}