#include "String.h"

// STL
#include <string>

// people will hate me for this, but PHP/node/etc RSA funcs all use openssl, so we'll use it as well to make it easier on us
#include <openssl\rsa.h>
#include <openssl\bn.h>
#include <openssl\pem.h>

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
	}
}