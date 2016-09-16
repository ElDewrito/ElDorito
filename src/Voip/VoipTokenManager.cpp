#include "VoipTokenManager.hpp"
#include "VoipServer.hpp"

#include "../Patches/CustomPackets.hpp"
#include "../Utils/Cryptography.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/String.hpp"

using namespace Patches::CustomPackets;

namespace Voip
{
	namespace
	{
		AuthToken MyToken;    // This client's current token
		bool MyTokenIsValid;  // True if the current token can be used to connect
		bool RequestingToken; // True if a token request has been made

		AuthToken AuthTokens[Blam::Network::MaxPeers]; // (Host only) Tokens for each peer
		Blam::Network::PeerBitSet ValidAuthTokens;     // (Host only) If the bit for a peer is set, then its voip token is valid

		// VoIP Token Request Packet

		struct AuthTokenRequestPacketData
		{
			// No data, the packet is just a simple indication that a client wants to join VoIP
		};

		typedef Packet<AuthTokenRequestPacketData> AuthTokenRequestPacket;
		typedef PacketSender<AuthTokenRequestPacketData> AuthTokenRequestPacketSender;

		class AuthTokenRequestPacketHandler : public PacketHandler<AuthTokenRequestPacketData>
		{
			void Serialize(Blam::BitStream *stream, const AuthTokenRequestPacketData *data) override;
			bool Deserialize(Blam::BitStream *stream, AuthTokenRequestPacketData *data) override;
			void HandlePacket(Blam::Network::ObserverChannel *sender, const AuthTokenRequestPacket *packet) override;
		};

		std::shared_ptr<AuthTokenRequestPacketSender> AuthTokenRequestSender;
		std::shared_ptr<AuthTokenRequestPacketHandler> AuthTokenRequestHandler;

		// VoIP Token Response Packet

		enum class AuthTokenResponseStatus
		{
			Valid,            // The token is valid and can be used to connect
			AlreadyConnected, // The peer is already connected to VoIP
			Banned,           // The peer has been banned from VoIP
			Error,            // An unknown error occurred while generating a token

			Count
		};

		struct AuthTokenResponsePacketData
		{
			AuthTokenResponseStatus Status; // The token is only valid if this is AuthTokenResponseStatus::Valid
			AuthToken Token;                // The one-time-use token the client can use to connect to VoIP
		};

		typedef Packet<AuthTokenResponsePacketData> AuthTokenResponsePacket;
		typedef PacketSender<AuthTokenResponsePacketData> AuthTokenResponsePacketSender;

		class AuthTokenResponsePacketHandler : public PacketHandler<AuthTokenResponsePacketData>
		{
			void Serialize(Blam::BitStream *stream, const AuthTokenResponsePacketData *data) override;
			bool Deserialize(Blam::BitStream *stream, AuthTokenResponsePacketData *data) override;
			void HandlePacket(Blam::Network::ObserverChannel *sender, const AuthTokenResponsePacket *packet) override;
		};

		std::shared_ptr<AuthTokenResponsePacketSender> AuthTokenResponseSender;
		std::shared_ptr<AuthTokenResponsePacketHandler> AuthTokenResponseHandler;

		std::string GetTokenString(const AuthToken token);
		bool GeneratePeerToken(int peerIndex);
		void SetAvailableToken(const AuthToken token);
	}

	namespace TokenManager
	{
		void Initialize()
		{
			AuthTokenRequestHandler = std::make_shared<AuthTokenRequestPacketHandler>();
			AuthTokenResponseHandler = std::make_shared<AuthTokenResponsePacketHandler>();
			AuthTokenRequestSender = RegisterPacket<AuthTokenRequestPacketData>("eldewrito-voip-auth-request", AuthTokenRequestHandler);
			AuthTokenResponseSender = RegisterPacket<AuthTokenResponsePacketData>("eldewrito-voip-auth-response", AuthTokenResponseHandler);
		}

		void RequestToken()
		{
			MyTokenIsValid = false;
			auto session = Blam::Network::GetActiveSession();
			if (!session || !session->IsEstablished())
				return;
			if (session->IsHost())
			{
				// Generate a token locally
				auto localPeer = session->MembershipInfo.LocalPeerIndex;
				if (GeneratePeerToken(localPeer))
					SetAvailableToken(AuthTokens[localPeer]);
			}
			else
			{
				// Send the host a packet requesting a token
				auto hostPeer = session->MembershipInfo.HostPeerIndex;
				Utils::Logger::Instance().Log(Utils::LogTypes::Voice, Utils::LogLevel::Info, "Sending connection token request to host (%d)", hostPeer);
				RequestingToken = true;
				auto request = AuthTokenRequestSender->New();
				AuthTokenRequestSender->Send(hostPeer, request);
			}
		}

		bool GetAvailableToken(AuthToken *dest)
		{
			if (!MyTokenIsValid)
				return false;
			memcpy(*dest, MyToken, sizeof(AuthToken));
			memset(MyToken, 0, sizeof(AuthToken));
			MyTokenIsValid = false;
			return true;
		}

		int CheckToken(const AuthToken token)
		{
			for (size_t i = 0; i < ValidAuthTokens.size(); i++)
			{
				if (ValidAuthTokens[i] && memcmp(token, AuthTokens[i], sizeof(AuthToken)) == 0)
				{
					InvalidateToken(i);
					return i;
				}
			}
			return -1;
		}

		void InvalidateToken(int peerIndex)
		{
			if (peerIndex >= 0 && peerIndex < static_cast<int>(ValidAuthTokens.size()))
			{
				ValidAuthTokens[peerIndex] = false;
				memset(AuthTokens[peerIndex], 0, sizeof(AuthToken));
			}
		}
	}

	namespace
	{
		std::string GetTokenString(const AuthToken token)
		{
			std::string result;
			Utils::String::BytesToHexString(token, sizeof(AuthToken), result);
			return result;
		}

		bool GeneratePeerToken(int peerIndex)
		{
			if (!Utils::Cryptography::RandomBytes(sizeof(AuthToken), AuthTokens[peerIndex]))
			{
				ValidAuthTokens[peerIndex] = false;
				Utils::Logger::Instance().Log(Utils::LogTypes::Voice, Utils::LogLevel::Error, "Failed to generate connection token for peer %d", peerIndex);
				return false;
			}
			ValidAuthTokens[peerIndex] = true;
			return true;
		}

		void SetAvailableToken(const AuthToken token)
		{
			memcpy(MyToken, token, sizeof(AuthToken));
			MyTokenIsValid = true;
		}

		void AuthTokenRequestPacketHandler::Serialize(Blam::BitStream *stream, const AuthTokenRequestPacketData *data)
		{
		}

		bool AuthTokenRequestPacketHandler::Deserialize(Blam::BitStream *stream, AuthTokenRequestPacketData *data)
		{
			return true;
		}

		void AuthTokenRequestPacketHandler::HandlePacket(Blam::Network::ObserverChannel *sender, const AuthTokenRequestPacket *packet)
		{
			// Get the index of the peer who requested a token
			auto session = Blam::Network::GetActiveSession();
			if (!session->IsHost())
				return;
			auto senderPeer = session->GetChannelPeer(sender);
			if (senderPeer < 0)
				return;

			// Try to generate a token and send it back
			// TODO: VoIP-only bans?
			auto response = AuthTokenResponseSender->New();
			memset(response.Data.Token, 0, sizeof(AuthToken));
			response.Data.Status = AuthTokenResponseStatus::Error;
			if (Server::IsPeerConnected(senderPeer))
			{
				// Peers can only connect once - don't give them another token
				response.Data.Status = AuthTokenResponseStatus::AlreadyConnected;
			}
			else if (GeneratePeerToken(senderPeer))
			{
				// Token generation successful - send it back
				memcpy(response.Data.Token, AuthTokens[senderPeer], sizeof(AuthToken));
				response.Data.Status = AuthTokenResponseStatus::Valid;
			}
			AuthTokenResponseSender->Send(senderPeer, response);
		}

		void AuthTokenResponsePacketHandler::Serialize(Blam::BitStream* stream, const AuthTokenResponsePacketData* data)
		{
			stream->WriteUnsigned(static_cast<uint32_t>(data->Status), 0U, static_cast<uint32_t>(AuthTokenResponseStatus::Count) - 1);
			stream->WriteBlock(sizeof(AuthToken) * 8, data->Token);
		}

		bool AuthTokenResponsePacketHandler::Deserialize(Blam::BitStream* stream, AuthTokenResponsePacketData* data)
		{
			auto status = stream->ReadUnsigned<uint32_t>(0U, static_cast<uint32_t>(AuthTokenResponseStatus::Count) - 1);
			data->Status = static_cast<AuthTokenResponseStatus>(status);
			stream->ReadBlock(sizeof(AuthToken) * 8, data->Token);
			return true;
		}

		void AuthTokenResponsePacketHandler::HandlePacket(Blam::Network::ObserverChannel* sender, const AuthTokenResponsePacket* packet)
		{
			if (!RequestingToken)
				return;
			RequestingToken = false;
			switch (packet->Data.Status)
			{
			case AuthTokenResponseStatus::Valid:
			{
				SetAvailableToken(packet->Data.Token);
				auto tokenStr = GetTokenString(MyToken);
				Utils::Logger::Instance().Log(
					static_cast<Utils::LogTypes>(Utils::LogTypes::Voice | Utils::LogTypes::Debug),
					Utils::LogLevel::Info,
					"Connection token request succeeded - got token %s", tokenStr.c_str());
				break;
			}
			case AuthTokenResponseStatus::AlreadyConnected:
				Utils::Logger::Instance().Log(
					Utils::LogTypes::Voice,
					Utils::LogLevel::Info,
					"Connection token request failed - already connected to VoIP");
				break;
			case AuthTokenResponseStatus::Banned:
				Utils::Logger::Instance().Log(
					Utils::LogTypes::Voice,
					Utils::LogLevel::Error,
					"Connection token request failed - banned");
				break;
			case AuthTokenResponseStatus::Error:
				Utils::Logger::Instance().Log(
					Utils::LogTypes::Voice,
					Utils::LogLevel::Error,
					"Connection token request failed - server-side error");
				break;
			default:
				Utils::Logger::Instance().Log(
					Utils::LogTypes::Voice,
					Utils::LogLevel::Error,
					"Connection token request failed - received unknown status code %d",
					static_cast<int>(packet->Data.Status));
				break;
			}
		}
	}
}