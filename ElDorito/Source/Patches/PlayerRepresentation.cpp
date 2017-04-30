#include <cstdint>
#include "../Blam/BlamData.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "PlayerRepresentation.hpp"
#include "../Patches/PlayerPropertiesExtension.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Blam/Cache/StringIdCache.hpp"
#include "../Patch.hpp"

namespace
{
	struct RepresentationData
	{
		uint32_t RepresentationNameId;
	};

	class PlayerRepresentationExtensions : public Patches::Network::PlayerPropertiesExtension<RepresentationData>
	{
	protected:
		void BuildData(int playerIndex, RepresentationData *out) override
		{
			auto& representation = Modules::ModulePlayer::Instance().VarRepresentation->ValueString;

			if (representation == std::string("elite"))
				out->RepresentationNameId = 0xCC;
			else
				out->RepresentationNameId = 0x129;
			
		}

		void ApplyData(int playerIndex, Blam::Players::PlayerProperties *properties, const RepresentationData &data) override
		{
			// just elite for now...
			switch (data.RepresentationNameId)
			{
			case 0xCC:
				properties->PlayerRepresentation = 1; // elite
				break;
			default:
				properties->PlayerRepresentation = 0; // spartan
				break;
			}

			auto activeSession = Blam::Network::GetActiveSession();
			if (activeSession && activeSession->IsEstablished() && activeSession->IsHost())
			{
				activeSession->MembershipInfo.Update();
			}
		}

		void Serialize(Blam::BitStream *stream, const RepresentationData &data) override
		{
			stream->WriteUnsigned<uint32_t>(data.RepresentationNameId, 0, 0xFFFFFFFF);
		}

		void Deserialize(Blam::BitStream *stream, RepresentationData *out) override
		{
			out->RepresentationNameId = stream->ReadUnsigned<uint32_t>(0, 0xFFFFFFFF);
		}
	};
}


namespace Patches
{
	namespace PlayerRepresentation
	{
		void ApplyAll()
		{
			Patches::Network::PlayerPropertiesExtender::Instance().Add(std::make_unique<PlayerRepresentationExtensions>());
		}

		void UpdateLocalRepresentation()
		{
			auto activeSession = Blam::Network::GetActiveSession();
			if (activeSession && activeSession->IsEstablished())
			{
				// this will allow the player to change in-game, which is fine for testing, but it'll need to be locked down before release.
				auto Network_session_update_user_properties = (signed __int32(__cdecl*)(Blam::Network::Session* session, int a2))(0x00437B30);
				Network_session_update_user_properties(activeSession, 0);
			}
		}
	}
}