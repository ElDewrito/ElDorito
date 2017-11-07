
// Disable warnings about using "safe" C functions
#pragma warning (disable : 4996)

#include "Signaling.hpp"

#include <algorithm>
#include <map>
#include <websocketpp/server.hpp>
#include <Windows.h>

#include "../Patches/CustomPackets.hpp"
#include "../Patches/Network.hpp"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Utils/WebSocket.hpp"
#include "../Utils/Cryptography.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Patches/Events.hpp"
#include "../Utils/Utils.hpp"
#include "../Patches/Core.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModuleUPnP.hpp"
#include "../Web/Ui/ScreenLayer.hpp"

#define PASSWORD_LENGTH (8)

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef websocketpp::server<Utils::WebSocket::Config> server;

using namespace Patches::CustomPackets;

namespace
{
	DWORD WINAPI SignalingThread(LPVOID);
	bool OnValidate(server* signalServer, websocketpp::connection_hdl hdl);
	void OnMessage(server* signalServer, websocketpp::connection_hdl hdl, server::message_ptr msg);
	void OnClose(server* signalServer, websocketpp::connection_hdl hdl);

	std::string ServerPortJson();

	struct coninfo
	{
		std::string uid;
		std::string subprotocol;
		bool authed;
		int peerIdx;
	};

	enum RejectionReason
	{
		eBadPassword,
		eSessionHasBadInfo,
		eNone
	};

	RejectionReason ProcessPassword(const char* echo, coninfo &info);
	void CreatePasswords();
	void ResetPassword(int playerIndex);
	void ForceStopServer();

	void LifeCycleChanged(Blam::Network::LifeCycleState state);

	std::string authStrings[Blam::Network::MaxPeers];
	std::map<websocketpp::connection_hdl, coninfo, std::owner_less<websocketpp::connection_hdl>> connectedSockets; //std::owner_less doesn't work with std::unordered_map

	static std::string currentPassword = "not-connected";
	static uint16_t port;

	struct WebSocketPacketData
	{
		char echoString[PASSWORD_LENGTH + 1];
		uint16_t serverPort;
	};

	typedef Packet<WebSocketPacketData> WebSocketPacket;
	typedef PacketSender<WebSocketPacketData> WebSocketPacketSender;

	class WebSocketPacketHandler : public PacketHandler<WebSocketPacketData>
	{
		void Serialize(Blam::BitStream *stream, const WebSocketPacketData *data) override;
		bool Deserialize(Blam::BitStream *stream, WebSocketPacketData *data) override;
		void HandlePacket(Blam::Network::ObserverChannel *sender, const WebSocketPacket *packet) override;
	};

	std::shared_ptr<WebSocketPacketSender> wspsender;
	std::shared_ptr<WebSocketPacketHandler> wsphandler;

	server signalServer;
	bool is_listening = false;
	bool setupDone = false;
}

namespace Server::Signaling
{
	void Initialize()
	{
		wsphandler = std::make_shared<WebSocketPacketHandler>();
		wspsender = RegisterPacket<WebSocketPacketData>("eldewrito-signal-server-echo", wsphandler);

		Patches::Network::OnLifeCycleStateChanged(LifeCycleChanged);
		Patches::Core::OnShutdown(ForceStopServer);
	}

	void StartServer()
	{
		auto session = Blam::Network::GetActiveSession();
		CreatePasswords();
		currentPassword = authStrings[session->MembershipInfo.HostPeerIndex];
		port = Modules::ModuleServer::Instance().VarSignalServerPort->ValueInt;
		Web::Ui::ScreenLayer::Notify("signal-ready", ServerPortJson(), true);
		if (Modules::ModuleUPnP::Instance().VarUPnPEnabled->ValueInt)
		{
			Modules::ModuleUPnP::Instance().UPnPForwardPort(true, port, port, "Eldewrito Signal Server");
		}
		if (!is_listening)
			CreateThread(nullptr, 0, SignalingThread, nullptr, 0, nullptr);
	}

	void StopServer()
	{
		if (is_listening)
		{
			signalServer.stop_listening(); //run will cleanly exit after all connections are stopped
			for each (auto client in connectedSockets)
			{
				signalServer.get_con_from_hdl(client.first)->close(websocketpp::close::status::going_away, "Server closing");
			}
			is_listening = false;
		}
	}

	void SendPeerPassword(int playerIndex)
	{
		auto packet = wspsender->New();
		auto *session = Blam::Network::GetActiveSession();
		auto peerIdx = session->MembershipInfo.GetPlayerPeer(playerIndex);
		ResetPassword(peerIdx);
		strncpy_s(packet.Data.echoString, authStrings[peerIdx].c_str(), PASSWORD_LENGTH + 1);
		packet.Data.serverPort = port;
		wspsender->Send(peerIdx, packet);
	}

	void RemovePeer(int peerIndex)
	{
		ResetPassword(peerIndex);
		for each (auto client in connectedSockets)
		{
			if (client.second.peerIdx == peerIndex)
			{
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				writer.StartObject();
				writer.Key("leave");
				writer.String(client.second.uid.c_str());
				writer.EndObject();
				for each(auto otherClient in connectedSockets)
				{
					if (!otherClient.second.subprotocol.compare(client.second.subprotocol))
					{
						signalServer.get_con_from_hdl(otherClient.first)->send(buffer.GetString());
					}
				}
				signalServer.get_con_from_hdl(client.first)->close(websocketpp::close::status::normal, "Left session");
			}
		}
	}

	std::string GetPassword()
	{
		return currentPassword;
	}

	uint16_t GetPort()
	{
		return port;
	}
}

namespace
{
	void LifeCycleChanged(Blam::Network::LifeCycleState state)
	{
		if (state == Blam::Network::eLifeCycleStateNone || state == Blam::Network::eLifeCycleStateLeaving)
		{
			currentPassword = "not-connected";
		}
	}

	//packets
	void WebSocketPacketHandler::Serialize(Blam::BitStream *stream, const WebSocketPacketData *data)
	{
		stream->WriteString(data->echoString);
		stream->WriteUnsigned<uint16_t>(port, 0, 65535);
	}

	bool WebSocketPacketHandler::Deserialize(Blam::BitStream *stream, WebSocketPacketData *data)
	{
		if(!stream->ReadString(data->echoString))
			return false;
		data->serverPort = stream->ReadUnsigned<uint16_t>(16);
		return true;
	}

	void WebSocketPacketHandler::HandlePacket(Blam::Network::ObserverChannel *sender, const WebSocketPacket *packet)
	{
		auto *session = Blam::Network::GetActiveSession();
		auto sendingPeer = session->GetChannelPeer(sender);
		if (sendingPeer < 0)
			return;

		if (session->IsHost())
			return;

		currentPassword = std::string(packet->Data.echoString);
		port = packet->Data.serverPort;

		Web::Ui::ScreenLayer::Notify("signal-ready", ServerPortJson(), true);
	}

	std::string ServerPortJson()
	{
		auto session = Blam::Network::GetActiveSession();
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		buffer.Clear();

		writer.StartObject();
		writer.Key("server");
		std::string ip = "";
		if (!session->GetServerIPAddress().ToString().compare("0.0.0.0") || !session->GetServerIPAddress().ToString().compare("1.0.0.0"))
			ip = "127.0.0.1:" + std::to_string(Server::Signaling::GetPort());
		else
			ip = session->GetServerIPAddress().ToString() + ":" + std::to_string(Server::Signaling::GetPort());
		writer.String(ip.c_str());

		writer.Key("password");
		writer.String(Server::Signaling::GetPassword().c_str());
		writer.EndObject();
		return buffer.GetString();
	}

	void ForceStopServer()
	{
		if(is_listening)
			signalServer.stop();
	}

	//websocket server
	DWORD WINAPI SignalingThread(LPVOID)
	{
		if (setupDone)
		{
			while (is_listening)//wait until server is ready to start back up
				Sleep(1);

			try
			{
				signalServer.listen(port);
				is_listening = true;
				signalServer.start_accept();
				signalServer.run();
				signalServer.reset();
				is_listening = false;
			}
			catch (websocketpp::exception const& e)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, "SignalServer: %s", e.what());
			}
			return 0;
		}

		try
		{
#ifdef _DEBUG
			signalServer.set_access_channels(websocketpp::log::alevel::all);
			signalServer.set_error_channels(websocketpp::log::elevel::all);
#else
			signalServer.set_access_channels(websocketpp::log::alevel::fail);
			signalServer.set_error_channels(websocketpp::log::elevel::warn | websocketpp::log::elevel::rerror | websocketpp::log::elevel::fatal);
#endif

			signalServer.init_asio();
			signalServer.set_validate_handler(websocketpp::lib::bind(OnValidate, &signalServer, _1));
			signalServer.set_message_handler(websocketpp::lib::bind(OnMessage, &signalServer, _1, _2));
			signalServer.set_close_handler(websocketpp::lib::bind(OnClose, &signalServer, _1));


			signalServer.listen(port);
			is_listening = true;
			setupDone = true;

			signalServer.start_accept();
			signalServer.run();
			signalServer.reset();
			is_listening = false;
		}
		catch (websocketpp::exception const& e)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, "SignalServer: %s", e.what());
		}
		return 0;
	}

	bool OnValidate(server* signalServer, websocketpp::connection_hdl hdl)
	{
		auto connection = signalServer->get_con_from_hdl(hdl);
		auto subprotocols = connection->get_requested_subprotocols();
		coninfo info = {
			"",
			subprotocols[0],
			false,
			-1
		};

		connectedSockets.insert(std::make_pair(hdl, info));
		connection->select_subprotocol(subprotocols[0]);
		return true;
	}

	void OnMessage(server* signalServer, websocketpp::connection_hdl hdl, server::message_ptr msg)
	{
		std::map<websocketpp::connection_hdl, coninfo>::iterator it = connectedSockets.find(hdl);
		if (it == connectedSockets.end())
			return;
		if (it->second.authed == false)
		{
			RejectionReason reject = ProcessPassword(msg->get_payload().c_str(), it->second);
			if (reject != RejectionReason::eNone)
			{
				if(reject == RejectionReason::eBadPassword)
					signalServer->get_con_from_hdl(hdl)->send("bad password");
				else if(reject == RejectionReason::eSessionHasBadInfo)
					signalServer->get_con_from_hdl(hdl)->send("try again later");
				return;
			}
		}

		rapidjson::Document doc;
		doc.Parse(msg->get_payload().c_str());
		if (doc.HasParseError())
			return;

		rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

		std::string proto = signalServer->get_con_from_hdl(hdl)->get_requested_subprotocols()[0];

		if (doc.HasMember("leave") || doc.HasMember("uid"))
			return;

		if (doc.HasMember("sendTo") && doc["sendTo"].IsString())
		{
			rapidjson::Value setUid;
			setUid.SetString(it->second.uid.c_str(), allocator);

			doc.AddMember("uid", setUid, allocator);

			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			doc.Accept(writer);

			for each (auto peer in connectedSockets)
			{
				if (peer.second.subprotocol == proto && peer.second.uid == doc["sendTo"].GetString())
				{
					signalServer->get_con_from_hdl(peer.first)->send(buffer.GetString());
					break;
				}
			}
		}
		else if (doc.HasMember("broadcast"))
		{
			rapidjson::Value setUid;
			setUid.SetString(it->second.uid.c_str(), allocator);

			doc.AddMember("uid", setUid, allocator);

			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			doc.Accept(writer);

			for each (auto peer in connectedSockets)
			{
				if (peer.second.authed && peer.second.subprotocol == proto && signalServer->get_con_from_hdl(hdl) != signalServer->get_con_from_hdl(peer.first))
					signalServer->get_con_from_hdl(peer.first)->send(buffer.GetString());
			}
		}
	}

	void OnClose(server* signalServer, websocketpp::connection_hdl hdl)
	{
		connectedSockets.erase(hdl);
	}

	RejectionReason ProcessPassword(const char* echo, coninfo &info)
	{
		auto *session = Blam::Network::GetActiveSession();
		int peerIdx = session->MembershipInfo.FindFirstPeer();
		while(peerIdx > -1)
		{
			if(std::strcmp(echo, authStrings[peerIdx].c_str()) == 0)
			{
				std::string name = Utils::String::ThinString(session->MembershipInfo.PlayerSessions[session->MembershipInfo.GetPeerPlayer(peerIdx)].Properties.DisplayName);

				if (name == "" || session->MembershipInfo.PlayerSessions[session->MembershipInfo.GetPeerPlayer(peerIdx)].Properties.Uid == 0)
					return RejectionReason::eSessionHasBadInfo;

				char uid[17];
				Blam::Players::FormatUid(uid, session->MembershipInfo.PlayerSessions[session->MembershipInfo.GetPeerPlayer(peerIdx)].Properties.Uid);

				std::stringstream ss;
				ss << name << "|" << uid; //unique
				info.uid = ss.str();
				info.authed = true;
				info.peerIdx = peerIdx;
				return RejectionReason::eNone;
			}
			peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
		}
		return RejectionReason::eBadPassword;
	}

	void CreatePasswords()
	{
		for (int i = 0; i < Blam::Network::MaxPeers; i++)
		{
			authStrings[i] = "";
			Utils::Cryptography::RandomPassword(PASSWORD_LENGTH, authStrings[i]);
		}
	}

	void ResetPassword(int peerIndex)
	{
		authStrings[peerIndex] = "";
		Utils::Cryptography::RandomPassword(PASSWORD_LENGTH, authStrings[peerIndex]);
	}
}
