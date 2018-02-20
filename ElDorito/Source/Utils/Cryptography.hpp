#pragma once

#include <string>
#include <vector>

namespace Utils::Cryptography
{
	std::string ReformatKey(bool isPrivateKey, std::string key);
	bool PrivateEncrypt(std::string privateKey, void* data, size_t dataSize, std::string& encrypted);
	bool CreateRSASignature(std::string privateKey, void* data, size_t dataSize, std::string& signature);
	bool VerifyRSASignature(std::string pubKey, std::string signature, void* data, size_t dataSize);
	bool GenerateRSAKeyPair(int numBits, std::string& privKey, std::string& pubKey);
	bool Hash32(const std::string& str, uint32_t *out);
	bool RandomBytes(int num, uint8_t *out);
	bool RandomString(int length, const std::string& chars, std::string& out);
	bool RandomPassword(int length, std::string& out);
}
