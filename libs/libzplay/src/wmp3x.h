/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *
 *  Copyright (C) 2003-2009 Zoran Cindori ( zcindori@inet.hr )
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * ver: 2.00
 * date: 24. April, 2010.
 *
*/


#ifndef _WMP3X_Z_
#define _WMP3X_Z_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>


#include "../include/libzplay.h"
#include "wdecoder.h"
#include "wqueue.h"
#include "weqprocessor.h"
#include "wechoprocessor.h"
#include "wstprocessor.h"
#include "wcentercutprocessor.h"
#include "woutputprocessor.h"
#include "winputprocessor.h"



using namespace libZPlay;


typedef struct {
	HANDLE hFile;
	HANDLE hFileMapping;
	LPVOID hViewOfFile;
} FILE_MAPPING;



#define MAX_ERROR_MESSAGE 128


// WMP3x CLASS  ------------------------------------------------------------------------


class  WMp3x : public ZPlay {
	public:
        WMp3x();  // constructor
        ~WMp3x(); // destructor


		int __stdcall SetSettings(TSettingID nSettingID, int nValue);
		int __stdcall GetSettings(TSettingID nSettingID);
	
		int __stdcall GetVersion();
		char * __stdcall GetError();
		wchar_t * __stdcall GetErrorW();
		TStreamFormat __stdcall GetFileFormat(const char *pchFileName);
		TStreamFormat __stdcall GetFileFormatW(const wchar_t *pchFileName);
		int __stdcall EnumerateWaveOut();

		int __stdcall EnumerateWaveIn();
		int __stdcall GetWaveInInfo(unsigned int nIndex, TWaveInInfo *pWaveInInfo);
		int __stdcall GetWaveInInfoW(unsigned int nIndex, TWaveInInfoW *pWaveInInfo);
		int __stdcall SetWaveInDevice(unsigned int nIndex);

		int __stdcall OpenFile(const char *sFileName, TStreamFormat nFormat);
		int __stdcall OpenFileW(const wchar_t *sFileName, TStreamFormat nFormat);
		int __stdcall OpenStream(int fBuffered, int fManaged, const void *sMemStream, unsigned int nStreamSize, TStreamFormat nFormat);
		int __stdcall PushDataToStream(const void *sMemNewData, unsigned int nNewDataize);
		int __stdcall IsStreamDataFree(const void *sMemNewData);
		void __stdcall GetDynamicStreamLoad(TStreamLoadInfo *pStreamLoadInfo);
		int __stdcall OpenResource(HMODULE hModule, const char *sResName, const char *sResType,  int nWaveBufferLengthMs, unsigned int nSeekFromStart, unsigned int nStreamSize);
		int __stdcall Close();
		int __stdcall Play();
		int __stdcall GetWaveOutInfo(unsigned int nIndex,TWaveOutInfo *pWaveOutInfo);
		int __stdcall GetWaveOutInfoW(unsigned int nIndex,TWaveOutInfoW *pWaveOutInfo);
		int __stdcall PlayLoop(TTimeFormat fFormatStartTime, TStreamTime* pStartTime, TTimeFormat fFormatEndTime, TStreamTime* pEndTime, unsigned int nNumOfCycles, unsigned int fContinuePlaying);
		int __stdcall SetWaveOutDevice(unsigned int nIndex);
		int __stdcall Seek(TTimeFormat fFormat,	TStreamTime *pTime, TSeekMethod nMoveMethod);
        int __stdcall Stop();
        int __stdcall Pause();
        int __stdcall Resume();
        void __stdcall GetPosition(TStreamTime *pTime);
        void __stdcall GetStreamInfo(TStreamInfo *pInfo); // return mpeg version string
		void __stdcall GetStreamInfoW(TStreamInfoW *pInfo); // return mpeg version string
        int __stdcall GetBitrate(int fAverage);
        void __stdcall GetStatus(TStreamStatus *pStatus);
		int __stdcall MixChannels(int fEnable, unsigned int nLeftPercent, unsigned int nRightPercent) ;
		void __stdcall GetVUData(unsigned int *pnLeftChannel, unsigned int *pnRightChannel);

		int __stdcall SlideVolume(
			int fFormatStart,				// flag specifying time format of starting position
			TStreamTime *pTimeStart,			// pointer to TStreamTime structure specifying start fade position
			unsigned int nStartVolumeLeft,	// starting fade volume of left channel
			unsigned int nStartVolumeRight,	// starting fade volume of right channel
			int fFormatEnd,					// flag specifying time format of end position
			TStreamTime *pTimeEnd,				// pointer to TStreamTime structure specifying end fade position
			unsigned int nEndVolumeLeft,	// end fade volume of left channel
			unsigned int nEndVolumeRight	// end fade volume of right channel
		);


        int __stdcall LoadID3(TID3Version nId3Version, TID3Info *pId3Info);

		int __stdcall LoadID3Ex(TID3InfoEx *pId3Info, unsigned int fDecodeEmbededPicture);
		int __stdcall LoadID3ExW(TID3InfoExW *pId3Info, unsigned int fDecodeEmbededPicture);

		//int __stdcall LoadID3W(TID3Version nId3Version, TID3InfoW *pId3Info);
		//int __stdcall LoadFileID3(const char *pchFileName, TStreamFormat nFormat, TID3Version nId3Version,TID3Info *pId3Info);
		//int __stdcall LoadFileID3W(const wchar_t *pchFileName, TStreamFormat nFormat, TID3Version nId3Version,TID3InfoW *pId3Info);

		//int __stdcall LoadFileID3Ex(const char *pchFileName, TStreamFormat nFormat, TID3InfoEx *pId3Info, unsigned int fDecodeEmbededPicture);

		//int __stdcall LoadFileID3ExW(const wchar_t *pchFileName, TStreamFormat nFormat, TID3InfoExW *pId3Info, unsigned int fDecodeEmbededPicture);

		int __stdcall ReverseMode(int fEnable);
		void __stdcall Release();

		int __stdcall SetCallbackFunc(TCallbackFunc pCallbackFunc, TCallbackMessage nMessages, void *user_data);


		int __stdcall SetMasterVolume(unsigned int nLeftVolume,unsigned int nRightVolume);
		int __stdcall SetPlayerVolume(unsigned int nLeftVolume,unsigned int nRightVolume);
		void __stdcall GetMasterVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume);
		void __stdcall GetPlayerVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume);

		int __stdcall EnableEcho(int fEnable);
		int __stdcall SetEchoParam(TEchoEffect *pEchoEffect, int nNumberOfEffects);
		int __stdcall GetEchoParam(TEchoEffect *pEchoEffect, int nNumberOfEffects);

		int __stdcall SetRate(unsigned int nRate);
		int __stdcall SetPitch(unsigned int nPitch);
		int __stdcall SetTempo(unsigned int nTempo);

		unsigned int __stdcall GetRate();
		unsigned int __stdcall GetPitch();
		unsigned int __stdcall GetTempo();

		int __stdcall StereoCut(int fEnable, int fOutputCenter, int fBassToSides);


int __stdcall EnableEqualizer(int fEnable);
int __stdcall SetEqualizerPoints(int *pnFreqPoint, int nNumOfPoints);
int __stdcall GetEqualizerPoints(int *pnFreqPoint, int nFreqPointNumber);
int __stdcall GetEqualizerParam(int *pnPreAmpGain, int *pnBandGain, int nNumberOfBands);
int __stdcall SetEqualizerParam(int nPreAmpGain,int *pnBandGain, int nNumberOfBands);
int __stdcall SetEqualizerPreampGain(int nGain);
int __stdcall GetEqualizerPreampGain(); 
int __stdcall SetEqualizerBandGain(int nBandIndex, int nGain);
int __stdcall GetEqualizerBandGain(int nBandIndex);

int __stdcall AddFile(const char *sFileName, TStreamFormat nFormat);
int __stdcall AddFileW(const wchar_t *sFileName, TStreamFormat nFormat);

int __stdcall SetWaveOutFile(const char *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard);
int __stdcall SetWaveOutFileW(const wchar_t *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard);


// ----------------------------------------------------------------------------------------		
    private:

	// main queue with stream data
	WQueue cMainQueue;

	FILE_MAPPING c_MainFileMapping;

// ==============================================================================================================

	// processors
	WEqProcessor c_EqualizerProcessor;
	WEchoProcessor c_EchoProcessor;
	WOutputProcessor c_OutputProcessor;
	WInputProcessor c_InputProcessor;
	WSoundTouchProcessor c_SoundTouchProcessor;
	WCenterCutProcessor c_CenterCutProcessor;

	unsigned int c_nSideCut;	// indicates side cut in center processor

// ============================================================================================================


	// indicates that all is ready to play
	int c_fReady;

	// current format
	unsigned int c_nFormat;
	// parameters from SetFormat function
	int c_param1;
	int c_param2;
	int c_param3;
	int c_param4;

	int c_fAccurateLength;
	int c_fAccurateSeek;

	// decoder class
	WAudioDecoder *c_decoder;
	WAudioDecoder *c_id3_decoder;
	// input stream info, samplerate, channel ...
	INPUT_STREAM_INFO *c_isInfo;

	INPUT_STREAM_INFO c_isInfo_backup;
	char *c_desc_backup;

	WQueue c_queue_decoder;

	unsigned int c_nSongIndex;
	unsigned int c_nOldSongIndex;
	unsigned int c_nCurrentSongIndex;

// =============================================================
//	PLAYING STATUS INFORMATIONS

		// indicate playing status
		unsigned int c_fPlay;
		// indicate pause status
		unsigned int c_fPause;

		unsigned int c_nPositionInSamples;

// =============================================================
//	INTERNAL VOLUME PROCESSING
		
		// left channel gain
		double c_dLeftGain;	
		// right channel gain
		double c_dRightGain;
		// left channel volume
		unsigned int c_nLeftVolume;
		// right channel volume
        unsigned int c_nRightVolume;
		// change volume
		static void _ChangeVolume(WMp3x *instance, unsigned char *buf, unsigned int size);

		static void _SetPlayerVolume(WMp3x *instance, unsigned int nLeftVolume, unsigned int nRightVolume);

	// =============================================================
	// WAVE OUTPUT

		// use this event to detect when wave buffer is done with playing
		HANDLE c_hEvent_WOM_DONE;
		// WAVEHDR array
		WAVEHDR *c_WH_WaveHeader;
		// number of wave buffers
		unsigned int c_nNumberOfWaveBuffers;
		// number of bytes we need for one wave buffer
		unsigned int c_nBytesForOneBuffer;	
		// number of samples we need for one wave buffer
		unsigned int c_nSamplesForOneBuffer;
		// indicate that we use wave-out
		unsigned int c_fUseWaveOut;
		// current position in samples, number of samples sent to soundcard
		unsigned int c_nCurrentPosition;
		// sum size of wave buffer in bytes
		unsigned int c_nWaveBufferSize;
		// WAVEFORMATEX structure for wave output
		WAVEFORMATEX c_wfx;
		// HWAVEOUT
		HWAVEOUT c_HWO_WaveOut;

		// current playing buffer index
		unsigned int c_nPlayingIndex;

		unsigned int c_nWaveBufferLengthMs;

// === POSITION CALCULATION ==================
		// number of input bytes
		unsigned int c_nBytesIn;
		// number of output bytes
		unsigned int c_nBytesOut;
		// real bytes in and out
		unsigned int c_nRealBytesIn;
// =============================================

		// callback function
		static void CALLBACK _WaveOutProc(HWAVEOUT hwo, UINT uMsg,DWORD dwInstance,DWORD dwParam1, DWORD dwParam2);

		static int _AllocateVUBuffers(WMp3x *instance);
		static void _FreeVUBuffers(WMp3x *instance);
		
		// working output buffer, decoder will fill this buffer with data
		// size of this buffer must be at least c_nBytesForOneBuffer
		char *c_pchWorkingWaveBuffer;
		// decoder data
		DECODER_DATA c_DecoderData;

		// pointer to current wave buffer
		char *c_pchCurrentWaveBuffer;
		// number of bytes we need in current wave buffer
		unsigned int c_nHaveBytesInWaveBuffer;
		
				
		// index of current wave buffer, use this to identify wave buffer to load
		unsigned int c_nWaveBufferIndex;
		// number of wave output devices
		// current wave out device
		unsigned int c_nCurrentWOC;	

		// number of enumerated wave out devices
		unsigned int c_nOutputDevicesNumber;
		// structure describes the capabilities of a waveform-audio output device, filled by EnumerateDevices()
		WAVEOUTCAPSA *c_pWOC;

		// number of enumerated wave out devices
		unsigned int c_nInputDevicesNumber;
		// structure describes the capabilities of a waveform-audio output device, filled by EnumerateDevices()
		WAVEINCAPSA *c_pWIC;
		// current wave in device
		unsigned int c_nCurrentWIC;	

		unsigned int c_nRealBytesHaveInWaveBuffer;


		// playing thread, decode mp3, run sound processing, send wave data to soundcard
		HANDLE w_hThread; 
		DWORD w_hThreadID; // playing thread ID
		HANDLE c_hThreadStartEvent;
		int c_thread_no_error;
		static int WINAPI _ThreadFuncPCM(void* lpdwParam);
		


		QUEUE_SAMPLES_BUFFER c_QueueSamplesBuffer;


		

// =======================================================================================
// LOOP PROCESSING
// =======================================================================================

		// indicated that we need loop playing
		unsigned int c_fLoop;
		// starting position of loop in samples
		unsigned int c_nLoopStartingPosition;
		// ending position of loop in samples
		unsigned int c_nLoopEndingPosition;
		// length of loop in samples
		unsigned int c_nLoopLength;
		// indicates if we are in flying loop, loop will continue playing after repetition
		int c_fFlyingLoop;
		// number of samples sent to soundcard in one loop
		unsigned int c_nLoopSampleCounter;
		// number of loop repetition
		unsigned int c_nLoopCycleCounter;
		// number of loop repetition
		unsigned int c_nLoopCycleNumber;
		// indicates if we are in loop
		int c_fInLoop;
		// indicates that enter loop message was send
		int c_fLoopEnterSent;
		// indicates that exit loop message was send	
		int c_fLoopExitSent;

		static int _SendLoopEnter(WMp3x *instance);
		static int _SendLoopExit(WMp3x *instance);

// =======================================================================================
// FADE VOLUME PROCESSING
// =======================================================================================
		
		// fade volume is rising ( left channel )
		unsigned int c_fFadeVolumeLeftUp;
		// fade volume is rising ( right channel )
		unsigned int c_fFadeVolumeRightUp;
		// starting position of fade  (samples)
		unsigned int c_nFadeStartingPosition;
		// length of fade  effect ( number of PCM buffers )
		unsigned int c_nFadeFrameNum;
		// fade  step for right channel
		double c_dFadeStepLeftChannel;
		// fade  step for right channel
		double c_dFadeStepRightChannel;
		// internal counter for fade processing
		unsigned int c_nFadeFrameCount;
		// check if occurs fade starting position
		unsigned int c_fCheckFade;
		// activates fade
		int c_fFade;


		// fade start volume
		unsigned int c_nFadeStartVolumeLeft;
		unsigned int c_nFadeStartVolumeRight;
		// fade end volume
		unsigned int c_nFadeEndVolumeLeft;
		unsigned int c_nFadeEndVolumeRight;
		// send fade enter callback message
		static int _SendFadeEnter(WMp3x *instance);
		// send fade exit callback message
		static int _SendFadeExit(WMp3x *instance);
		// need this to control fade processing
		CRITICAL_SECTION c_csFade;


		unsigned int c_nFadeSemaphore; // control sending fade_enter and fade_exit messages
		

// ============================================================================
//	MIX CHANNELS

	// enable channel mixing
	int c_fMixChannels;	
	// left channel mixing gain
	REAL c_nMixLeftPerc;
	// right channel mixing gain
	REAL c_nMixRightPerc;

	static void _MixChannels(WMp3x *instance, unsigned char *buf, unsigned int size);

// ===============================================================================


	static int _Open(WMp3x *instance, int nWaveBufferLengthMs);


// ==========================================================================================================
// CALLBACK MESSAGE PROCESSING
	TCallbackFunc callback_func; // pointer to callback function
	unsigned int callback_messages;	// callback mode
	HANDLE c_hThreadCallback;
	DWORD c_hThreadCallbackId;
	HANDLE c_hEventCallback;
	static int WINAPI CallbackThreadFunc(void* lpdwParam);
	static int send_stop(WMp3x *instance);
	static int send_buffer_done(WMp3x *instance, unsigned int count, unsigned int sumsize);
	static int send_stream_out_of_data(WMp3x *instance);
	static int send_callback_message(WMp3x *instance, unsigned int message, unsigned int param1, unsigned int param2);
	int c_callback_ret;
	void *c_user_callback_data;

// ==================================================================================================================
// VU meter
	short *c_pchVUDataBufferLeft;
	short *c_pchVUDataBufferRight;
	// size of VU data buffer in bytes
	unsigned int c_nVUDataBufferSize;
// ==================================================================================================================

	// stream event, use this to work with managed streams
	HANDLE c_hStreamEvent;

	// indicates managed stream
	unsigned int c_fManagedStream;

	static void _GetInternalVolume(WMp3x *instance, unsigned int *lvolume, unsigned int *rvolume);



	int c_fReverseMode;
// =========================================================================
// GET FFT VALUES
		WFFT *c_fft;	//	WFFT class used to calculate FFT
		int *c_pnSamples;	// buffer for PCM samples

		int c_nFFTPoints;
		int c_nFFTWindow;
		static void load_fft_stereo_buffer(WMp3x *instance, int *pSamples, unsigned int nNumberOfSamples);
// =========================================================================
// DISPLAY FFT SPECTRUM

	CRITICAL_SECTION c_CS_FFT;				// critical section protecting spectrum class in multiple threads
	unsigned int c_nFFTPointsForDisplay;	// number of FFT points

// =========================================================================
	// critical section, block decoding loop until seek is finished
	CRITICAL_SECTION c_CS_DecodingLoop;
	// this CS protects all waveOut... functions in playing thread
	CRITICAL_SECTION c_CS_waveOut;


	// clear chain in decoder loop
	unsigned int c_fClearChain;

	// enables decoding loop
	BOOL c_fRunDecodingLoop;
	// enables playing loop
	BOOL c_fRunPlayingLoop;

	// playing loop
	static int _PlayingLoop(WMp3x *instance, unsigned int fFlush);


	float in[12000];
	float out[12000];



// ==========================================================================
//	ERROR CODES AND ERROR MESSAGES

	wchar_t c_error_messageW[MAX_ERROR_MESSAGE + 1];
	char c_error_messageA[MAX_ERROR_MESSAGE + 1];


	void err(DECODER_ERROR_MESSAGE *error);
	void err(unsigned int error_code);

	char c_WaveOutProductName[32];
	wchar_t c_WaveOutProductNameW[32];


	char c_WaveInProductName[32];
	wchar_t c_WaveInProductNameW[32];

	char *c_StreamDescription;
	wchar_t *c_StreamDescriptionW;
	
	TID3Picture c_id3_picture;
	TID3Picture c_id3_picture_file;

	WAudioDecoder *_GetAudioDecoder(TStreamFormat nFormat);

	int c_nPCMSampleRate;
	int c_nPCMChannelNumber;
	int c_nPCMBitPerSample;
	int c_nPCMBigEndian;

	int  _SetFormat(TStreamFormat nFormat);

	void _FormatID3(wchar_t **info, TID3InfoExW *pId3Info, int fDecodeEmbededPicture);

	unsigned int c_fEndOfStream; // end-of-stream indicator for dynamic stream



	char *c_user_mixer_line;
	unsigned int c_user_mixer_line_id;
	unsigned int c_user_mixer_volume;

	unsigned int c_nWaveInBufferSize;  // maximal size(latency) of wave in buffer in milliseconds


// ========================================
// SAVE TO


	TStreamFormat c_save_format;
	wchar_t *c_save_filename;

	int _CheckEncoderFormat(TStreamFormat format);


	static unsigned int  read_callback(void *data, unsigned int data_size, void *user_data);
	static unsigned int  write_callback(void *data, unsigned int data_size, void *user_data);
	static unsigned int  seek_callback(unsigned int offset, void *user_data);
	static unsigned int  tell_callback(void *user_data);


// ============================================

};



#endif

