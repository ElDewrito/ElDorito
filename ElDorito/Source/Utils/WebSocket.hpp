#pragma once

#include <websocketpp/config/core.hpp>
#include <websocketpp/transport/asio/endpoint.hpp>
#include <websocketpp/logger/levels.hpp>

#include "Logger.hpp"

namespace Utils::WebSocket
{
	// Websocket++ logger implementation that forwards to the ED logger
	template <typename concurrency, typename names>
	class Logger {
	public:
		explicit Logger(websocketpp::log::channel_type_hint::value type = websocketpp::log::channel_type_hint::access)
			: type_(type), staticChannels_(0xffffffff), dynamicChannels_(0)
		{
		}

		explicit Logger(websocketpp::log::level staticChannels, websocketpp::log::channel_type_hint::value type = websocketpp::log::channel_type_hint::access)
			: type_(type), staticChannels_(staticChannels), dynamicChannels_(0)
		{
		}

		void set_channels(websocketpp::log::level channels)
		{
			if (channels == names::none)
				clear_channels(names::all);
			else
				dynamicChannels_ |= (channels & staticChannels_);
		}

		void clear_channels(websocketpp::log::level channels)
		{
			dynamicChannels_ &= ~channels;
		}

		void write(websocketpp::log::level channel, std::string const & msg)
		{
			write(channel, msg.c_str());
		}

		void write(websocketpp::log::level channel, char const * msg)
		{
			if (dynamic_test(channel))
			{
				// Always log using LogLevel::Trace so that malicious clients can't spam your log file
				Utils::Logger::Instance().Log(LogTypes::Network, LogLevel::Trace, "websocketpp: %s", msg);
			}
		}

		_WEBSOCKETPP_CONSTEXPR_TOKEN_ bool static_test(websocketpp::log::level channel) const
		{
			return (staticChannels_ & channel) != 0;
		}

		bool dynamic_test(const websocketpp::log::level channel)
		{
			return (dynamicChannels_ & channel) != 0;
		}

	private:
		websocketpp::log::channel_type_hint::value type_;
		const websocketpp::log::level staticChannels_;
		websocketpp::log::level dynamicChannels_;
	};

	// Websocket++ config for using ASIO with the ED logger
	struct Config : public websocketpp::config::core {
		typedef Config type;
		typedef websocketpp::config::core base;

		typedef base::concurrency_type concurrency_type;

		typedef base::request_type request_type;
		typedef base::response_type response_type;

		typedef base::message_type message_type;
		typedef base::con_msg_manager_type con_msg_manager_type;
		typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

		typedef Logger<concurrency_type, websocketpp::log::alevel> alog_type;
		typedef Logger<concurrency_type, websocketpp::log::elevel> elog_type;

		typedef base::rng_type rng_type;

		struct transport_config : public base::transport_config {
			typedef type::concurrency_type concurrency_type;
			typedef type::alog_type alog_type;
			typedef type::elog_type elog_type;
			typedef type::request_type request_type;
			typedef type::response_type response_type;
			typedef websocketpp::transport::asio::basic_socket::endpoint
				socket_type;
		};

		typedef websocketpp::transport::asio::endpoint<transport_config> transport_type;
	};
}
