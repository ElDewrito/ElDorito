#include "PlayerUid.h"

#include "../ElDorito.h"
#include "../Modules/ModulePlayer.h"
#include "../Patch.h"
#include "PlayerPropertiesExtension.h"

// people will hate me for this, but PHP/node/etc RSA funcs all use openssl, so we'll use it as well to make it easier on us
#include <openssl\rsa.h>
#include <openssl\bn.h>
#include <openssl\pem.h>

namespace
{
	void EnsureValidUid();
	uint64_t GetPlayerUidHook(int unused);

	Pointer UidValidPtr = Pointer::Base(0x15AB728); // true if the UID is set
	Pointer UidPtr = Pointer::Base(0x15AB730);      // The local player's UID

	// Player properties packet extension to send player UID
	class UidExtension : public Patches::Network::PlayerPropertiesExtension<uint64_t>
	{
	protected:
		void BuildData(int playerIndex, uint64_t *out)
		{
			*out = Patches::PlayerUid::Get();
		}

		void ApplyData(int playerIndex, void *session, const uint64_t &data)
		{
			*reinterpret_cast<uint64_t*>(static_cast<uint8_t*>(session) + 0x50) = data;
		}

		void Serialize(Blam::BitStream *stream, const uint64_t &data)
		{
			stream->WriteUnsigned(data, 64);
		}

		void Deserialize(Blam::BitStream *stream, uint64_t *out)
		{
			*out = stream->ReadUnsigned<uint64_t>(64);
		}
	};
}

namespace Patches
{
	namespace PlayerUid
	{
		void ApplyAll()
		{
			// Override the "get UID" function to pull the UID from preferences
			Hook(0x67E005, GetPlayerUidHook, HookFlags::IsCall).Apply();

			// Register the player-properties packet extension
			Network::PlayerPropertiesExtender::Instance().Add(std::make_shared<UidExtension>());
		}

		uint64_t Get()
		{
			EnsureValidUid();
			return UidPtr.Read<uint64_t>();
		}
	}
}

namespace
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

	void GenerateKeyPair()
	{
		RSA* rsa = RSA_new();
		BIGNUM* bn = BN_new();
		BN_GENCB cb;
		BIO* bio_err = NULL;
		BN_GENCB_set(&cb, genrsa_cb, bio_err);
		BN_set_word(bn, RSA_F4);
		RSA_generate_key_ex(rsa, 4096, bn, &cb);

		BIO* privKeyBuff = BIO_new(BIO_s_mem());
		BIO* pubKeyBuff = BIO_new(BIO_s_mem());
		PEM_write_bio_RSAPrivateKey(privKeyBuff, rsa, 0, 0, 0, 0, 0);
		PEM_write_bio_RSA_PUBKEY(pubKeyBuff, rsa); // RSA_PUBKEY includes some data that RSAPublicKey doesn't have

		char* privKeyData;
		char* pubKeyData;
		auto privKeySize = BIO_get_mem_data(privKeyBuff, &privKeyData);
		auto pubKeySize = BIO_get_mem_data(pubKeyBuff, &pubKeyData);

		auto privKey = std::string(privKeyData, privKeySize);
		auto pubKey = std::string(pubKeyData, pubKeySize);
		// strip headers/footers from PEM keys so they can be stored in cfg / transmitted over JSON easier
		// TODO: find if we can just store the private key and generate the pub key from it when its needed
		Utils::String::ReplaceString(privKey, "\n", "");
		Utils::String::ReplaceString(privKey, "-----BEGIN RSA PRIVATE KEY-----", "");
		Utils::String::ReplaceString(privKey, "-----END RSA PRIVATE KEY-----", "");
		Utils::String::ReplaceString(pubKey, "\n", "");
		Utils::String::ReplaceString(pubKey, "-----BEGIN PUBLIC KEY-----", "");
		Utils::String::ReplaceString(pubKey, "-----END PUBLIC KEY-----", "");

		auto& playerVars = Modules::ModulePlayer::Instance();
		Modules::CommandMap::Instance().SetVariable(playerVars.VarPlayerPrivKey, privKey, std::string());
		Modules::CommandMap::Instance().SetVariable(playerVars.VarPlayerPubKey, pubKey, std::string());
		BIO_free_all(privKeyBuff);
		BIO_free_all(pubKeyBuff);
		BN_free(bn);
		RSA_free(rsa);
	}

	void EnsureValidUid()
	{
		if (UidValidPtr.Read<bool>())
			return; // UID is already set

		// Try to pull the UID from preferences
		auto pubKey = Modules::ModulePlayer::Instance().VarPlayerPubKey->ValueString;
		uint64_t uid;
		if (pubKey.length() <= 0)
		{
			std::cout << "Generating player keypair, this might take a moment..." << std::endl;
			GenerateKeyPair();
			std::cout << "Done!" << std::endl;
			pubKey = Modules::ModulePlayer::Instance().VarPlayerPubKey->ValueString;
			// save the keypair
			Modules::CommandMap::Instance().ExecuteCommand("WriteConfig");
		}

		unsigned char hash[SHA256_DIGEST_LENGTH];
		SHA256_CTX sha256;
		SHA256_Init(&sha256);
		SHA256_Update(&sha256, pubKey.c_str(), pubKey.length());
		SHA256_Final(hash, &sha256);
		memcpy(&uid, hash, sizeof(uint64_t)); // use first 8 bytes of SHA256(pubKey) as UID

		UidPtr.Write<uint64_t>(uid);
		UidValidPtr.Write(true);
	}

	uint64_t GetPlayerUidHook(int unused)
	{
		return Patches::PlayerUid::Get();
	}
}