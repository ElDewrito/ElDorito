#include "PlayerUid.h"

#include "../ElDorito.h"
#include "../Modules/ModulePlayer.h"
#include "../Patch.h"
#include "PlayerPropertiesExtension.h"
#include "../Utils/Cryptography.h"

#include <openssl/sha.h>

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
	void EnsureValidUid()
	{
		if (UidValidPtr.Read<bool>())
			return; // UID is already set

		// Try to pull the UID from preferences
		auto& playerVars = Modules::ModulePlayer::Instance();
		std::string pubKey = playerVars.VarPlayerPubKey->ValueString;
		uint64_t uid;
		if (pubKey.length() <= 0)
		{
			std::cout << "Generating player keypair, this might take a moment..." << std::endl;
			std::string privKey;
			Utils::Cryptography::GenerateRSAKeyPair(4096, privKey, pubKey);

			// strip headers/footers from PEM keys so they can be stored in cfg / transmitted over JSON easier
			// TODO: find if we can just store the private key and generate the pub key from it when its needed
			Utils::String::ReplaceString(privKey, "\n", "");
			Utils::String::ReplaceString(privKey, "-----BEGIN RSA PRIVATE KEY-----", "");
			Utils::String::ReplaceString(privKey, "-----END RSA PRIVATE KEY-----", "");
			Utils::String::ReplaceString(pubKey, "\n", "");
			Utils::String::ReplaceString(pubKey, "-----BEGIN PUBLIC KEY-----", "");
			Utils::String::ReplaceString(pubKey, "-----END PUBLIC KEY-----", "");

			Modules::CommandMap::Instance().SetVariable(playerVars.VarPlayerPrivKey, privKey, std::string());
			Modules::CommandMap::Instance().SetVariable(playerVars.VarPlayerPubKey, pubKey, std::string());

			std::cout << "Done!" << std::endl;
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