#ifndef PUBLIC_SDK_DEFINITIONS_H
#define PUBLIC_SDK_DEFINITIONS_H

#include "teamspeak/public_definitions.h"

enum VirtualServerPropertiesSDK {
	VIRTUALSERVER_FILEBASE=24,                                     //not available to clients, stores the folder used for file transfers
	VIRTUALSERVER_MAX_DOWNLOAD_TOTAL_BANDWIDTH = 29,               //only available on request (=> requestServerVariables)
	VIRTUALSERVER_MAX_UPLOAD_TOTAL_BANDWIDTH= 30,                  //only available on request (=> requestServerVariables)
	VIRTUALSERVER_LOG_FILETRANSFER=64,
};

enum ConnectionPropertiesSDK {
	CONNECTION_FILETRANSFER_BANDWIDTH_SENT =62,                     //how many bytes per second are currently being sent by file transfers
	CONNECTION_FILETRANSFER_BANDWIDTH_RECEIVED =63,                 //how many bytes per second are currently being received by file transfers
	CONNECTION_FILETRANSFER_BYTES_RECEIVED_TOTAL =64,               //how many bytes we received in total through file transfers
	CONNECTION_FILETRANSFER_BYTES_SENT_TOTAL =65,                   //how many bytes we sent in total through file transfers
};

#endif //PUBLIC_SDK_DEFINITIONS_H
