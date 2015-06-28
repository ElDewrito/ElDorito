/*
 * This is the main header for the TeamSpeak 3 Client SDK. All the functions that are used
 * to communicate with the Teamspeak 3 Client are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3sclient_freeMemory()
 * after finishing with the result. This holds true for all the functions returning strings
 * (so the parameter is "char** result"). Again, the details are all in the documentation.
 */

#ifndef CLIENTLIB_H
#define CLIENTLIB_H

//system
#include <stdlib.h>

//own
#include "teamspeak/clientlib_publicdefinitions.h"
#include "teamspeak/public_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ClientUIFunctionsRare;

struct ClientUIFunctions {
	void (*onConnectStatusChangeEvent)              (uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
	void (*onServerProtocolVersionEvent)            (uint64 serverConnectionHandlerID, int protocolVersion);
	void (*onNewChannelEvent)                       (uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID);
	void (*onNewChannelCreatedEvent)                (uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onDelChannelEvent)                       (uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onChannelMoveEvent)                      (uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onUpdateChannelEvent)                    (uint64 serverConnectionHandlerID, uint64 channelID);
	void (*onUpdateChannelEditedEvent)              (uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onUpdateClientEvent)                     (uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onClientMoveEvent)                       (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage);
	void (*onClientMoveSubscriptionEvent)           (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);
	void (*onClientMoveTimeoutEvent)                (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage);
	void (*onClientMoveMovedEvent)                  (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage);
	void (*onClientKickFromChannelEvent)            (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);
	void (*onClientKickFromServerEvent)             (uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);
	void (*onClientIDsEvent)                        (uint64 serverConnectionHandlerID, const char* uniqueClientIdentifier, anyID clientID, const char* clientName);
	void (*onClientIDsFinishedEvent)                (uint64 serverConnectionHandlerID);
	void (*onServerEditedEvent)                     (uint64 serverConnectionHandlerID, anyID editerID, const char* editerName, const char* editerUniqueIdentifier);
	void (*onServerUpdatedEvent)                    (uint64 serverConnectionHandlerID);
	void (*onServerErrorEvent)                      (uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
	void (*onServerStopEvent)                       (uint64 serverConnectionHandlerID, const char* shutdownMessage);
	void (*onTextMessageEvent)                      (uint64 serverConnectionHandlerID, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message);
	void (*onTalkStatusChangeEvent)                 (uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
	void (*onIgnoredWhisperEvent)                   (uint64 serverConnectionHandlerID, anyID clientID);
	void (*onConnectionInfoEvent)                   (uint64 serverConnectionHandlerID, anyID clientID);
	void (*onServerConnectionInfoEvent)             (uint64 serverConnectionHandlerID);
	void (*onChannelSubscribeEvent)                 (uint64 serverConnectionHandlerID, uint64 channelID);
	void (*onChannelSubscribeFinishedEvent)         (uint64 serverConnectionHandlerID);
	void (*onChannelUnsubscribeEvent)               (uint64 serverConnectionHandlerID, uint64 channelID);
	void (*onChannelUnsubscribeFinishedEvent)       (uint64 serverConnectionHandlerID);
	void (*onChannelDescriptionUpdateEvent)         (uint64 serverConnectionHandlerID, uint64 channelID);
	void (*onChannelPasswordChangedEvent)           (uint64 serverConnectionHandlerID, uint64 channelID);
	void (*onPlaybackShutdownCompleteEvent)         (uint64 serverConnectionHandlerID);
	void (*onSoundDeviceListChangedEvent)           (const char* modeID, int playOrCap);
	void (*onEditPlaybackVoiceDataEvent)            (uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);
	void (*onEditPostProcessVoiceDataEvent)         (uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
	void (*onEditMixedPlaybackVoiceDataEvent)       (uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
	void (*onEditCapturedVoiceDataEvent)            (uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited);
	void (*onCustom3dRolloffCalculationClientEvent) (uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume);
	void (*onCustom3dRolloffCalculationWaveEvent)   (uint64 serverConnectionHandlerID, uint64 waveHandle, float distance, float* volume);
	void (*onUserLoggingMessageEvent)               (const char* logmessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
	void (*onCustomPacketEncryptEvent)              (char** dataToSend, unsigned int* sizeOfData);
	void (*onCustomPacketDecryptEvent)              (char** dataReceived, unsigned int* dataReceivedSize);
	void (*onProvisioningSlotRequestResultEvent)    (unsigned int error, uint64 requestHandle, const char* connectionKey);
	void (*onCheckServerUniqueIdentifierEvent)      (uint64 serverConnectionHandlerID, const char* ServerUniqueIdentifier, int* cancelConnect);
	void (*onClientPasswordEncrypt)                 (uint64 serverConnectionHandlerID, const char* plaintext, char* encryptedText, int encryptedTextByteSize);
	void (*onFileTransferStatusEvent)               (anyID transferID, unsigned int status, const char* statusMessage, uint64 remotefileSize, uint64 serverConnectionHandlerID);
	void (*onFileListEvent)                        (uint64 serverConnectionHandlerID, uint64 channelID, const char* path, const char* name, uint64 size, uint64 datetime, int type, uint64 incompletesize, const char* returnCode);
	void (*onFileListFinishedEvent)                (uint64 serverConnectionHandlerID, uint64 channelID, const char* path);
	void (*onFileInfoEvent)                        (uint64 serverConnectionHandlerID, uint64 channelID, const char* name, uint64 size, uint64 datetime);
}; //END OF ClientUIFunctions

/*Memory management*/
EXPORTDLL unsigned int ts3client_freeMemory(void* pointer);

/*Construction and Destruction*/
EXPORTDLL unsigned int ts3client_initClientLib(const struct ClientUIFunctions* functionPointers, const struct ClientUIFunctionsRare* functionRarePointers, int usedLogTypes, const char* logFileFolder, const char* resourcesFolder);
EXPORTDLL unsigned int ts3client_destroyClientLib();
EXPORTDLL unsigned int ts3client_getClientLibVersion(char** result);
EXPORTDLL unsigned int ts3client_getClientLibVersionNumber(uint64* result);

EXPORTDLL unsigned int ts3client_spawnNewServerConnectionHandler(int port, uint64* result);
EXPORTDLL unsigned int ts3client_destroyServerConnectionHandler(uint64 serverConnectionHandlerID);

/*Identity management*/
EXPORTDLL unsigned int ts3client_createIdentity(char** result);
EXPORTDLL unsigned int ts3client_identityStringToUniqueIdentifier(const char* identityString, char** result);

/*sound*/
EXPORTDLL unsigned int ts3client_getPlaybackDeviceList(const char* modeID, char**** result);
EXPORTDLL unsigned int ts3client_getCaptureDeviceList(const char* modeID, char**** result);
EXPORTDLL unsigned int ts3client_getPlaybackModeList(char*** result);
EXPORTDLL unsigned int ts3client_getCaptureModeList(char*** result);
EXPORTDLL unsigned int ts3client_getDefaultPlaybackDevice(const char* modeID, char*** result);
EXPORTDLL unsigned int ts3client_getDefaultCaptureDevice(const char* modeID, char*** result);
EXPORTDLL unsigned int ts3client_getDefaultPlayBackMode(char** result);
EXPORTDLL unsigned int ts3client_getDefaultCaptureMode(char** result);

EXPORTDLL unsigned int ts3client_openPlaybackDevice(uint64 serverConnectionHandlerID, const char* modeID, const char* playbackDevice);
EXPORTDLL unsigned int ts3client_openCaptureDevice(uint64 serverConnectionHandlerID, const char* modeID, const char* captureDevice);
EXPORTDLL unsigned int ts3client_getCurrentPlaybackDeviceName(uint64 serverConnectionHandlerID, char** result, int* isDefault);
EXPORTDLL unsigned int ts3client_getCurrentPlayBackMode(uint64 serverConnectionHandlerID, char** result);
EXPORTDLL unsigned int ts3client_getCurrentCaptureDeviceName(uint64 serverConnectionHandlerID, char** result, int* isDefault);
EXPORTDLL unsigned int ts3client_getCurrentCaptureMode(uint64 serverConnectionHandlerID, char** result);
EXPORTDLL unsigned int ts3client_initiateGracefulPlaybackShutdown(uint64 serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_closePlaybackDevice(uint64 serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_closeCaptureDevice(uint64 serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_activateCaptureDevice(uint64 serverConnectionHandlerID);

EXPORTDLL unsigned int ts3client_playWaveFile(uint64 serverConnectionHandlerID, const char* path);
EXPORTDLL unsigned int ts3client_playWaveFileHandle(uint64 serverConnectionHandlerID, const char* path, int loop, uint64* waveHandle);
EXPORTDLL unsigned int ts3client_pauseWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle, int pause);
EXPORTDLL unsigned int ts3client_closeWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle);

EXPORTDLL unsigned int ts3client_registerCustomDevice(const char* deviceID, const char* deviceDisplayName, int capFrequency, int capChannels, int playFrequency, int playChannels);
EXPORTDLL unsigned int ts3client_unregisterCustomDevice(const char* deviceID);
EXPORTDLL unsigned int ts3client_processCustomCaptureData(const char* deviceName, const short* buffer, int samples);
EXPORTDLL unsigned int ts3client_acquireCustomPlaybackData(const char* deviceName, short* buffer, int samples);

EXPORTDLL unsigned int ts3client_setLocalTestMode(uint64 serverConnectionHandlerID, int status);

EXPORTDLL unsigned int ts3client_startVoiceRecording(uint64 serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_stopVoiceRecording(uint64 serverConnectionHandlerID);

EXPORTDLL unsigned int ts3client_allowWhispersFrom(uint64 serverConnectionHandlerID, anyID clID);
EXPORTDLL unsigned int ts3client_removeFromAllowedWhispersFrom(uint64 serverConnectionHandlerID, anyID clID);

/* 3d sound positioning */
EXPORTDLL unsigned int ts3client_systemset3DListenerAttributes(uint64 serverConnectionHandlerID, const TS3_VECTOR* position, const TS3_VECTOR* forward, const TS3_VECTOR* up);
EXPORTDLL unsigned int ts3client_set3DWaveAttributes(uint64 serverConnectionHandlerID, uint64 waveHandle, const TS3_VECTOR* position);
EXPORTDLL unsigned int ts3client_systemset3DSettings(uint64 serverConnectionHandlerID, float distanceFactor, float rolloffScale);
EXPORTDLL unsigned int ts3client_channelset3DAttributes(uint64 serverConnectionHandlerID, anyID clientID, const TS3_VECTOR* position);

/*preprocessor*/
EXPORTDLL unsigned int ts3client_getPreProcessorInfoValueFloat(uint64 serverConnectionHandlerID, const char* ident, float* result);
EXPORTDLL unsigned int ts3client_getPreProcessorConfigValue(uint64 serverConnectionHandlerID, const char* ident, char** result);
EXPORTDLL unsigned int ts3client_setPreProcessorConfigValue(uint64 serverConnectionHandlerID, const char* ident, const char* value);

/*encoder*/
EXPORTDLL unsigned int ts3client_getEncodeConfigValue(uint64 serverConnectionHandlerID, const char* ident, char** result);

/*playback*/
EXPORTDLL unsigned int ts3client_getPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const char* ident, float* result);
EXPORTDLL unsigned int ts3client_setPlaybackConfigValue(uint64 serverConnectionHandlerID, const char* ident, const char* value);
EXPORTDLL unsigned int ts3client_setClientVolumeModifier(uint64 serverConnectionHandlerID, anyID clientID, float value);

/*logging*/
EXPORTDLL unsigned int ts3client_logMessage(const char* logMessage, enum LogLevel severity, const char* channel, uint64 logID);
EXPORTDLL unsigned int ts3client_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/
EXPORTDLL unsigned int ts3client_getErrorMessage(unsigned int errorCode, char** error);

/*Interacting with the server*/
EXPORTDLL unsigned int ts3client_startConnection(uint64 serverConnectionHandlerID, const char* identity, const char* ip, unsigned int port, const char* nickname,
                                                 const char** defaultChannelArray, const char* defaultChannelPassword, const char* serverPassword);
EXPORTDLL unsigned int ts3client_stopConnection(uint64 serverConnectionHandlerID, const char* quitMessage);
EXPORTDLL unsigned int ts3client_requestClientMove(uint64 serverConnectionHandlerID, anyID clientID, uint64 newChannelID, const char* password, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientVariables(uint64 serverConnectionHandlerID, anyID clientID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientKickFromChannel(uint64 serverConnectionHandlerID, anyID clientID, const char* kickReason, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientKickFromServer(uint64 serverConnectionHandlerID, anyID clientID, const char* kickReason, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelDelete(uint64 serverConnectionHandlerID, uint64 channelID, int force, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelMove(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, uint64 newChannelOrder, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestSendPrivateTextMsg(uint64 serverConnectionHandlerID, const char* message, anyID targetClientID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestSendChannelTextMsg(uint64 serverConnectionHandlerID, const char* message, uint64 targetChannelID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestSendServerTextMsg(uint64 serverConnectionHandlerID, const char* message, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestConnectionInfo(uint64 serverConnectionHandlerID, anyID clientID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientSetWhisperList(uint64 serverConnectionHandlerID, anyID clientID, const uint64* targetChannelIDArray, const anyID* targetClientIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelSubscribe(uint64 serverConnectionHandlerID, const uint64* channelIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelSubscribeAll(uint64 serverConnectionHandlerID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelUnsubscribe(uint64 serverConnectionHandlerID, const uint64* channelIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelUnsubscribeAll(uint64 serverConnectionHandlerID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelDescription(uint64 serverConnectionHandlerID, uint64 channelID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestMuteClients(uint64 serverConnectionHandlerID, const anyID* clientIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestUnmuteClients(uint64 serverConnectionHandlerID, const anyID* clientIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientIDs(uint64 serverConnectionHandlerID, const char* clientUniqueIdentifier, const char* returnCode);

/*provisioning server calls*/
EXPORTDLL unsigned int ts3client_requestSlotsFromProvisioningServer(const char* ip, unsigned short port, const char* serverPassword, unsigned short slots, const char* identity, const char* region, uint64* requestHandle);
EXPORTDLL unsigned int ts3client_cancelRequestSlotsFromProvisioningServer(uint64 requestHandle);
EXPORTDLL unsigned int ts3client_startConnectionWithProvisioningKey(uint64 serverConnectionHandlerID, const char* identity, const char* nickname, const char* connectionKey, const char* clientMetaData);

/*retrieve information ClientLib has stored*/

/*general info*/
EXPORTDLL unsigned int ts3client_getClientID(uint64 serverConnectionHandlerID, anyID* result);

/*client connection info*/
EXPORTDLL unsigned int ts3client_getConnectionStatus(uint64 serverConnectionHandlerID, int* result);
EXPORTDLL unsigned int ts3client_getConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getConnectionVariableAsDouble(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, double* result);
EXPORTDLL unsigned int ts3client_getConnectionVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char** result);
EXPORTDLL unsigned int ts3client_cleanUpConnectionInfo(uint64 serverConnectionHandlerID, anyID clientID);

/*server connection info*/
EXPORTDLL unsigned int ts3client_requestServerConnectionInfo(uint64 serverConnectionHandlerID, const char* returnCode);
EXPORTDLL unsigned int ts3client_getServerConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getServerConnectionVariableAsFloat(uint64 serverConnectionHandlerID, size_t flag, float* result);

/*client info*/
EXPORTDLL unsigned int ts3client_getClientSelfVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, char** result);
EXPORTDLL unsigned int ts3client_setClientSelfVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int value);
EXPORTDLL unsigned int ts3client_setClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, const char* value);
EXPORTDLL unsigned int ts3client_flushClientSelfUpdates(uint64 serverConnectionHandlerID, const char* returnCode);

EXPORTDLL unsigned int ts3client_getClientVariableAsInt(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getClientVariableAsUInt64(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getClientVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char** result);

EXPORTDLL unsigned int ts3client_getClientList(uint64 serverConnectionHandlerID, anyID** result);
EXPORTDLL unsigned int ts3client_getChannelOfClient(uint64 serverConnectionHandlerID, anyID clientID, uint64* result);

/*channel info*/
EXPORTDLL unsigned int ts3client_getChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, char** result);

EXPORTDLL unsigned int ts3client_getChannelIDFromChannelNames(uint64 serverConnectionHandlerID, char** channelNameArray, uint64* result);
EXPORTDLL unsigned int ts3client_setChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, int value);
EXPORTDLL unsigned int ts3client_setChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, uint64 value);
EXPORTDLL unsigned int ts3client_setChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, const char* value);
EXPORTDLL unsigned int ts3client_flushChannelUpdates(uint64 serverConnectionHandlerID, uint64 channelID, const char* returnCode);
EXPORTDLL unsigned int ts3client_flushChannelCreation(uint64 serverConnectionHandlerID, uint64 channelParentID, const char* returnCode);

EXPORTDLL unsigned int ts3client_getChannelList(uint64 serverConnectionHandlerID, uint64** result);
EXPORTDLL unsigned int ts3client_getChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID, anyID** result);
EXPORTDLL unsigned int ts3client_getParentChannelOfChannel(uint64 serverConnectionHandlerID, uint64 channelID, uint64* result);
EXPORTDLL unsigned int ts3client_getChannelEmptySecs(uint64 serverConnectionHandlerID, uint64 channelID, int* result);

/*server info*/
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerList(uint64** result);
EXPORTDLL unsigned int ts3client_getServerVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getServerVariableAsUInt64(uint64 serverConnectionHandlerID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getServerVariableAsString(uint64 serverConnectionHandlerID, size_t flag, char** result);
EXPORTDLL unsigned int ts3client_requestServerVariables(uint64 serverConnectionHandlerID);

/*filetransfer management*/
EXPORTDLL unsigned int ts3client_getTransferFileName(anyID transferID, char** result); //The returned memory is dynamically allocated, remember to call ts3client_freeMemory() to release it
EXPORTDLL unsigned int ts3client_getTransferFilePath(anyID transferID, char** result); //The returned memory is dynamically allocated, remember to call ts3client_freeMemory() to release it
EXPORTDLL unsigned int ts3client_getTransferFileRemotePath(anyID transferID, char** result); //The returned memory is dynamically allocated, remember to call ts3client_freeMemory() to release it
EXPORTDLL unsigned int ts3client_getTransferFileSize(anyID transferID, uint64* result);
EXPORTDLL unsigned int ts3client_getTransferFileSizeDone(anyID transferID, uint64* result);
EXPORTDLL unsigned int ts3client_isTransferSender(anyID transferID, int* result); //1 == upload, 0 == download
EXPORTDLL unsigned int ts3client_getTransferStatus(anyID transferID, int* result);
EXPORTDLL unsigned int ts3client_getCurrentTransferSpeed(anyID transferID, float* result); //bytes/second within the last few seconds
EXPORTDLL unsigned int ts3client_getAverageTransferSpeed(anyID transferID, float* result); //bytes/second since start of the transfer
EXPORTDLL unsigned int ts3client_getTransferRunTime(anyID transferID, uint64* result);

/*Interacting with the server - file transfers*/
EXPORTDLL unsigned int ts3client_sendFile(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* file, int overwrite, int resume, const char* sourceDirectory, anyID* result, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestFile(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* file, int overwrite, int resume, const char* destinationDirectory, anyID* result, const char* returnCode);
EXPORTDLL unsigned int ts3client_haltTransfer(uint64 serverConnectionHandlerID, anyID transferID, int deleteUnfinishedFile, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestFileList(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* path, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestFileInfo(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* file, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestDeleteFile(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char** file, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestCreateDirectory(uint64 serverConnectionHandlerID, uint64 channelID, const char* channelPW, const char* directoryPath, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestRenameFile(uint64 serverConnectionHandlerID, uint64 fromChannelID, const char* fromChannelPW, uint64 toChannelID, const char* toChannelPW, const char* oldFile, const char* newFile, const char* returnCode);
EXPORTDLL unsigned int ts3client_getInstanceSpeedLimitUp(uint64 *limit);
EXPORTDLL unsigned int ts3client_getInstanceSpeedLimitDown(uint64 *limit);
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerSpeedLimitUp(uint64 serverConnectionHandlerID, uint64*  limit);
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerSpeedLimitDown(uint64 serverConnectionHandlerID, uint64* limit);
EXPORTDLL unsigned int ts3client_getTransferSpeedLimit(anyID transferID, uint64* limit);
EXPORTDLL unsigned int ts3client_setInstanceSpeedLimitUp(uint64 newLimit);
EXPORTDLL unsigned int ts3client_setInstanceSpeedLimitDown(uint64 newLimit);
EXPORTDLL unsigned int ts3client_setServerConnectionHandlerSpeedLimitUp(uint64 serverConnectionHandlerID, uint64 newLimit);
EXPORTDLL unsigned int ts3client_setServerConnectionHandlerSpeedLimitDown(uint64 serverConnectionHandlerID, uint64 newLimit);
EXPORTDLL unsigned int ts3client_setTransferSpeedLimit(anyID transferID, uint64 newLimit);

#ifdef __cplusplus
}
#endif

#endif
