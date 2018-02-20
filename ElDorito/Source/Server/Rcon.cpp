
// Disable warnings about using "safe" C functions
#pragma warning (disable : 4996)

#include "Rcon.hpp"

#include <algorithm>
#include <set>
#include <websocketpp/server.hpp>
#include <Windows.h>

#include "../Patches/Core.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Utils/Cryptography.hpp"
#include "../Utils/WebSocket.hpp"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef websocketpp::server<Utils::WebSocket::Config> server;

namespace
{
	void ForceStopServer();
	server rconServer;
	DWORD WINAPI RconThread(LPVOID);
	bool OnValidate(server* rconServer, websocketpp::connection_hdl hdl);
	void OnMessage(server* rconServer, websocketpp::connection_hdl hdl, server::message_ptr msg);
	void ProcessCommand(server* rconServer, websocketpp::connection_hdl hdl, server::message_ptr msg);
	void ProcessPassword(server* rconServer, websocketpp::connection_hdl hdl, server::message_ptr msg);
	void OnClose(server* rconServer, websocketpp::connection_hdl hdl);
	void OnTimer(server* rconServer, websocketpp::lib::error_code const & ec);
	std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> authenticatedConnections;
	std::vector<std::string> sendThroughWebsocket = std::vector <std::string>{};


	const int DefaultPasswordLength = 32;
	const char* ProtocolName = "dew-rcon";
	const char* AcceptMessage = "accept";
	const char* DenyMessage = "deny";
}

namespace Server::Rcon
{
	void Initialize()
	{
		Patches::Core::OnShutdown(ForceStopServer);
		if (Modules::ModuleServer::Instance().VarRconPassword->ValueString == "")
		{
			// Ensure a password is set
			Utils::Cryptography::RandomPassword(DefaultPasswordLength, Modules::ModuleServer::Instance().VarRconPassword->ValueString);
			Modules::CommandMap::Instance().ExecuteCommand("WriteConfig");
		}
		CreateThread(nullptr, 0, RconThread, nullptr, 0, nullptr);
	}
	void SendMessageToClients(std::string message) {
		sendThroughWebsocket.push_back(message);
	}
}

namespace
{
	void ForceStopServer()
	{
		if (rconServer.is_listening())
			rconServer.stop();
	}

	DWORD WINAPI RconThread(LPVOID)
	{
		try
		{
#ifdef _DEBUG
			rconServer.set_access_channels(websocketpp::log::alevel::all);
			rconServer.set_error_channels(websocketpp::log::elevel::all);
#else
			rconServer.set_access_channels(websocketpp::log::alevel::fail);
			rconServer.set_error_channels(websocketpp::log::elevel::warn | websocketpp::log::elevel::rerror | websocketpp::log::elevel::fatal);
#endif

			rconServer.init_asio();
			rconServer.set_validate_handler(websocketpp::lib::bind(OnValidate, &rconServer, _1));
			rconServer.set_message_handler(websocketpp::lib::bind(OnMessage, &rconServer, _1, _2));
			rconServer.set_close_handler(websocketpp::lib::bind(OnClose, &rconServer, _1));
			rconServer.set_timer(1000, websocketpp::lib::bind(OnTimer, &rconServer, _1));

			auto port = Modules::ModuleGame::Instance().VarRconPort->ValueInt;
			rconServer.listen(static_cast<uint16_t>(port));
			rconServer.start_accept();
			rconServer.run();
		}
		catch (websocketpp::exception const& e)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, "websocketpp: %s", e.what());
		}
		return 0;
	}

	bool OnValidate(server* rconServer, websocketpp::connection_hdl hdl)
	{
		// Require the client to connect with the proper protocol
		auto connection = rconServer->get_con_from_hdl(hdl);
		auto subprotocols = connection->get_requested_subprotocols();
		if (std::find(subprotocols.begin(), subprotocols.end(), ProtocolName) == subprotocols.end())
			return false;
		connection->select_subprotocol(ProtocolName);
		return true;
	}

	void OnMessage(server* rconServer, websocketpp::connection_hdl hdl, server::message_ptr msg)
	{
		try
		{
			// If the connection is authenticated, then treat input as a command, otherwise treat it as a password
			if (authenticatedConnections.find(hdl) != authenticatedConnections.end())
				ProcessCommand(rconServer, hdl, msg);
			else
				ProcessPassword(rconServer, hdl, msg);
		}
		catch (const websocketpp::lib::error_code& e)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, "websocketpp: %s", e.message());
		}
	}
	void OnTimer(server* rconServer, websocketpp::lib::error_code const & ec)
	{

		for (auto it : authenticatedConnections) {
			for (auto m : sendThroughWebsocket) {
				rconServer->send(it, m, websocketpp::frame::opcode::text);
			}
		}
		sendThroughWebsocket.clear();
		
		rconServer->set_timer(1000, websocketpp::lib::bind(OnTimer, rconServer, _1));
	}
	void ProcessCommand(server* rconServer, websocketpp::connection_hdl hdl, server::message_ptr msg)
	{
		auto output = Modules::CommandMap::Instance().ExecuteCommand(msg->get_payload(), true);
		rconServer->send(hdl, output, websocketpp::frame::opcode::TEXT);
	}

	void ProcessPassword(server* rconServer, websocketpp::connection_hdl hdl, server::message_ptr msg)
	{
		auto inPassword = msg->get_payload();
		auto actualPassword = Modules::ModuleServer::Instance().VarRconPassword->ValueString;
		if (inPassword == actualPassword)
		{
			// Mark the connection as authenticated
			authenticatedConnections.insert(hdl);
			rconServer->send(hdl, AcceptMessage, websocketpp::frame::opcode::TEXT);
		}
		else
		{
			// Close the connection
			rconServer->send(hdl, DenyMessage, websocketpp::frame::opcode::TEXT);
			rconServer->close(hdl, websocketpp::close::status::policy_violation, DenyMessage);
		}
	}

	void OnClose(server* rconServer, websocketpp::connection_hdl hdl)
	{
		authenticatedConnections.erase(hdl);
	}
}