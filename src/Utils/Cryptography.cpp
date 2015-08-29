#include "String.hpp"

// STL
#include <string>
#include <sstream>
#include <cstdint>

// people will hate me for this, but PHP/node/etc RSA funcs all use openssl, so we'll use it as well to make it easier on us
#include <openssl\rsa.h>
#include <openssl\bn.h>
#include <openssl\pem.h>
#include <openssl\sha.h>

namespace Utils
{
	namespace Cryptography
	{
		static int genrsa_cb(int p, int n, BN_GENCB *cb)
		{
			char c = '*';

			if (p == 0)
				c = '.';
			if (p == 1)
				c = '+';
			if (p == 2)
				c = '*';
			if (p == 3)
				c = '\n';
			BIO_write((BIO*)cb->arg, &c, 1);
			(void)BIO_flush((BIO*)cb->arg);

			return 1;
		}

		std::string ReformatKey(bool isPrivateKey, std::string key)
		{
			size_t pos = 0;
			std::string returnKey;
			while (pos < key.length())
			{
				int toCopy = key.length() - pos;
				if (toCopy > 64)
					toCopy = 64;
				returnKey += key.substr(pos, toCopy);
				returnKey += "\n";
				pos += toCopy;
			}
			std::string keyType = (isPrivateKey ? "RSA PRIVATE KEY" : "PUBLIC KEY"); // public keys don't have RSA in the name some reason

			std::stringstream ss;
			ss << "-----BEGIN " << keyType << "-----\n" << returnKey << "-----END " << keyType << "-----\n";
			return ss.str();
		}

		bool CreateRSASignature(std::string privateKey, void* data, size_t dataSize, std::string& signature)
		{
			// privateKey has to be reformatted with -----RSA PRIVATE KEY----- header/footer and newlines after every 64 chars
			// before calling this function

			BIO* privKeyBuff = BIO_new_mem_buf((void*)privateKey.c_str(), privateKey.length());
			if (!privKeyBuff)
				return false;

			RSA* rsa = PEM_read_bio_RSAPrivateKey(privKeyBuff, 0, 0, 0);
			if (!rsa)
				return false;

			unsigned char hash[SHA256_DIGEST_LENGTH];
			SHA256_CTX sha;
			SHA256_Init(&sha);
			SHA256_Update(&sha, data, dataSize);
			SHA256_Final(hash, &sha);

			void* sigret = malloc(RSA_size(rsa));
			unsigned int siglen = 0;
			int retVal = RSA_sign(NID_sha256, (unsigned char*)hash, SHA256_DIGEST_LENGTH, (unsigned char*)sigret, &siglen, rsa);

			RSA_free(rsa);
			BIO_free_all(privKeyBuff);

			if (retVal != 1)
				return false;

			signature = Utils::String::Base64Encode((unsigned char*)sigret, siglen);
			return true;
		}

		bool VerifyRSASignature(std::string pubKey, std::string signature, void* data, size_t dataSize)
		{
			BIO* pubKeyBuff = BIO_new_mem_buf((void*)pubKey.c_str(), pubKey.length());
			if (!pubKeyBuff)
				return false;

			RSA* rsa = PEM_read_bio_RSA_PUBKEY(pubKeyBuff, 0, 0, 0);
			if (!rsa)
				return false;

			unsigned char hash[SHA256_DIGEST_LENGTH];
			SHA256_CTX sha;
			SHA256_Init(&sha);
			SHA256_Update(&sha, data, dataSize);
			SHA256_Final(hash, &sha);

			size_t length = 0;
			if (Utils::String::Base64DecodeBinary((char*)signature.c_str(), NULL, &length) != 1 || length == 0)
				return false;

			unsigned char* sigBuf = (unsigned char*)malloc(length);
			if (Utils::String::Base64DecodeBinary((char*)signature.c_str(), sigBuf, &length) != 0)
				return false;

			int retVal = RSA_verify(NID_sha256, (unsigned char*)hash, SHA256_DIGEST_LENGTH, sigBuf, length, rsa);
			free(sigBuf);
			RSA_free(rsa);
			BIO_free_all(pubKeyBuff);

			return retVal == 1;
		}

		bool GenerateRSAKeyPair(int numBits, std::string& privKey, std::string& pubKey)
		{
			// TODO: add some error checking
			RSA* rsa = RSA_new();
			BIGNUM* bn = BN_new();
			BN_GENCB cb;
			BIO* bio_err = NULL;
			BN_GENCB_set(&cb, genrsa_cb, bio_err);
			BN_set_word(bn, RSA_F4);
			RSA_generate_key_ex(rsa, numBits, bn, &cb);

			BIO* privKeyBuff = BIO_new(BIO_s_mem());
			BIO* pubKeyBuff = BIO_new(BIO_s_mem());
			PEM_write_bio_RSAPrivateKey(privKeyBuff, rsa, 0, 0, 0, 0, 0);
			PEM_write_bio_RSA_PUBKEY(pubKeyBuff, rsa); // RSA_PUBKEY includes some data that RSAPublicKey doesn't have

			char* privKeyData;
			char* pubKeyData;
			auto privKeySize = BIO_get_mem_data(privKeyBuff, &privKeyData);
			auto pubKeySize = BIO_get_mem_data(pubKeyBuff, &pubKeyData);

			privKey = std::string(privKeyData, privKeySize);
			pubKey = std::string(pubKeyData, pubKeySize);
			
			BIO_free_all(privKeyBuff);
			BIO_free_all(pubKeyBuff);
			BN_free(bn);
			RSA_free(rsa);
			return true;
		}

		bool Hash32(const std::string& str, uint32_t *out)
		{
			// Generate a SHA-1 digest and take the first 4 bytes
			unsigned char digest[SHA_DIGEST_LENGTH];
			SHA_CTX context;
			if (!SHA1_Init(&context))
				return false;
			if (!SHA1_Update(&context, str.c_str(), str.length()))
				return false;
			if (!SHA1_Final(digest, &context))
				return false;
			*out = digest[0] << 24 | digest[1] << 16 | digest[2] << 8 | digest[3];
			return true;
		}
	}
}