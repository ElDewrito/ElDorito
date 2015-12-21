/*
* TeamSpeak 3 server sample
*
* Copyright (c) 2007-2014 TeamSpeak-Systems
* https://halowiki.llf.to/ts3_sdk/server_html/index.html
*/

#ifdef _WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#else
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#endif
#include <stdio.h>

#include "../Console/GameConsole.hpp"
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_errors.h>
#include <teamspeak/clientlib_publicdefinitions.h>
#include <teamspeak/clientlib.h>
#include <teamspeak/serverlib_publicdefinitions.h>
#include <teamspeak/serverlib.h>


#ifdef _WIN32
#define snprintf sprintf_s
#define SLEEP(x) Sleep(x)
#else
#define SLEEP(x) usleep(x*1000)
#endif
bool VoIPServerRunning = false;
bool IsVoIPServerRunning(){
	return VoIPServerRunning;
}
/*
* Callback when client has connected.
*
* Parameter:
*   serverID  - Virtual server ID
*   clientID  - ID of connected client
*   channelID - ID of channel the client joined
*/

void onClientConnected(uint64 serverID, anyID clientID, uint64 channelID, unsigned int* removeClientError) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("Client " + std::to_string(clientID) + " joined channel " + std::to_string((unsigned long long)channelID) + " on Eldewrito VoIP Server " + std::to_string((unsigned long long)serverID));
#endif

	//Note: we can prevent clients from connecting by changing the removeClientError
	//This would be very useful if a client is in some kind of ban list...
	//See https://halowiki.llf.to/ts3_sdk/server_html/ar01s16.html#s_faq_2 for more info
}

/*
* Callback when client has disconnected.
*
* Parameter:
*   serverID  - Virtual server ID
*   clientID  - ID of disconnected client
*   channelID - ID of channel the client left
*/
void onClientDisconnected(uint64 serverID, anyID clientID, uint64 channelID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("Client " + std::to_string(clientID) + " left channel " + std::to_string((unsigned long long)channelID) + " on Eldewrito VoIP Server " + std::to_string((unsigned long long)serverID));
#endif
}

/*
* Callback when client has moved.
*
* Parameter:
*   serverID     - Virtual server ID
*   clientID     - ID of moved client
*   oldChannelID - ID of old channel the client left
*   newChannelID - ID of new channel the client joined
*/
void onClientMoved(uint64 serverID, anyID clientID, uint64 oldChannelID, uint64 newChannelID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("Client " + std::to_string(clientID) + " moved from channel " + std::to_string((unsigned long long)oldChannelID) + " to channel " + std::to_string((unsigned long long)newChannelID) + " on Eldewrito VoIP Server " + std::to_string((unsigned long long)serverID));
#endif
}

/*
* Callback when channel has been created.
*
* Parameter:
*   serverID        - Virtual server ID
*   invokerClientID - ID of the client who created the channel
*   channelID       - ID of the created channel
*/
void onChannelCreated(uint64 serverID, anyID invokerClientID, uint64 channelID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("Channel " + std::to_string((unsigned long long)channelID) + " created by " + std::to_string(invokerClientID) + "on Eldewrito VoIP Server " + std::to_string((unsigned long long)serverID));
#endif
}

/*
* Callback when channel has been edited.
*
* Parameter:
*   serverID        - Virtual server ID
*   invokerClientID - ID of the client who edited the channel
*   channelID       - ID of the edited channel
*/
void onChannelEdited(uint64 serverID, anyID invokerClientID, uint64 channelID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("Channel " + std::to_string((unsigned long long)channelID) + " edited by " + std::to_string(invokerClientID) + " on Eldewrito VoIP Server " + std::to_string((unsigned long long)serverID));
#endif
}

/*
* Callback when channel has been deleted.
*
* Parameter:
*   serverID        - Virtual server ID
*   invokerClientID - ID of the client who deleted the channel
*   channelID       - ID of the deleted channel
*/
void onChannelDeleted(uint64 serverID, anyID invokerClientID, uint64 channelID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("Channel " + std::to_string((unsigned long long)channelID) + " deleted by " + std::to_string(invokerClientID) + " on Eldewrito VoIP Server " + std::to_string((unsigned long long)serverID));
#endif
}

/*
* Callback for user-defined logging.
*
* Parameter:
*   logMessage        - Log message text
*   logLevel          - Severity of log message
*   logChannel        - Custom text to categorize the message channel
*   logID             - Virtual server ID giving the virtual server source of the log event
*   logTime           - String with the date and time the log entry occured
*   completeLogString - Verbose log message including all previous parameters for convinience
*/
void onUserLoggingMessageEventServer(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
	/* Your custom error display here... */
	/* printf("LOG: %s\n", completeLogString); */
	if (logLevel == LogLevel_CRITICAL) {
		auto& console = GameConsole::Instance();
		console.consoleQueue.pushLineFromGameToUI("Eldewrito VoIP had a critical error." + std::string(completeLogString));
		exit(1);  /* Your custom handling of critical errors */
	}
}

/*
* Callback triggered when the specified client starts talking.
*
* Parameters:
*   serverID - ID of the virtual server sending the callback
*   clientID - ID of the client which started talking
*/
void onClientStartTalkingEvent(uint64 serverID, anyID clientID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("onClientStartTalkingEvent serverID=" + std::to_string((unsigned long long)serverID) + ", clientID = " + std::to_string(clientID));
#endif
}

/*
* Callback triggered when the specified client stops talking.
*
* Parameters:
*   serverID - ID of the virtual server sending the callback
*   clientID - ID of the client which stopped talking
*/
void onClientStopTalkingEvent(uint64 serverID, anyID clientID) {
#ifdef _DEBUG
	auto& console = GameConsole::Instance();
	console.consoleQueue.pushLineFromGameToUI("onClientStopTalkingEvent serverID=" + std::to_string((unsigned long long)serverID) + ", clientID = " + std::to_string(clientID));
#endif
}

/*
* Callback triggered when a license error occurs.
*
* Parameters:
*   serverID  - ID of the virtual server on which the license error occured. This virtual server will be automatically
*               shutdown. If the parameter is zero, all virtual servers are affected and have been shutdown.
*   errorCode - Code of the occured error. Use ts3server_getGlobalErrorMessage() to convert to a message string
*/
void onAccountingErrorEvent(uint64 serverID, unsigned int errorCode) {
	auto& console = GameConsole::Instance();
	char* errorMessage;
	if (ts3server_getGlobalErrorMessage(errorCode, &errorMessage) == ERROR_ok) {
		printf("onAccountingErrorEvent serverID=%llu, errorCode=%u: %s\n", (unsigned long long)serverID, errorCode, errorMessage);
		ts3server_freeMemory(errorMessage);
	}

	/* Your custom handling here. In a real application, you wouldn't stop the whole process because the TS3 server part went down.
	* The whole idea of this callback is to let you gracefully handle the TS3 server failing to start and to continue your application. */
	exit(1);
}

/*
* Read server key from file
*/
int readKeyPairFromFile(const char *fileName, char *keyPair) {
	FILE *file;

	file = fopen(fileName, "r");
	if (file == NULL) {
		printf("Could not open file '%s' for reading keypair\n", fileName);
		return -1;
	}

	fgets(keyPair, BUFSIZ, file);
	if (ferror(file) != 0) {
		fclose(file);
		printf("Error reading keypair from file '%s'.\n", fileName);
		return -1;
	}
	fclose(file);

	printf("Read keypair '%s' from file '%s'.\n", keyPair, fileName);
	return 0;
}

/*
* Write server key to file
*/
int writeKeyPairToFile(const char *fileName, const char* keyPair) {
	FILE *file;

	file = fopen(fileName, "w");
	if (file == NULL) {
		printf("Could not open file '%s' for writing keypair\n", fileName);
		return -1;
	}

	fputs(keyPair, file);
	if (ferror(file) != 0) {
		fclose(file);
		printf("Error writing keypair to file '%s'.\n", fileName);
		return -1;
	}
	fclose(file);

	printf("Wrote keypair '%s' to file '%s'.\n", keyPair, fileName);
	return 0;
}
/*
* Kick a client given their username
*/
int kickTeamspeakClient(const std::string& name) {
	anyID* listClientIDs;
	char* workingClientName;

	auto& console = GameConsole::Instance();

	if (ts3server_getClientList(1, &listClientIDs) != ERROR_ok) {
		console.consoleQueue.pushLineFromGameToUI("VoIP: Error getting list of clients (-1)");
		return -1;
	}
	for (int a = 0; listClientIDs[a] != NULL; a++) {
		//The server ID here defaults to 1. We should have no reason to change it, but if we do, change this function and the requestKick further down
		if (ts3server_getClientVariableAsString(1, listClientIDs[a], CLIENT_NICKNAME, &workingClientName) != ERROR_ok) {
			console.consoleQueue.pushLineFromGameToUI("VoIP: Error getting client name (-2)");
			ts3server_freeMemory(listClientIDs);
			return -2;
		}
		bool nameEvaluationBool = name == workingClientName;
		ts3server_freeMemory(workingClientName);
		if (nameEvaluationBool) {
			if (ts3client_requestClientKickFromServer(1, listClientIDs[a], "Kicked from server", NULL) != ERROR_ok) {
				console.consoleQueue.pushLineFromGameToUI("VoIP: Error kicking client (-3)");
				ts3server_freeMemory(listClientIDs);
				return -3;
			}
			ts3server_freeMemory(listClientIDs);
			return 0;
		}
	}
	
	console.consoleQueue.pushLineFromGameToUI("VoIP: Client not found (-4)");
	ts3server_freeMemory(listClientIDs);
	return -4;
}


DWORD WINAPI StartTeamspeakServer(LPVOID) {
	char *version;
	uint64 serverID;
	unsigned int error;
	char buffer[BUFSIZ] = { 0 };
	char filename[BUFSIZ];
	char port_str[20];
	char *keyPair;

	/* Create struct for callback function pointers */
	struct ServerLibFunctions funcs;
	auto& console = GameConsole::Instance();

	console.consoleQueue.pushLineFromGameToUI("Starting Eldewrito VoIP Server on port 9987...");

	/* Initialize all callbacks with NULL */
	memset(&funcs, 0, sizeof(struct ServerLibFunctions));

	/* Now assign the used callback function pointers */
	funcs.onClientConnected = onClientConnected;
	funcs.onClientDisconnected = onClientDisconnected;
	funcs.onClientMoved = onClientMoved;
	funcs.onChannelCreated = onChannelCreated;
	funcs.onChannelEdited = onChannelEdited;
	funcs.onChannelDeleted = onChannelDeleted;
	funcs.onUserLoggingMessageEvent = onUserLoggingMessageEventServer;
	funcs.onClientStartTalkingEvent = onClientStartTalkingEvent;
	funcs.onClientStopTalkingEvent = onClientStopTalkingEvent;
	funcs.onAccountingErrorEvent = onAccountingErrorEvent;

	/* Initialize server lib with callbacks */
	if ((error = ts3server_initServerLib(&funcs, LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING, NULL)) != ERROR_ok) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			console.consoleQueue.pushLineFromGameToUI("Error initialzing Eldewrito VoIP Server serverlib : " + std::string(errormsg));
			ts3server_freeMemory(errormsg);
		}
		return 1;
	}

	/* Query and print server lib version */
	if ((error = ts3server_getServerLibVersion(&version)) != ERROR_ok) {
		printf("Error querying Eldewrito VoIP Server lib version: %d\n", error);
		return 1;
	}
	printf("Server lib version: %s\n", version);
	ts3server_freeMemory(version);  /* Release dynamically allocated memory */

	/* Attempt to load keypair from file */
	/* Assemble filename: keypair_<port>.txt */
	strcpy(filename, "keypair_");
	sprintf(port_str, "%d", 9987);  // Default port
	strcat(filename, port_str);
	strcat(filename, ".txt");

	/* Try reading keyPair from file */
	if (readKeyPairFromFile(filename, buffer) == 0) {
		keyPair = buffer;  /* Id read from file */
	}
	else {
		keyPair = "";  /* No Id saved, start virtual server with empty keyPair string */
	}

	/* Create virtual server using default port 9987 with max 10 slots */

	/* Create the virtual server with specified port, name, keyPair and max clients */
	console.consoleQueue.pushLineFromGameToUI("Create Eldewrito VoIP using keypair " + std::string(keyPair));
	if ((error = ts3server_createVirtualServer(9987, "0.0.0.0", "Eldewrito VoIP Server", keyPair, 16, &serverID)) != ERROR_ok) {
		char* errormsg;
		if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
			console.consoleQueue.pushLineFromGameToUI("Error creating Eldewrito VoIP Server: " + std::string(errormsg) + "(" + std::to_string(error) + ")");
			ts3server_freeMemory(errormsg);
		}
		return 1;
	}

	/* If we didn't load the keyPair before, query it from virtual server and save to file */
	if (!*buffer) {
		if ((error = ts3server_getVirtualServerKeyPair(serverID, &keyPair)) != ERROR_ok) {
			char* errormsg;
			if (ts3server_getGlobalErrorMessage(error, &errormsg) == ERROR_ok) {
				console.consoleQueue.pushLineFromGameToUI("Error creating Eldewrito VoIP Server: "+ std::string(errormsg));
				ts3server_freeMemory(errormsg);
			}
			return 0;
		}

		/* Save keyPair to file "keypair_<port>.txt"*/
		if (writeKeyPairToFile(filename, keyPair) != 0) {
			ts3server_freeMemory(keyPair);
			return 0;
		}
		ts3server_freeMemory(keyPair);
	}

	/* Set welcome message */
	if ((error = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_WELCOMEMESSAGE, "Eldorito VoIP")) != ERROR_ok) {
		console.consoleQueue.pushLineFromGameToUI("Error setting Eldewrito VoIP Server welcomemessage: "+ error);
		return 1;
	}

	/* Set server password */
	/* TODO: Make the password the server host uid? */
	if ((error = ts3server_setVirtualServerVariableAsString(serverID, VIRTUALSERVER_PASSWORD, "secret")) != ERROR_ok) {
		console.consoleQueue.pushLineFromGameToUI("Error setting Eldewrito VoIP Server password: " + error);
		return 1;
	}

	/* Flush above two changes to server */
	if ((error = ts3server_flushVirtualServerVariable(serverID)) != ERROR_ok) {
		console.consoleQueue.pushLineFromGameToUI("Error flushing Eldewrito VoIP Server variables: " + error);
		return 1;
	}
	VoIPServerRunning = true;
	while (VoIPServerRunning){
		SLEEP(200);
	}

	/* Stop virtual server */
	if ((error = ts3server_stopVirtualServer(serverID)) != ERROR_ok) {
		console.consoleQueue.pushLineFromGameToUI("Error stopping virtual server: "+ error);
		return 1;
	}

	/* Shutdown server lib */
	if ((error = ts3server_destroyServerLib()) != ERROR_ok) {
		console.consoleQueue.pushLineFromGameToUI("Error destroying server lib: "+ error);
		return 1;
	}
	console.consoleQueue.pushLineFromGameToUI("Stopped Eldewrito VoIP Server");
	return 0;
}

void StopTeamspeakServer(){
	VoIPServerRunning = false;
	return;
}
