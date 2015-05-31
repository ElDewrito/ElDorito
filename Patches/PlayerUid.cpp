#include "PlayerUid.h"

#include "../ElDorito.h"
#include "../Modules/ModulePlayer.h"
#include "../Patch.h"
#include "PlayerPropertiesExtension.h"

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
	uint64_t GenerateUid()
	{
		// Use CryptGenRandom to generate a random UID
		// TODO: Should we panic if this fails?
		HCRYPTPROV provider;
		if (!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
			return 0;
		uint64_t result = 0;
		while (result == 0)
		{
			if (!CryptGenRandom(provider, sizeof(result), reinterpret_cast<BYTE*>(&result)))
				break;
		}
		CryptReleaseContext(provider, 0);
		return result;
	}

	void EnsureValidUid()
	{
		if (UidValidPtr.Read<bool>())
			return; // UID is already set

		// Try to pull the UID from preferences
		auto& playerVars = Modules::ModulePlayer::Instance();
		uint64_t uid = playerVars.VarUserID->ValueInt64;
		if (uid == 0)
		{
			// Generate a new UID and save it
			uid = GenerateUid();
			Modules::CommandMap::Instance().SetVariable(playerVars.VarUserID, std::to_string(uid), std::string());
		}
		UidPtr.Write<uint64_t>(uid);
		UidValidPtr.Write(true);
	}

	uint64_t GetPlayerUidHook(int unused)
	{
		return Patches::PlayerUid::Get();
	}
}