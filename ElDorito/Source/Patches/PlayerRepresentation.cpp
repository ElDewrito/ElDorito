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
#include "../Utils/String.hpp"

namespace
{
	struct RepresentationData
	{
		uint32_t RepresentationNameId;
		wchar_t ServiceTag[6];
		int8_t Gender;
	};

	class PlayerRepresentationExtensions : public Patches::Network::PlayerPropertiesExtension<RepresentationData>
	{
	protected:
		void BuildData(int playerIndex, RepresentationData *out) override
		{
			const auto &modulePlayer = Modules::ModulePlayer::Instance();
			// representation
			if (modulePlayer.VarRepresentation->ValueString == std::string("elite"))
				out->RepresentationNameId = 0xCC;
			else
				out->RepresentationNameId = 0x129;
			// service tag
			auto serviceTag = Utils::String::WidenString(modulePlayer.VarPlayerServiceTag->ValueString);
			wcsncpy_s(out->ServiceTag, serviceTag.c_str(), 5);
			// gender
			out->Gender = !modulePlayer.VarPlayerGender->ValueString.compare("female") ? 1 : 0;
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

			wcsncpy_s(properties->ServiceTag, data.ServiceTag, 5);
			properties->Gender = data.Gender;

			auto activeSession = Blam::Network::GetActiveSession();
			if (activeSession && activeSession->IsEstablished() && activeSession->IsHost())
			{
				activeSession->MembershipInfo.Update();
			}
		}

		void Serialize(Blam::BitStream *stream, const RepresentationData &data) override
		{
			stream->WriteUnsigned<uint32_t>(data.RepresentationNameId, 0, 0xFFFFFFFF);
			stream->WriteString(data.ServiceTag);
			stream->WriteBool(data.Gender);
		}

		void Deserialize(Blam::BitStream *stream, RepresentationData *out) override
		{
			out->RepresentationNameId = stream->ReadUnsigned<uint32_t>(0, 0xFFFFFFFF);
			stream->ReadString(out->ServiceTag);
			out->Gender = stream->ReadBool();
		}
	};
}


namespace Patches::PlayerRepresentation
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