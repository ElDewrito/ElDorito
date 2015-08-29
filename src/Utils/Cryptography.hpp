#pragma once

#include <string>
#include <vector>

namespace Utils
{
	namespace Cryptography
	{
		std::string ReformatKey(bool isPrivateKey, std::string key);
		bool CreateRSASignature(std::string privateKey, void* data, size_t dataSize, std::string& signature);
		bool VerifyRSASignature(std::string pubKey, std::string signature, void* data, size_t dataSize);
		bool GenerateRSAKeyPair(int numBits, std::string& privKey, std::string& pubKey);
		bool Hash32(const std::string& str, uint32_t *out);
	}
}