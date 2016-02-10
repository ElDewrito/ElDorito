#include <stdio.h>
#include <stdlib.h>

#include <libwebsockets.h>
#include "../Console/GameConsole.hpp"
#include "../ElModules.hpp"
#include "websock-w32.h"
#include "gettimeofday.h"
#include "WebSockets.hpp"

static int callback_http(struct libwebsocket_context *context,
struct libwebsocket *wsi,
enum libwebsocket_callback_reasons reason, void *user,
	void *in, size_t len)
{
	return 0;
}

static int callback_dew_rcon(struct libwebsocket_context *context,
struct libwebsocket *wsi,
enum libwebsocket_callback_reasons reason,
	void *user, void *in, size_t len)
{
	//auto& console = GameConsole::Instance();
	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
		
		//console.consoleQueue.pushLineFromGameToUI("Websocket Connection Established!");
		break;
	case LWS_CALLBACK_RECEIVE: { 
		//console.consoleQueue.pushLineFromGameToUI("received data:" + std::string((char *)in));
		
		std::string send = Modules::CommandMap::Instance().ExecuteCommand(std::string((char *)in), true);

		//console.consoleQueue.pushLineFromGameToUI("Got this shit:" + send);

		unsigned char *sendChar = (unsigned char*)malloc(LWS_SEND_BUFFER_PRE_PADDING + send.length() + LWS_SEND_BUFFER_POST_PADDING);

		for (size_t i = 0; i < send.length(); i++) {
			sendChar[LWS_SEND_BUFFER_PRE_PADDING + i] = send.c_str()[i];
		}

		libwebsocket_write(wsi, &sendChar[LWS_SEND_BUFFER_PRE_PADDING], send.length(), LWS_WRITE_TEXT);

		// release memory back into the wild
		free(sendChar);
		break;
	}
	default:
		break;
	}

	return 0;
}



static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",   // name
		callback_http, // callback
		0              // per_session_data_size
	},
	{
		"dew-rcon", // protocol name - very important!
		callback_dew_rcon,   // callback
		0                          // we don't use any per session data
	},
	{
		NULL, NULL, 0   /* End of list */
	}
};

DWORD WINAPI StartRconWebSocketServer(LPVOID) {
	//auto& console = GameConsole::Instance();

	// server url will be http://localhost:11776 by default
	int port = Modules::ModuleGame::Instance().VarRconPort->ValueInt;
	struct libwebsocket_context *context;
	// create libwebsocket context representing this server
	int opts = 0;
	char interface_name[128] = "";
	const char * interfacez = NULL;

	//TODO: implement https/ssl later
	const char *cert_path = "/libwebsockets-test-server.pem";
	const char *key_path = "/libwebsockets-test-server.key.pem";

	//if (!use_ssl)
	cert_path = key_path = NULL;

	context = libwebsocket_create_context(port, interfacez, protocols,
		libwebsocket_internal_extensions,
		cert_path, key_path, NULL, -1, -1, opts);

	if (context == NULL) {
		//console.consoleQueue.pushLineFromGameToUI("libwebsocket init failed\n");
		return -1;
	}


	// infinite loop, to end this server send SIGTERM. (CTRL+C)
	while (1) {
		libwebsocket_service(context, 50);
		// libwebsocket_service will process all waiting events with their
		// callback functions and then wait 50 ms.
		// (this is a single threaded webserver and this will keep our server
		// from generating load while there are not requests to process)
	}

	libwebsocket_context_destroy(context);

	return 0;
}