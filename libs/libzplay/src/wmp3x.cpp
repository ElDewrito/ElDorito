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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


#include "debug.h"
#include "wmp3x.h"
#include "wtags.h"

#include "config.h"

extern WAVEIN_ID g_wavein_id[WAVE_IN_NUM];


#ifdef _MSC_VER
#pragma comment( lib, "winmm")
#pragma comment( lib, "user32")
#endif

enum {
	WMP3X_NO_ERROR = 0,
	WMP3X_MEMORY_ALLOCATION_FAIL,
	WMP3X_NO_DECODER_SELECTED,
	WMP3X_FILE_OPEN_ERROR,
	WMP3X_STREAM_IS_CLOSED,
	WMP3X_THREAD_CREATION_FAIL,
	WMP3X_INVALID_FUNCTION_PARAMETERS,
	WMP3X_WAVEOUT_ERROR,
	WMP3X_NOT_SUPPORTED_ON_MANAGED_STREAM,
	WMP3X_NOT_SUPPORTED_ONLY_ON_MANAGED_STREAM,
	WMP3X_NO_WAVE_DEVICE_PRESENT,
	WMP3X_DECODER_CREATION_FAIL,
	WMP3X_EVENT_CREATION_FAIL,
	WMP3X_UNKNOWN_FILE_FORMAT,
	WMP3X_STREAM_IS_NOT_SEEKABLE,
	WMP3X_FAIL_TO_DETECT_BPM,
	WMP3X_FAIL_TO_INITIALIZE_DECODER,
	WMP3X_SAMPLERATE_NOT_MATCHING,
	WMP3X_DECODER_INITIALIZATION_FAIL,
	WMP3X_UNSUPPORTED_FORMAT_PF_VERSION,
	WMP3X_UNSUPPORTED_IMAGE_FORMAT,

	WMP3X_UNSUPPORTED_ON_WAVEIN,
	WMP3X_UNSUPPORTED_SAVE_FORMAT,

	WMP3X_CANT_OPEN_SAVE_FILE,


	WMP3X_ENCODER_CREATION_FAIL,
	WMP3X_UNSUPPORTED_ENCODER,

	WMP3X_NO_ID3_DATA,
	WMP3X_UNKNOWN_ERROR
};




wchar_t *g_error_strW[WMP3X_UNKNOWN_ERROR + 1] = {
	L"No error.",
	L"Memory allocation fail.",
	L"No decoder selected.",
	L"File open error.",
	L"Stream is closed.",
	L"Thread creation fail.",
	L"Invalid function parameters.",
	L"Wave output error.",
	L"This function is not supported on managed stream.",
	L"This function is supported only on managed stream.",
	L"No wave out device present.",
	L"Decoder creation fail.",
	L"Event creation fail.",
	L"Unknown file format.",
	L"Stream is not seekable.",
	L"Fail to detect beat value.",
	L"Fail to initialize decoder.",
	L"Sample rate is not matching.",
	L"Decoder initialization fail.",
	L"Unsupported format in PF (patent free) version.",
	L"Unsupported image format",

	L"Unsupported on sfWaveIn format",
	L"Unsupported encoder format.",
	L"Can't open save file.",


	L"Encoder creation fail.",
	L"Unsupported encoder.",
	L"Can't find ID3 tag",
	L"Unknown error."

};





// play section return values
#define PLAY_FATAL_ERROR 2
#define PLAY_USER_STOP 1
#define PLAY_OK 0



typedef struct {
	FILE_MAPPING fm;
	WQueue *queue;
	WAudioDecoder *decoder;
	TStreamFormat format;
} AUDIO_DECODER;


void _InitFileMapping(FILE_MAPPING *pfm, WQueue *queue, int fBuffered);
void _CloseFileMapping(FILE_MAPPING *pfm, WQueue *queue);
int _OpenFileMapping(wchar_t *sFileName, FILE_MAPPING *pfm, WQueue *queue);


void ConvertID3UTF16ToANSI(TID3InfoExW *info);

unsigned int Mp3TimeToSamples(int fFormat, TStreamTime *pTime, unsigned int nSampleRate);

// average sine size in 16 bit audio data, need to calculate VU data
#define AVG_SINE_16 23169.0 
// single audio buffer size in ms
#define SINGLE_AUDIO_BUFFER_SIZE 50  
// size of VU data buffer in ms
#define VU_BUFFER_SIZE 50


#define FFT_POINTS_NUM 9
int FFT_Points_Table[FFT_POINTS_NUM] = {32, 64, 128, 256, 512, 1024, 2048, 8192, 16384};



int allocate_fft(int points, REAL **samplesl, REAL **samplesr, REAL **window, int **ip, REAL **w);
void free_fft(REAL **samplesl, REAL **samplesr, REAL **window, int **ip, REAL **w);


typedef struct {
	unsigned int nIndex;
	unsigned int nStart;
	unsigned int nEnd;
	unsigned int nSongIndex;
} WAVE_USER_DATA;



int  EncoderCallback(void *data, unsigned int data_size, void *user_data);


WMp3x::WMp3x()
{
	// init error messages
	err(WMP3X_NO_ERROR);

	// fade semaphore
	c_nFadeSemaphore = 0;

	c_fUseWaveOut = 1;



	c_save_filename = NULL;
	c_save_format = sfUnknown;

	c_user_mixer_line = NULL;
	c_user_mixer_line_id = WAVE_IN_NUM;
	c_user_mixer_volume = 200;	// set to larger than 100, this value will not change current volume
	c_nWaveInBufferSize = 1000;

	_InitFileMapping(&c_MainFileMapping, &cMainQueue, 0); 
	

	memset(&c_isInfo_backup, 0, sizeof(INPUT_STREAM_INFO));
	c_desc_backup = NULL;

	// =========================
	// default PCM settings, change this with SetSettings

	c_nPCMSampleRate = 44100;
	c_nPCMChannelNumber = 2;
	c_nPCMBitPerSample = 16;
	c_nPCMBigEndian = 0;
	// ==========================

	

	c_nWaveBufferLengthMs = 500;
	c_fAccurateLength = 0;
	c_fAccurateSeek = 0;
	
	c_nSongIndex = 0;
	c_nOldSongIndex = 0;
	c_nCurrentSongIndex = 0;

	c_id3_decoder = NULL;

	c_queue_decoder.SetMemoryMode(1);


	c_StreamDescription = NULL;
	c_StreamDescriptionW = NULL;


	memset(&c_id3_picture, 0, sizeof(TID3Picture));
	memset(&c_id3_picture_file, 0, sizeof(TID3Picture));

// ==================================================
//	DISPLAY FFT SPECTRUM GRAPH
	InitializeCriticalSection(&c_CS_FFT);
	c_nFFTPointsForDisplay = 512;
// ===================================================
//	GET FFT VALUES
	c_fft = 0;
	c_pnSamples = 0;

// ===================================================

	err(WMP3X_NO_ERROR);

// ==========================================================
// protect main "decoding and playing" loop


	InitializeCriticalSection(&c_CS_DecodingLoop);
	InitializeCriticalSection(&c_CS_waveOut);

	c_fPlay = 0;
	c_fPause = 0;


	c_InputProcessor.SetOutputProcessor(&c_SoundTouchProcessor);
	c_SoundTouchProcessor.SetOutputProcessor(&c_EqualizerProcessor);
	c_EqualizerProcessor.SetOutputProcessor(&c_CenterCutProcessor);
	c_CenterCutProcessor.SetOutputProcessor(&c_EchoProcessor);
	c_EchoProcessor.SetOutputProcessor(&c_OutputProcessor);

	c_nSideCut = 0;


// ==========================================================
//	NOT READY TO PLAY
	c_fReady = 0;
// ==========================================================
//	GET DECODER CLASS
	c_decoder = 0;
	c_isInfo = 0;

// =============================================================
//	INTERNAL VOLUME PROCESSING
	c_dLeftGain = 1.0;	
	c_dRightGain = 1.0;
	c_nLeftVolume = 100;
    c_nRightVolume = 100;

// ==========================================================
//	INITIALIZE FADE PROCESSING VALUES
	c_fCheckFade = 0;
	c_fFade = 0;
	InitializeCriticalSection(&c_csFade);
// ===========================================================================
// INITIALIZE LOOP PROCESSING
	c_fLoop = 0;
	c_fInLoop = 0;
	c_nLoopCycleCounter = 0;
	c_nLoopCycleNumber = 0;

// ==========================================================
// WAVE OUT DEVICES
	c_nCurrentWOC = WaveOutWaveMapper;
	c_nCurrentWIC = WaveInWaveMapper;


	c_nWaveBufferSize = 0;
	c_pchWorkingWaveBuffer = 0;
	c_hEvent_WOM_DONE = 0;
	c_WH_WaveHeader = 0;

	c_HWO_WaveOut = 0;

	c_nNumberOfWaveBuffers = 0;
	

	c_nOutputDevicesNumber = 0;
	c_pWOC = 0;

	c_nInputDevicesNumber = 0;
	c_pWIC = 0;



	c_nPlayingIndex = 0;
	c_nPositionInSamples = 0;


// ==========================================================
// FFT
	c_nFFTPoints = 0;
	c_nFFTWindow = 0;
// ==========================================================
// VU BUFFERS
	c_pchVUDataBufferLeft = 0;
	c_pchVUDataBufferRight = 0;
	c_nVUDataBufferSize = 0;

// =================================================
// INITIALIZE CALLBACK PREOCESSING
	c_hThreadCallback = 0;
	c_hThreadCallbackId = 0;
	c_hEventCallback = 0;
	callback_func = 0;
	callback_messages = 0;
	c_hEventCallback = CreateEvent(NULL, TRUE, FALSE, NULL);
// =================================================
// MIX CHANNELS
	c_fMixChannels = 0;
// =================================================
// REVERSE MODE	
	c_fReverseMode = 0;
// =================================================
// MAIN PLAYING THREAD
    w_hThread = 0; // playing thread handle
	c_thread_no_error = 1;
	w_hThreadID = 0;


// stream event
	c_hStreamEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	c_fManagedStream = 0;


	c_hThreadStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

// create MP3 decoder as default decoder for new created class instance

	c_decoder = CreateMp3Decoder();
	c_decoder->Initialize(0, 0, 0, 0);
	c_nFormat = sfMp3;
	c_param1 = 0;
	c_param2 = 0;
	c_param3 = 0;
	c_param4 = 0;

	
	c_nCurrentPosition = 0;
}


WMp3x::~WMp3x()
{


// =======================================================

 	Stop(); // stop playing

// =========================================

	Close();

// =======================================
//	FADE VOLUME
	DeleteCriticalSection(&c_csFade);
	
// ==================================================
// EXIT CALLBACK THREAD
	SetCallbackFunc(0, (TCallbackMessage)0, 0);	// destroy cllback thread
	CloseHandle(c_hEventCallback);
// ==================================================


// close stream event
	CloseHandle(c_hStreamEvent);

// destructor

// ===========================================
// FREE VU DATA
	if(c_pchVUDataBufferLeft)
		free(c_pchVUDataBufferLeft);

	if(c_pchVUDataBufferRight)
		free(c_pchVUDataBufferRight);

	c_pchVUDataBufferLeft = 0;
	c_pchVUDataBufferRight = 0;
	c_nVUDataBufferSize = 0;


// ============================================
//	FREE DECODER
	if(c_decoder)
		c_decoder->Release();

	if(c_id3_decoder)
	{
		c_id3_decoder->Close();
		c_id3_decoder->Release();
	}

	c_id3_decoder = NULL;
// ============================================
//	FREE OUTPUT
//	ASSERT_W(c_output);
//	delete c_output;	
// ============================================

// ==========================================================

	if(c_pWOC)
		free(c_pWOC);

	if(c_pWIC)
		free(c_pWIC);


// =============================================
// FFT DISPLAY CLEANUP
	DeleteCriticalSection(&c_CS_FFT);
// =============================================
//	GET FFT VALUES CLEANUP
	if(c_fft)
		delete c_fft;

	if(c_pnSamples)
		free(c_pnSamples);
// =============================================

	if(c_StreamDescription != NULL)
		free(c_StreamDescription);

	if(c_StreamDescriptionW != NULL)
		free(c_StreamDescriptionW);


	if(c_user_mixer_line)
	{
		free(c_user_mixer_line);
		c_user_mixer_line = NULL;
	}

	c_user_mixer_line_id = WAVE_IN_NUM;

	DeleteCriticalSection(&c_CS_DecodingLoop);
	DeleteCriticalSection(&c_CS_waveOut);

}



void _InitFileMapping(FILE_MAPPING *pfm, WQueue *queue, int fBuffered)
{
	pfm->hViewOfFile = 0;
	pfm->hFileMapping = 0;
	pfm->hFile = INVALID_HANDLE_VALUE;
	queue->SetMemoryMode(fBuffered);
}


void _CloseFileMapping(FILE_MAPPING *pfm, WQueue *queue)
{
	if(pfm->hViewOfFile)
	{
		UnmapViewOfFile(pfm->hViewOfFile);
		pfm->hViewOfFile = 0;
	}

	if(pfm->hFileMapping)
	{
		CloseHandle(pfm->hFileMapping);
		pfm->hFileMapping = 0;
	}

	if(pfm->hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pfm->hFile);
		pfm->hFile = INVALID_HANDLE_VALUE;
	}

	queue->Clear();
}


int _OpenFileMapping(wchar_t *sFileName, FILE_MAPPING *pfm, WQueue *queue)
{
	// open file for file mapping	
	pfm->hFile = CreateFileW(sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);	
	if(pfm->hFile ==  INVALID_HANDLE_VALUE)
        return 0;	
	

	// get file size
	unsigned int nStreamSize = GetFileSize(pfm->hFile, NULL);
	if(nStreamSize == 0xFFFFFFFF || nStreamSize == 0)
	{
		CloseHandle(pfm->hFile);
		pfm->hFile = INVALID_HANDLE_VALUE;
        return 0;
	}


	// create file mapping
	pfm->hFileMapping = CreateFileMappingW(pfm->hFile,NULL,PAGE_READONLY,0,0,NULL);
	if(pfm->hFileMapping == NULL)
	{
		CloseHandle(pfm->hFile);
		pfm->hFile = INVALID_HANDLE_VALUE;
        return 0;
	}
	
	// create file view
	pfm->hViewOfFile = MapViewOfFile(pfm->hFileMapping,FILE_MAP_READ,0,0,0);
	if(pfm->hViewOfFile == NULL)
	{
		CloseHandle(pfm->hFileMapping);
		pfm->hFileMapping = NULL;
		CloseHandle(pfm->hFile);
		pfm->hFile = INVALID_HANDLE_VALUE;
        return 0;
	}


	// set main queue
	if(queue->PushLast(pfm->hViewOfFile, nStreamSize) == 0)
	{
		UnmapViewOfFile(pfm->hViewOfFile);
		pfm->hViewOfFile = 0;
		CloseHandle(pfm->hFileMapping);
		pfm->hFileMapping = NULL;
		CloseHandle(pfm->hFile);
		pfm->hFile = INVALID_HANDLE_VALUE;
        return 0;
	}

	return 1;
}

int __stdcall WMp3x::OpenFile(const char *sFileName, TStreamFormat nFormat)
{
	err(WMP3X_NO_ERROR);

	if(sFileName == NULL)
	{
		Close();
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	wchar_t *filename = ANSIToUTF16((char*)sFileName, -1);
	if(filename == NULL)
	{
		// close previous stream
		Close();
		err(WMP3X_FILE_OPEN_ERROR);
		return 0;
	}

	int ret = OpenFileW(filename, nFormat);
	free(filename);
	return ret;
		
}


int __stdcall WMp3x::OpenFileW(const wchar_t *sFileName, TStreamFormat nFormat)
{
	// close previous stream
	Close();

	if(sFileName == NULL)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}



	err(WMP3X_NO_ERROR);

	c_fManagedStream = 0;

	// detect stream format
	if(nFormat == sfAutodetect || nFormat == sfUnknown || nFormat == sfWaveIn)
	{
		TStreamFormat old = nFormat;
		nFormat = GetFileFormatW(sFileName);
		if(old == sfWaveIn)
			nFormat = old;
		if(nFormat == sfUnknown)
			return 0; // unknown format, error message set by GetFileFormat function

	}

	// set decoder if not already set
	if(nFormat != c_nFormat || nFormat == sfWaveIn)
	{
		if(_SetFormat(nFormat) == 0)
		{
			err(WMP3X_DECODER_CREATION_FAIL);
			return 0;
		}
	}

	if(c_decoder == 0)
	{
		err(WMP3X_NO_DECODER_SELECTED);
		return 0;
	}


	if(nFormat != sfWaveIn)
	{
		if(_OpenFileMapping((wchar_t*) sFileName, &c_MainFileMapping, &cMainQueue) == 0)
		{
			err(WMP3X_FILE_OPEN_ERROR);
			return 0;
		}
	}


	// open decoder
	if(c_decoder->OpenStream(&cMainQueue, 0, 0, 0) == 0)
	{
		_CloseFileMapping(&c_MainFileMapping, &cMainQueue);
		err(c_decoder->GetError());
		return 0;
	}


	// decoder is ready now, prepare output
	if(_Open(this, c_nWaveBufferLengthMs) == 0)
	{
		c_decoder->Close();
		_CloseFileMapping(&c_MainFileMapping, &cMainQueue);
		return 0;

	}

	return 1;			
}



int __stdcall WMp3x::Stop()
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
		return 1;
	
// ========================================================================
// WE NEED TO EXIT PLAYING AND DECODING LOOP AND THREAD

	// block decoding thread
	EnterCriticalSection(&c_CS_DecodingLoop);
	SetEvent(c_hStreamEvent);
	// disable decoder loop
	c_fRunDecodingLoop = 0;
	// disable playing loop
	c_fRunPlayingLoop = 0;
	// protect access to wave out functions
	EnterCriticalSection(&c_CS_waveOut);
	// stop playing
	if(c_HWO_WaveOut)
		waveOutReset(c_HWO_WaveOut);

	// call this to be sure that playing loop can end
	SetEvent(c_hEvent_WOM_DONE);

	LeaveCriticalSection(&c_CS_waveOut);
	LeaveCriticalSection(&c_CS_DecodingLoop);

// ALL THIS ABOVE WILL KILL PLAYING AND DECODING LOOP, AND THIS WILL KILL THREAD
// ========================================================================
// NOW WAIT THREAD TO END

	// be sure that playing thread is ended
	WaitForSingleObject(w_hThread, INFINITE);
	if(w_hThread)
		CloseHandle(w_hThread);

	w_hThread = 0;

// ===========================
// INITIALIZE VARIABLES USED BY OTHER FUNCTIONS

// MAIN INDICATOR OF PLAYING
	c_fPlay = 0;


	c_fPause = 0;
	c_fLoop = 0;
	c_fCheckFade = 0;
	c_fFade = 0;
	c_nPlayingIndex = 0;

// ============================
// SEEK DECODER TO 0 POSITION
	c_decoder->Seek(0);
	// set seek position
	c_nCurrentPosition = 0;
// ============================

    return 1;

}



int __stdcall WMp3x::Play()
{	
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

// ====== CHECK IF WE NEED TO RESUME PLAYING ==============

	if(c_fPause)
		return Resume();

// ===== IF WE ARE ALREADY PLAYING RETURN 1 ===============

	if(c_fPlay)
    	return 1;

// ====== INITIALIZE VARIABLES USED BY OTHER FUNCTIONS ( GetPosition )
	c_fLoop = 0;


// ======= CLOSE HANDLE OF PREVIOUS THREAD =========

	if(w_hThread)
		CloseHandle(w_hThread);

	w_hThread = 0;

// ==== ENABLE DECODING AND PLAYING LOOP  ==========

	c_fRunDecodingLoop = 1;
	c_fRunPlayingLoop = 1;

// ==== CREATE PLAYING THREAD =========

	c_fClearChain = 0;

	c_thread_no_error = 1;
	w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFuncPCM, (void*) this,0,&w_hThreadID);

	if( w_hThread == NULL)
	{
    	err(WMP3X_THREAD_CREATION_FAIL);
        return 0;
	}
	
	// wait for thread to create and get possible error 
	WaitForSingleObject(c_hThreadStartEvent, INFINITE);
	if(c_thread_no_error == 0)	// we have some error in thread
		return 0;


// ===== WE ARE OK, THREAD IS CREATED =====

	// position calculation
	c_nBytesIn = 0;
	// number of output bytes
	c_nBytesOut = 0;
	// real bytes in and out
	c_nRealBytesIn = 0;

	c_nPlayingIndex = 0;
	c_fPause = 0;
	c_fPlay = 1;

    return 1;
}

void __stdcall WMp3x::GetStreamInfo(TStreamInfo *pInfo)
{
	err(WMP3X_NO_ERROR);

	if(pInfo == 0)
		return;

	memset(pInfo, 0, sizeof(TStreamInfo));
	pInfo->Description = "";

	if(c_StreamDescription != NULL)
		free(c_StreamDescription);

	c_StreamDescription = NULL;

	if(c_fReady == 0 || c_isInfo == NULL)
		return;

	
	// make duplicate of string to preserve safe pointer when stream is closed
	char *tmp = strdup(c_isInfo->pchStreamDescription);
	if(tmp == NULL)
		return;

	pInfo->Description = tmp;
	c_StreamDescription = tmp;

	pInfo->SamplingRate = c_isInfo->nSampleRate;
	pInfo->ChannelNumber = c_isInfo->nChannel;
	pInfo->VBR = c_isInfo->fVbr;
	pInfo->Bitrate = c_isInfo->nFileBitrate / 1000; 


	pInfo->Length.ms = (unsigned int) ((double) c_isInfo->nLength / (double) c_isInfo->nSampleRate * 1000.0);
	pInfo->Length.sec = (unsigned int) ((double) c_isInfo->nLength / (double) c_isInfo->nSampleRate );
	pInfo->Length.samples = c_isInfo->nLength;

	unsigned int length = pInfo->Length.ms;
	pInfo->Length.hms.hour = length / 3600000;
	length = length % 3600000;
	pInfo->Length.hms.minute= length / 60000;
	length = length % 60000;
	pInfo->Length.hms.second = length / 1000;
	pInfo->Length.hms.millisecond = length % 1000;

}



void __stdcall WMp3x::GetStreamInfoW(TStreamInfoW *pInfo)
{
	err(WMP3X_NO_ERROR);

	if(pInfo == 0)
		return;

	memset(pInfo, 0, sizeof(TStreamInfoW));
	pInfo->Description = L"";

	if(c_StreamDescriptionW != NULL)
		free(c_StreamDescriptionW);

	c_StreamDescriptionW = NULL;

	if(c_fReady == 0 || c_isInfo == NULL)
		return;


	pInfo->SamplingRate = c_isInfo->nSampleRate;
	pInfo->ChannelNumber = c_isInfo->nChannel;
	pInfo->VBR = c_isInfo->fVbr;
	pInfo->Bitrate = c_isInfo->nFileBitrate / 1000; 


	unsigned int size = strlen(c_isInfo->pchStreamDescription) + 1;
	wchar_t *tmp = (wchar_t*) malloc(size * sizeof(wchar_t));
	if(tmp == NULL)
		return;

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, c_isInfo->pchStreamDescription, -1, tmp, size);
	
	pInfo->Description = tmp;	
	c_StreamDescriptionW = tmp; 


	pInfo->Length.ms = (unsigned int) ((double) c_isInfo->nLength / (double) c_isInfo->nSampleRate * 1000.0);
	pInfo->Length.sec = (unsigned int) ((double) c_isInfo->nLength / (double) c_isInfo->nSampleRate );
	pInfo->Length.samples = c_isInfo->nLength;

	unsigned int length = pInfo->Length.ms;
	pInfo->Length.hms.hour = length / 3600000;
	length = length % 3600000;
	pInfo->Length.hms.minute= length / 60000;
	length = length % 60000;
	pInfo->Length.hms.second = length / 1000;
	pInfo->Length.hms.millisecond = length % 1000;
}


int __stdcall WMp3x::Pause()
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

	if(c_HWO_WaveOut == 0)
	{
		err(WMP3X_WAVEOUT_ERROR);
		return 0;
	}


    if(waveOutPause(c_HWO_WaveOut) != MMSYSERR_NOERROR)
	{
    	err(WMP3X_WAVEOUT_ERROR);
        return 0;
    }

    c_fPause = 1;
    return 1;
}

int __stdcall WMp3x::Resume()
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

	if(c_HWO_WaveOut == 0)
	{
		err(WMP3X_WAVEOUT_ERROR);
		return 0;
	}


    if(waveOutRestart(c_HWO_WaveOut) != MMSYSERR_NOERROR)
	{
    	err(WMP3X_WAVEOUT_ERROR);
        return 0;
    }

    c_fPause = 0;

    return 1;
}




int __stdcall WMp3x::Seek(TTimeFormat fFormat, TStreamTime *pTime, TSeekMethod nMoveMethod)
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

	if(c_fManagedStream)
	{
		err(WMP3X_NOT_SUPPORTED_ON_MANAGED_STREAM);
		return 0;
	}

	if(pTime == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(fFormat == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}


	
	// calculate seek time in samples
	unsigned int seek_time = 0;
	switch(fFormat)
	{
		case tfMillisecond:
			seek_time = (unsigned int) MulDiv(pTime->ms, c_isInfo->nSampleRate, 1000);
		break;

		case tfSecond:
			seek_time = pTime->sec * c_isInfo->nSampleRate;
		break;

		case tfHMS:
			seek_time = pTime->hms.millisecond;
			seek_time += pTime->hms.second * 1000;
			seek_time += pTime->hms.minute * 60000;
			seek_time += pTime->hms.hour * 3600000;
			seek_time = (unsigned int) MulDiv(seek_time, c_isInfo->nSampleRate, 1000);
		break;

		case tfSamples:
			seek_time = pTime->samples;
		break;
	}


	unsigned int new_position = 0;

	switch(nMoveMethod)
	{
		case smFromBeginning:
			new_position = seek_time;
		break;
		
		case smFromEnd:
			if( seek_time > c_isInfo->nLength)
				new_position = 0;
			else
				new_position = c_isInfo->nLength - seek_time - 1;	
		break;
		
		case smFromCurrentForward:
		{	
			TStreamTime cTime;
			GetPosition(&cTime);
			new_position = seek_time + cTime.samples;
		}
		break;
		
		case smFromCurrentBackward:
		{
			TStreamTime cTime;
			GetPosition(&cTime);
			if(seek_time > cTime.samples)
				new_position = 0;
			else
				new_position = cTime.samples - seek_time;		
		}
		break;	
	}


	// ========================================================================
// WE NEED TO EXIT PLAYING AND DECODING LOOP

	// block decoding thread
	EnterCriticalSection(&c_CS_DecodingLoop);
	SetEvent(c_hStreamEvent);
	// disable playing loop
	c_fRunPlayingLoop = 0;
	// protect access to wave out functions
	EnterCriticalSection(&c_CS_waveOut);


	// stop playing
	if(c_HWO_WaveOut)
		waveOutReset(c_HWO_WaveOut);

	// call this to be sure that playing loop can end
	SetEvent(c_hEvent_WOM_DONE);



	c_nBytesOut = 0;
	c_nBytesIn = 0;
	c_nRealBytesIn = 0;
	// ==============================
	c_fLoop = 0;
	c_fCheckFade = 0;
	c_fFade = 0;

	// set index to first wave buffer, we will start with 0 wave buffer
	c_nWaveBufferIndex = 0;
	// set current wave buffer
	c_pchCurrentWaveBuffer = c_WH_WaveHeader[c_nWaveBufferIndex].lpData;
	// set number of bytes we need to fill current wave buffer
	c_nHaveBytesInWaveBuffer = 0;
	// number of real bytes in wave buffer
	c_nRealBytesHaveInWaveBuffer = 0;
	// clear audio processor chain
	c_fClearChain = 1;

	c_nPlayingIndex = 0;


	if(c_decoder->Seek(new_position) == 0)
	{
		err(c_decoder->GetError());
		LeaveCriticalSection(&c_CS_waveOut);
		LeaveCriticalSection(&c_CS_DecodingLoop);
		return 0;
	}

	c_nCurrentPosition = new_position;


	LeaveCriticalSection(&c_CS_waveOut);
	LeaveCriticalSection(&c_CS_DecodingLoop);


	

	

	

// ========================================================================

	return 1;
}

void __stdcall WMp3x::GetStatus(TStreamStatus *pStatus)
{
	err(WMP3X_NO_ERROR);
	if(pStatus == 0)
		return;

	memset(pStatus, 0, sizeof(TStreamStatus));

	if(c_fReady == 0)
		return;

	
	pStatus->nSongIndex = c_nCurrentSongIndex;
	pStatus->nSongsInQueue = c_queue_decoder.GetCount();

	if(c_fPlay)
		pStatus->fPlay = 1;


	if(c_fPause)
	{
    	pStatus->fPause = 1;
		pStatus->fPlay = 0;
	}
	
	pStatus->fEcho = c_EchoProcessor.IsEnabled();
	pStatus->fEqualizer = c_EqualizerProcessor.IsEnabled(); 

	if(c_CenterCutProcessor.IsEnabled())
	{
		if(c_nSideCut)
		{
			pStatus->fVocalCut = 0;
			pStatus->fSideCut = 1;
		}
		else
		{
			pStatus->fVocalCut = 1;
			pStatus->fSideCut = 0;
		}
	}
	else
	{
		pStatus->fVocalCut = 0;
		pStatus->fSideCut = 0;
	}

	
	pStatus->fChannelMix = c_fMixChannels;
	pStatus->fSlideVolume = c_fFade;
	pStatus->nLoop = c_nLoopCycleNumber  - c_nLoopCycleCounter;
	pStatus->fReverse = c_fReverseMode;
}







int __stdcall WMp3x::GetBitrate(int fAverage)
{ 
	if(c_fReady == 0 || c_fPlay == 0)
		return 0;

	return c_decoder->GetBitrate(fAverage) / 1000;	
}





int __stdcall WMp3x::Close()
{
	err(WMP3X_NO_ERROR);

	if(c_desc_backup)
	{
		free(c_desc_backup);
		c_desc_backup = NULL;
	}

	if(c_id3_picture.MIMEType)
		free(c_id3_picture.MIMEType);

	if(c_id3_picture.Description)
		free(c_id3_picture.Description);

	if(c_id3_picture.PictureData)
		free(c_id3_picture.PictureData);

	if(c_id3_picture.hBitmap)
		DeleteObject(c_id3_picture.hBitmap);

	memset(&c_id3_picture, 0, sizeof(TID3Picture));



	if(c_id3_picture_file.MIMEType)
		free(c_id3_picture_file.MIMEType);

	if(c_id3_picture_file.Description)
		free(c_id3_picture_file.Description);

	if(c_id3_picture_file.PictureData)
		free(c_id3_picture_file.PictureData);

	if(c_id3_picture_file.hBitmap)
		DeleteObject(c_id3_picture_file.hBitmap);

	memset(&c_id3_picture_file, 0, sizeof(TID3Picture));


	if(c_fReady)
	{


		EnterCriticalSection(&c_CS_FFT);
	// ==================================
	// stop playing
		Stop();

	// ==================================
	// close thread handle
		if(w_hThread)
			CloseHandle(w_hThread);

		w_hThread = 0;

	// ===============================================
	// FREE VU DATA
		if(c_pchVUDataBufferLeft)
			free(c_pchVUDataBufferLeft);

		if(c_pchVUDataBufferRight)
			free(c_pchVUDataBufferRight);

		c_pchVUDataBufferLeft = 0;
		c_pchVUDataBufferRight = 0;
		c_nVUDataBufferSize = 0;
// ===============================================

		CloseHandle(c_hEvent_WOM_DONE);
		// memory allocation error, free all allocated memory
		if(c_WH_WaveHeader)
		{
			unsigned int i;
			for(i = 0; i < c_nNumberOfWaveBuffers; i++)
			{
				if(c_WH_WaveHeader[i].lpData)
					free(c_WH_WaveHeader[i].lpData );
			}

			free(c_WH_WaveHeader);
			c_WH_WaveHeader = 0;
		}
// ===============================================
// CLOSE DECODER
		c_decoder->Close();
		c_isInfo = 0;
// ================================================

		// clear decoder queue
		while(c_queue_decoder.GetCount())
		{
			AUDIO_DECODER ad;
			if(c_queue_decoder.PullFirst(&ad, sizeof(AUDIO_DECODER)))
			{
				// close decoder
				if(ad.decoder)
				{
					ad.decoder->Close();
					ad.decoder->Release();
				}

				// close file mapping
				_CloseFileMapping(&ad.fm, ad.queue);

				// delete queue
				delete ad.queue;
		
			}
		}

		// clear decoder queue
		c_queue_decoder.Clear();
	// ===============================================
		c_nWaveBufferSize = 0;

		if(c_pchWorkingWaveBuffer)
			free(c_pchWorkingWaveBuffer);

		c_pchWorkingWaveBuffer = 0;
		
		c_nCurrentPosition = 0;

		c_nSongIndex = 0;
		c_nOldSongIndex = 0;
		c_nCurrentSongIndex = 0;


	// WE ARE NOW CLOSED
		c_fReady = 0;
		
		LeaveCriticalSection(&c_CS_FFT);
	}


// close file mapping
	_CloseFileMapping(&c_MainFileMapping, &cMainQueue);
	_InitFileMapping(&c_MainFileMapping, &cMainQueue, 0);


	if(c_save_filename)
	{
		free(c_save_filename);
		c_save_filename = NULL;
	}

	c_save_format = sfUnknown;

	

	return 1;

	

}


void __stdcall WMp3x::GetVUData(unsigned int *pnLeftChannel, unsigned int *pnRightChannel)
{
	err(WMP3X_NO_ERROR);

		
	if(c_fReady == 0 || c_fPlay == 0)
	{
		if(pnLeftChannel)
			*pnLeftChannel = 0;

		if(pnRightChannel)
			*pnRightChannel = 0;

		return;

	}


// LOAD DATA INTO BUFFER

	// get current playing index
	unsigned int nCurrentIndex = c_nPlayingIndex;
	int nOffset = c_nSamplesForOneBuffer / 2 * 4;
	
	int nHave;
	short *psData;
	unsigned int i;
	unsigned int j = 0;
	
	unsigned int nNeed = c_nVUDataBufferSize / 2;
	unsigned int nNum = 0;
	for(i = 0; i < c_nNumberOfWaveBuffers; i++)
	{
		psData = (short*) (c_WH_WaveHeader[nCurrentIndex].lpData  + nOffset);
		nHave = (int) ( c_nBytesForOneBuffer - nOffset);
		nNum = 0;
	
	
		
		for(j; j < nNeed; j++)
		{
			if(nHave < 4 ) // go to next buffer
			{
				nCurrentIndex++;
				if(nCurrentIndex >= c_nNumberOfWaveBuffers)
					nCurrentIndex = 0;	
				nOffset = 0;
				break; 
			}

				
			c_pchVUDataBufferLeft[j] = (short) psData[nNum];
			c_pchVUDataBufferRight[j] = (short) psData[nNum + 1];
			nNum += 2;
			nHave -= 4;
		}
			
	}


	// if we don't have enough data in wave buffers, fill rest with 0
	for(j; j < nNeed / 2; j++) 		
	{
		c_pchVUDataBufferLeft[j] = 0;
		c_pchVUDataBufferRight[j] = 0;
	
	}		
	
// ==========================================================================
// now we have vu data in buffers, calculate VU data


	REAL rSumLeft = 0;
	REAL rSumRight = 0;
	REAL rAvgLeft;
	REAL rAvgRight;

	nNum = c_nVUDataBufferSize / 2;
	for(i = 0; i < nNum; i++)
	{
		
		rSumLeft += (REAL) abs(c_pchVUDataBufferLeft[i]);
		rSumRight += (REAL) abs(c_pchVUDataBufferRight[i]);
	}

	rAvgLeft = rSumLeft / (REAL) nNum;
	rAvgRight = rSumRight / (REAL) nNum;

		

	if(rAvgLeft < 21.61)
		*pnLeftChannel = 0;
	else
	{
		rAvgLeft = rAvgLeft / AVG_SINE_16;
		if(pnLeftChannel)
		{
			*pnLeftChannel =  (unsigned int) (100 + (33.0 * log10(rAvgLeft)));
			if(*pnLeftChannel > 100)
				*pnLeftChannel = 100;
		}
				
	}

	if(rAvgRight < 21.61)
		*pnRightChannel = 0;
	else
	{
		rAvgRight = rAvgRight / AVG_SINE_16;
		if(pnRightChannel)
		{
			*pnRightChannel =  (unsigned int) (100 + (33.0 * log10(rAvgRight)));
			if(*pnRightChannel > 100)
				*pnRightChannel = 100;
		}
				
	}
}






int __stdcall WMp3x::EnableEqualizer(int fEnable)
{
	err(WMP3X_NO_ERROR);
	if(c_EqualizerProcessor.Enable(0, fEnable) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}



int __stdcall WMp3x::SetEqualizerParam(int nPreAmpGain, int *pnBandGain, int nNumberOfBands)
{

	err(WMP3X_NO_ERROR);
	if(c_EqualizerProcessor.SetParameters(nPreAmpGain, pnBandGain, nNumberOfBands) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}





int __stdcall WMp3x::GetEqualizerParam( int *pnPreAmpGain, int *pnBandGain, int nNumberOfBands)
{
	err(WMP3X_NO_ERROR);

	int num = c_EqualizerProcessor.GetParameters(pnPreAmpGain, pnBandGain, nNumberOfBands);
	if(num == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return num;
}





int __stdcall WMp3x::PlayLoop(TTimeFormat fFormatStartTime, TStreamTime *pStartTime, TTimeFormat fFormatEndTime, TStreamTime *pEndTime, unsigned int nNumOfCycles, unsigned int fContinuePlaying)
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

	if(c_fManagedStream)
	{
		err(WMP3X_NOT_SUPPORTED_ON_MANAGED_STREAM);
		return 0;
	}

	if(pStartTime == 0 || fFormatStartTime == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(pEndTime == 0 || fFormatEndTime == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(nNumOfCycles	== 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}
			

	c_nLoopStartingPosition = Mp3TimeToSamples(fFormatStartTime, pStartTime, c_isInfo->nSampleRate);
	c_nLoopEndingPosition = Mp3TimeToSamples(fFormatEndTime, pEndTime, c_isInfo->nSampleRate);

	if(c_nLoopStartingPosition > c_isInfo->nLength)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(c_nLoopEndingPosition > c_isInfo->nLength)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}


	if(c_nLoopStartingPosition >= c_nLoopEndingPosition)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}


	// calculate loop length in samples
	c_nLoopLength = c_nLoopEndingPosition - c_nLoopStartingPosition;

// ==== STOP PLAYING, THIS WILL KILL THREAD =============

	Stop();

// === SEEK TO STARTING POSITION ========================


	if(c_decoder->Seek(c_nLoopStartingPosition) == 0)
	{
		err(c_decoder->GetError());
		return 0;
	}


	c_nCurrentPosition = c_nLoopStartingPosition;

	
	// enable loop processing
	c_nLoopSampleCounter = 0;
	c_nLoopCycleCounter = 0;
	c_nLoopCycleNumber = nNumOfCycles;
	c_fLoop = 1;

	c_fFlyingLoop = 0;
	if(fContinuePlaying)
		c_fFlyingLoop = 1;



// ======= CLOSE HANDLE OF PREVIOUS THREAD =========
	if(w_hThread)
		CloseHandle(w_hThread);

	w_hThread = 0;

// ==== ENABLE DECODING AND PLAYING LOOP  ==========
	c_fRunDecodingLoop = 1;
	c_fRunPlayingLoop = 1;


// ==== CREATE PLAYING THREAD =========
	c_thread_no_error = 1;
	w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFuncPCM, (void*) this,0,&w_hThreadID);

	if( w_hThread == NULL)
	{
    	err(WMP3X_THREAD_CREATION_FAIL);
        return 0;
	}
	
	// wait for thread to create and get possible error 
	WaitForSingleObject(c_hThreadStartEvent, INFINITE);
	if(c_thread_no_error == 0)	// we have some error in thread
	{
		// wait thread to end
		WaitForSingleObject(w_hThread, INFINITE);
		// close thread handle
		if(w_hThread)
			CloseHandle(w_hThread);

		w_hThread = 0;

		c_fLoop = 0;
		// ==============================

		return 0;
	}

	
// ===== WE ARE OK, THREAD IS CREATED =====

	// position calculation
	c_nBytesIn = 0;
	// number of output bytes
	c_nBytesOut = 0;
	// real bytes in and out
	c_nRealBytesIn = 0;
	c_nPlayingIndex = 0;
	c_fPause = 0;
	c_fPlay = 1;


    return 1;



}


int __stdcall WMp3x::MixChannels(int fEnable, unsigned int nLeftPercent, unsigned int nRightPercent)
{
	err(WMP3X_NO_ERROR);

	if(fEnable)
	{
		c_fMixChannels = 1;
		if(nLeftPercent > 100)
			nLeftPercent = 100;

		if(nRightPercent > 100)
			nRightPercent = 100;

		c_nMixLeftPerc = (REAL) nLeftPercent / 100.0;
		c_nMixRightPerc = (REAL) nRightPercent / 100.0;
	}
	else
		c_fMixChannels = 0;
			
	return 1;
}



int __stdcall WMp3x::SlideVolume(
			int fFormatStart,				// flag specifying time format of starting position
			TStreamTime *pTimeStart,			// pointer to TStreamTime structure specifying start fade position
			unsigned int nStartVolumeLeft,	// starting fade volume of left channel
			unsigned int nStartVolumeRight,	// starting fade volume of right channel
			int fFormatEnd,					// flag specifying time format of end position
			TStreamTime *pTimeEnd,				// pointer to TStreamTime structure specifying end fade position
			unsigned int nEndVolumeLeft,	// end fade volume of left channel
			unsigned int nEndVolumeRight	// end fade volume of right channel
)
{

	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}


	if(fFormatEnd == 0 || pTimeEnd == 0 || pTimeStart == NULL || pTimeEnd == NULL)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(nStartVolumeLeft > 100)
		nStartVolumeLeft = 100;

	if(nStartVolumeRight > 100)
		nStartVolumeRight = 100;

	if(nEndVolumeLeft > 100)
		nEndVolumeLeft = 100;

	if(nEndVolumeRight > 100)
		nEndVolumeRight = 100;

	EnterCriticalSection(&c_csFade);

	// calculate volume difference and volume change direction
	unsigned int nVolumeDiffLeft;
	unsigned int nVolumeDiffRight;
	if(nStartVolumeLeft < nEndVolumeLeft)
	{
		nVolumeDiffLeft = nEndVolumeLeft - nStartVolumeLeft;
		c_fFadeVolumeLeftUp = 1;
	}
	else
	{
		nVolumeDiffLeft = nStartVolumeLeft - nEndVolumeLeft;
		c_fFadeVolumeLeftUp = 0;
	}

	if(nStartVolumeRight < nEndVolumeRight)
	{
		nVolumeDiffRight = nEndVolumeRight - nStartVolumeRight;
		c_fFadeVolumeRightUp = 1;
	}
	else
	{
		nVolumeDiffRight = nStartVolumeRight - nEndVolumeRight;
		c_fFadeVolumeRightUp = 0;
	}
	


	
	c_nFadeStartingPosition = Mp3TimeToSamples(fFormatStart, pTimeStart, c_isInfo->nSampleRate);

	unsigned int nFadeEndPosition = Mp3TimeToSamples(fFormatEnd, pTimeEnd, c_isInfo->nSampleRate);

	// calculate number of wave buffers to fill one fade interval 
	if(nFadeEndPosition <= c_nFadeStartingPosition)
		c_nFadeFrameNum = 0;
	else
		c_nFadeFrameNum = MulDiv(nFadeEndPosition - c_nFadeStartingPosition, 1, c_nSamplesForOneBuffer); 
	
	

	// calculate fade  step for each wave buffer
	if(c_nFadeFrameNum > 0)
	{
		c_dFadeStepLeftChannel = (double) nVolumeDiffLeft / (double) c_nFadeFrameNum;
		c_dFadeStepRightChannel = (double) nVolumeDiffRight / (double) c_nFadeFrameNum;
	}
	else
	{
		c_dFadeStepLeftChannel = 0;
		c_dFadeStepRightChannel = 0;
	}

	c_nFadeFrameNum++;

	c_nFadeStartVolumeLeft = nStartVolumeLeft;
	c_nFadeStartVolumeRight = nStartVolumeRight;
	c_nFadeEndVolumeLeft = nEndVolumeLeft;
	c_nFadeEndVolumeRight = nEndVolumeRight;
	

	// reset counter to 0
	c_nFadeFrameCount = 0;
	// start checking for starting position
	c_fCheckFade = 1;
	// we are not in fade  yet.
	c_fFade = 0;



	LeaveCriticalSection(&c_csFade);

	return 1;
}






void __stdcall WMp3x::GetPosition(TStreamTime *pTime)
{
	err(WMP3X_NO_ERROR);

	if(pTime == 0)
		return;

	// set time to 0
	memset(pTime, 0, sizeof(TStreamTime));
	
	if(c_fReady == 0)
		return;
	
	unsigned int pos = c_nCurrentPosition;
	
	pTime->samples = pos;
	pos = (unsigned int) MulDiv(pos, 1000, c_isInfo->nSampleRate);
	pTime->ms = pos;
	pTime->sec = pos / 1000;
	pTime->hms.hour = pos / 3600000;
	pos = pos % 3600000;
	pTime->hms.minute = pos / 60000;
	pos = pos % 60000;
	pTime->hms.second = pos / 1000;
	pTime->hms.millisecond = pos % 1000;
	
}






void WMp3x::_GetInternalVolume(WMp3x *instance, unsigned int *lvolume, unsigned int *rvolume)
{
	*lvolume = instance->c_nLeftVolume;
	*rvolume = instance->c_nRightVolume;
}



char * __stdcall WMp3x::GetError()
{ 
	return (char*) &c_error_messageA;
}


wchar_t * __stdcall WMp3x::GetErrorW()
{ 
	return (wchar_t*) &c_error_messageW;
}



int __stdcall WMp3x::SetEqualizerPoints(int *pnFreqPoint, int nNumOfPoints)
{
	
	Stop(); // stop playing
	err(WMP3X_NO_ERROR);

	if(c_EqualizerProcessor.CreateEqBands(pnFreqPoint, nNumOfPoints) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}


int __stdcall WMp3x::GetEqualizerPoints(int *pnFreqPoint, int nFreqPointNumber)
{
	err(WMP3X_NO_ERROR);

	int num = c_EqualizerProcessor.GetEqBands(pnFreqPoint, nFreqPointNumber);
	if(num == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return num;
}






void __stdcall WMp3x::Release()
{
	delete this;
}


int __stdcall WMp3x::IsStreamDataFree(const void *sMemNewData)
{
	if(c_fReady == 0 || c_fManagedStream == 0)
		return 1;

	return cMainQueue.FindFromFirst((void*) sMemNewData);
}

int __stdcall WMp3x::PushDataToStream(const void *sMemNewData, unsigned int nNewDataize)
{
	err(WMP3X_NO_ERROR);

	// check if stream is ready	
	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

	// check if stream is managed
	if(c_fManagedStream == 0)
	{
		err(WMP3X_NOT_SUPPORTED_ONLY_ON_MANAGED_STREAM);
		return 0;
	}


	if(sMemNewData == NULL || nNewDataize == 0)
	{
		c_fEndOfStream = 1;

	}
	else
	{
		c_fEndOfStream = 0;
		// add data into queue
		if(cMainQueue.PushLast((void*) sMemNewData, nNewDataize) == 0)
		{
			err(WMP3X_MEMORY_ALLOCATION_FAIL);
			return 0;
		}
	}


	// set event to trigger waiting decoding loop
	SetEvent(c_hStreamEvent);

	return 1;

}


void __stdcall WMp3x::GetDynamicStreamLoad(TStreamLoadInfo *pStreamLoadInfo)
{
	err(WMP3X_NO_ERROR);

	if(pStreamLoadInfo == NULL)
		return;


	memset(pStreamLoadInfo, 0, sizeof(TStreamLoadInfo));

	if(c_fReady == 0 || c_fManagedStream == 0)
		return;

	pStreamLoadInfo->NumberOfBuffers = cMainQueue.GetCount();
	pStreamLoadInfo->NumberOfBytes = cMainQueue.GetSizeSum();

}



int __stdcall WMp3x::OpenStream(int fBuffered, int fManaged, const void *sMemStream, unsigned int nStreamSize, TStreamFormat nFormat)
{
	// close previous stream
	Close();

	err(WMP3X_NO_ERROR);

	if(sMemStream == 0 || nStreamSize == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	// detect stream format
	if(nFormat == sfAutodetect || nFormat == sfUnknown)
	{
		err(WMP3X_UNKNOWN_FILE_FORMAT);
		return 0;
	}

	// get decoder
	if(nFormat != c_nFormat)
	{
		if(_SetFormat(nFormat) == 0)
		{
			err(WMP3X_DECODER_CREATION_FAIL);
			return 0;
		}
	}

	if(c_decoder == 0)
	{
		err(WMP3X_NO_DECODER_SELECTED);
		return 0;
	}



	// set main queue
	cMainQueue.SetMemoryMode(fBuffered);
	if(cMainQueue.PushLast((void*) sMemStream, nStreamSize) == 0)
	{
		err(WMP3X_MEMORY_ALLOCATION_FAIL);
        return 0;

	}


	// open decoder
	if(c_decoder->OpenStream(&cMainQueue, fManaged, 0, 0) == 0)
	{
		cMainQueue.Clear();
		err(c_decoder->GetError());
		return 0;
	}

	c_fManagedStream = 0;
	if(fManaged)
	{
		c_fEndOfStream = 0;
		c_fManagedStream = 1;
	}


	// decoder is ready now, prepare output
	if(_Open(this, c_nWaveBufferLengthMs) == 0)
	{
		c_decoder->Close();
		cMainQueue.Clear();
		return 0;

	}

	return 1;
}


int __stdcall WMp3x::EnumerateWaveOut()
{
	err(WMP3X_NO_ERROR);

	c_nOutputDevicesNumber = waveOutGetNumDevs();

// allocate memory for device informations
	WAVEOUTCAPSA *pwoc = (WAVEOUTCAPSA*) malloc(c_nOutputDevicesNumber * sizeof(WAVEOUTCAPSA));
	if(!pwoc)
	{
		err(WMP3X_MEMORY_ALLOCATION_FAIL);
		c_nOutputDevicesNumber = 0;
		return 0;
	}

	// retrieves the capabilities of a given waveform-audio output device.
	int num1 = 0;
	unsigned int i;
	for(i = 0; i < c_nOutputDevicesNumber; i++)
	{
		if(waveOutGetDevCapsA(i, &pwoc[num1], sizeof(WAVEOUTCAPSA)) == MMSYSERR_NOERROR)
			num1++;	
	}

	// free old informations
	if(c_pWOC)
		free(c_pWOC);

	// assing new informations
	c_pWOC = pwoc;
	c_nOutputDevicesNumber = num1;

	if(c_nOutputDevicesNumber == 0)
	{
		free(c_pWOC);
		c_pWOC = 0;
		err(WMP3X_NO_WAVE_DEVICE_PRESENT);
		return 0;
	}

	return c_nOutputDevicesNumber;

}

int __stdcall WMp3x::GetWaveOutInfo(unsigned int nIndex, TWaveOutInfo *pWaveOutInfo)
{
	err(WMP3X_NO_ERROR);

	if(pWaveOutInfo == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);	
		return 0;
	}

	memset(pWaveOutInfo, 0, sizeof(TWaveOutInfo));


	if(c_nOutputDevicesNumber == 0 || c_pWOC == 0)
	{
		if(EnumerateWaveOut() == 0)
			return 0;
	}

	if(nIndex >= c_nOutputDevicesNumber)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}



	WAVEOUTCAPS *caps = &c_pWOC[nIndex];

	pWaveOutInfo->ManufacturerID = caps->wMid;
	pWaveOutInfo->ProductID = caps->wPid;
	pWaveOutInfo->DriverVersion = caps->vDriverVersion;
	pWaveOutInfo->Formats = caps->dwFormats;
	pWaveOutInfo->Channels = caps->wChannels;
	pWaveOutInfo->Support = caps->dwSupport;
	memcpy(c_WaveOutProductName, caps->szPname, 32);
	c_WaveOutProductName[31] = 0;
	pWaveOutInfo->ProductName = c_WaveOutProductName;

	return 1;
}

int __stdcall WMp3x::GetWaveOutInfoW(unsigned int nIndex, TWaveOutInfoW *pWaveOutInfo)
{
	err(WMP3X_NO_ERROR);

	if(pWaveOutInfo == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);		
		return 0;
	}

	memset(pWaveOutInfo, 0, sizeof(TWaveOutInfoW));

	TWaveOutInfo wi;
	if(GetWaveOutInfo(nIndex, &wi) == 0)
		return 0;




	pWaveOutInfo->ManufacturerID = wi.ManufacturerID;
	pWaveOutInfo->ProductID = wi.ProductID;
	pWaveOutInfo->DriverVersion = wi.DriverVersion;
	pWaveOutInfo->Formats = wi.Formats;
	pWaveOutInfo->Channels = wi.Channels;
	pWaveOutInfo->Support = wi.Support;
	c_WaveOutProductNameW[0] = 0;
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, wi.ProductName, -1, c_WaveOutProductNameW, 32); 
	c_WaveOutProductNameW[31] = 0;
	pWaveOutInfo->ProductName = c_WaveOutProductNameW;

	return 1;
}


int __stdcall WMp3x::EnumerateWaveIn()
{
	err(WMP3X_NO_ERROR);

	c_nInputDevicesNumber = waveInGetNumDevs();

// allocate memory for device informations
	WAVEINCAPSA *pwic = (WAVEINCAPSA*) malloc(c_nInputDevicesNumber * sizeof(WAVEINCAPSA));
	if(!pwic)
	{
		err(WMP3X_MEMORY_ALLOCATION_FAIL);
		c_nInputDevicesNumber = 0;
		return 0;
	}

	// retrieves the capabilities of a given waveform-audio output device.
	int num1 = 0;
	unsigned int i;
	for(i = 0; i < c_nInputDevicesNumber; i++)
	{
		if(waveInGetDevCapsA(i, &pwic[num1], sizeof(WAVEINCAPSA)) == MMSYSERR_NOERROR)
			num1++;	
	}

	// free old informations
	if(c_pWIC)
		free(c_pWIC);

	// assing new informations
	c_pWIC = pwic;
	c_nInputDevicesNumber = num1;

	if(c_nInputDevicesNumber == 0)
	{
		free(c_pWIC);
		c_pWIC = 0;
		err(WMP3X_NO_WAVE_DEVICE_PRESENT);
		return 0;
	}

	return c_nInputDevicesNumber;

}

int __stdcall WMp3x::GetWaveInInfo(unsigned int nIndex, TWaveInInfo *pWaveInInfo)
{
	err(WMP3X_NO_ERROR);

	if(pWaveInInfo == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);	
		return 0;
	}

	memset(pWaveInInfo, 0, sizeof(TWaveInInfo));


	if(c_nInputDevicesNumber == 0 || c_pWIC == 0)
	{
		if(EnumerateWaveIn() == 0)
			return 0;
	}

	if(nIndex >= c_nInputDevicesNumber)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}



	WAVEINCAPS *caps = &c_pWIC[nIndex];
	pWaveInInfo->ManufacturerID = caps->wMid;
	pWaveInInfo->ProductID = caps->wPid;
	pWaveInInfo->DriverVersion = caps->vDriverVersion;
	pWaveInInfo->Formats = caps->dwFormats;
	pWaveInInfo->Channels = caps->wChannels;
	memcpy(c_WaveInProductName, caps->szPname, 32);
	c_WaveInProductName[31] = 0;
	pWaveInInfo->ProductName = c_WaveInProductName;

	return 1;
}

int __stdcall WMp3x::GetWaveInInfoW(unsigned int nIndex, TWaveInInfoW *pWaveInInfo)
{
	err(WMP3X_NO_ERROR);

	if(pWaveInInfo == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);		
		return 0;
	}

	memset(pWaveInInfo, 0, sizeof(TWaveInInfoW));

	TWaveInInfo wi;
	if(GetWaveInInfo(nIndex, &wi) == 0)
		return 0;


	pWaveInInfo->ManufacturerID = wi.ManufacturerID;
	pWaveInInfo->ProductID = wi.ProductID;
	pWaveInInfo->DriverVersion = wi.DriverVersion;
	pWaveInInfo->Formats = wi.Formats;
	pWaveInInfo->Channels = wi.Channels;

	c_WaveInProductNameW[0] = 0;
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, wi.ProductName, -1, c_WaveInProductNameW, 32); 
	c_WaveInProductNameW[31] = 0;
	pWaveInInfo->ProductName = c_WaveInProductNameW;

	return 1;
}

int __stdcall WMp3x::SetWaveInDevice(unsigned int nIndex)
{
	err(WMP3X_NO_ERROR);

	if(nIndex == WaveInWaveMapper)
	{
		c_nCurrentWIC = nIndex;
		return 1;
	}


	if(nIndex >= EnumerateWaveIn())
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);	
		return 0;
	}

	c_nCurrentWIC = nIndex;
	
	return 1;
}



int __stdcall WMp3x::SetWaveOutDevice(unsigned int nIndex)
{
	err(WMP3X_NO_ERROR);

	if(nIndex == WaveOutWaveMapper)
	{
		c_nCurrentWOC = nIndex;
		return 1;
	}


	if(nIndex >= EnumerateWaveOut())
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);	
		return 0;
	}

	c_nCurrentWOC = nIndex;
	
	return 1;
}



unsigned int Mp3TimeToSamples(int fFormat, TStreamTime *pTime, unsigned int nSampleRate)
{
	if(pTime == 0)
		return 0;

	switch(fFormat)
	{

		case tfSamples:
			return pTime->samples;
		break;

		case tfMillisecond:
		return  (unsigned int) MulDiv(pTime->ms, nSampleRate, 1000);

		case tfSecond:
		return (pTime->sec * nSampleRate);

		case tfHMS:
		{
			unsigned int seek_time;
			seek_time = pTime->hms.millisecond;
			seek_time += pTime->hms.second * 1000;
			seek_time += pTime->hms.minute * 60000;
			seek_time += pTime->hms.hour * 3600000;
			return ((unsigned int) MulDiv(seek_time, nSampleRate, 1000));
		}
	}

	return 0;
}

int __stdcall WMp3x::ReverseMode(int fEnable)
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}

	if(fEnable)
	{
		if(c_fReverseMode)
			return 1;

		TStreamStatus s;
		s.fPlay = 0;
		GetStatus(&s);
		Pause();
		TStreamTime t;
		t.samples = 0;
		// get current position
		GetPosition(&t);
		// stop playing
		Stop();
		// set decoder in reverse mode
		if(c_decoder->SetReverseMode(1) == 0)
		{		
			err(c_decoder->GetError());
			return 0;
		}

		c_fReverseMode = 1;

		if(Seek(tfSamples, &t, smFromBeginning) == 0)
			return 0;

		if(s.fPlay)
			return Play();
			
	}
	else
	{
		if(c_fReverseMode == 0)
			return 1;

		TStreamStatus s;
		s.fPlay = 0;
		GetStatus(&s);
		Pause();
		TStreamTime t;
		t.samples = 0;
		// get current position
		GetPosition(&t);
		// stop playing
		Stop();
		// set decoder to normal mode
		if(c_decoder->SetReverseMode(0) == 0)
		{		
			err(c_decoder->GetError());
			return 0;
		}

		
		c_fReverseMode = 0;

		if(Seek(tfSamples, &t, smFromBeginning) == 0)
			return 0;
		
		if(s.fPlay)
			return Play();

		
	}

	return 1;
}



void WMp3x::_ChangeVolume(WMp3x *instance, unsigned char *buf, unsigned int size)
{
	unsigned int j;
	short* left;
	short* right;

    for(j = 0; j < size; j += 4)
	{           	
		left = (short*) ( buf + j);
        right = (short*) ( buf + j + 2);
        *left = (short) ((double)*left  *  instance->c_dLeftGain);
        *right = (short) ((double)*right * instance->c_dRightGain);
	}	
}






int __stdcall WMp3x::GetVersion()
{
	return LIBRARY_VERSION;
}

int __stdcall WMp3x::SetCallbackFunc(TCallbackFunc callback, TCallbackMessage nMessages, void *user_data)
{
	err(WMP3X_NO_ERROR);

	if(callback == 0)
	{
		callback_func = 0;
		callback_messages = 0;
		if(c_hThreadCallback)
		{
			PostThreadMessage(c_hThreadCallbackId, WM_QUIT, 0,0);
			if(WaitForSingleObject(c_hThreadCallback, 500) == WAIT_TIMEOUT)
				TerminateThread(c_hThreadCallback, 0);

			CloseHandle(c_hThreadCallback);
			c_hThreadCallback = 0;

		}

	}
	else
	{
		c_user_callback_data = user_data;
		if(c_hThreadCallback)
		{
			PostThreadMessage(c_hThreadCallbackId, WM_QUIT, 0,0);
			if(WaitForSingleObject(c_hThreadCallback, 500) == WAIT_TIMEOUT)
				TerminateThread(c_hThreadCallback, 0);

			CloseHandle(c_hThreadCallback);
			c_hThreadCallback = 0;
		}
				
		ResetEvent(c_hEventCallback);
		c_hThreadCallback = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) CallbackThreadFunc, (void*) this, 0, &c_hThreadCallbackId);
		if(c_hThreadCallback == 0)
		{
			err(WMP3X_THREAD_CREATION_FAIL);
			return 0;

		}

		WaitForSingleObject(c_hEventCallback, INFINITE);
			 
		callback_func = callback;
		callback_messages = nMessages;
	}

	return 1;
}


int WINAPI WMp3x::CallbackThreadFunc(void* lpdwParam)
{
	WMp3x *instance = (WMp3x*) lpdwParam;
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	SetEvent(instance->c_hEventCallback);


	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		unsigned int message = (unsigned int) msg.message - WM_USER; 

		if(message > 0x3DFF) 
		{
			message = (message - 0x3DFF) << 16;
			instance->c_callback_ret = instance->callback_func( instance, instance->c_user_callback_data, (TCallbackMessage) message, msg.wParam, msg.lParam);
			SetEvent(instance->c_hEventCallback);
		}
		else
		{
			instance->c_callback_ret = instance->callback_func(instance, instance->c_user_callback_data, (TCallbackMessage)  message, msg.wParam, msg.lParam);
		}
	}
	

	return 1;
}


int __stdcall WMp3x::SetMasterVolume(unsigned int nLeftVolume,unsigned int nRightVolume)
{
	err(WMP3X_NO_ERROR);

    if( nLeftVolume > 100)
    	nLeftVolume = 100;

	if( nRightVolume > 100)
    	nRightVolume = 100;



	unsigned int l = (unsigned int) (nLeftVolume * 0xFFFF / 100);
	unsigned int r = (unsigned int) (nRightVolume * 0xFFFF / 100);

	// set wave device ID
	unsigned int wave_output_id;
	if(c_nCurrentWOC == WaveOutWaveMapper)
		wave_output_id = WAVE_MAPPER;
	else
		wave_output_id = c_nCurrentWOC;


	MMRESULT res = waveOutSetVolume((HWAVEOUT) wave_output_id, MAKELONG(l, r));
	if(res != MMSYSERR_NOERROR)
	{
		err(WMP3X_WAVEOUT_ERROR);
		return 0;

	}

	return 1;
}


int __stdcall WMp3x::SetPlayerVolume(unsigned int nLeftVolume, unsigned int nRightVolume)
{
	err(WMP3X_NO_ERROR);

	EnterCriticalSection(&c_csFade);
	c_fFade = 0;
	c_fCheckFade = 0;
	LeaveCriticalSection(&c_csFade);


	if( nLeftVolume > 100)
    	nLeftVolume = 100;
 
	if( nRightVolume > 100)
    	nRightVolume = 100;

	c_nLeftVolume =  nLeftVolume;
	c_nRightVolume = nRightVolume;

	if(nLeftVolume == 100)
		c_dLeftGain = 1.0;
	else
		c_dLeftGain = (pow(10.0, (double) nLeftVolume / 100.0) - 1.0) / 9.0;
		
	if(c_nRightVolume == 100)
		c_dRightGain = 1.0;
	else
		c_dRightGain = (pow(10.0, (double) nRightVolume / 100.0) - 1.0) / 9.0;	



	return 1;

}

void WMp3x::_SetPlayerVolume(WMp3x *instance, unsigned int nLeftVolume, unsigned int nRightVolume)
{
	if( nLeftVolume > 100)
    	nLeftVolume = 100;
 
	if( nRightVolume > 100)
    	nRightVolume = 100;

	instance->c_nLeftVolume =  nLeftVolume;
	instance->c_nRightVolume = nRightVolume;

	if(nLeftVolume == 100)
		instance->c_dLeftGain = 1.0;
	else
		instance->c_dLeftGain = (pow(10.0, (double) nLeftVolume / 100.0) - 1.0) / 9.0;
		
	if(instance->c_nRightVolume == 100)
		instance->c_dRightGain = 1.0;
	else
		instance->c_dRightGain = (pow(10.0, (double) nRightVolume / 100.0) - 1.0) / 9.0;	

}


void __stdcall WMp3x::GetMasterVolume(unsigned int *pnLeftVolume,unsigned int *pnRightVolume)
{
	err(WMP3X_NO_ERROR);


	unsigned int lvolume = 100;
	unsigned int rvolume = 100;

	// set wave device ID
	unsigned int wave_output_id;
	if(c_nCurrentWOC == WaveOutWaveMapper)
		wave_output_id = WAVE_MAPPER;
	else
		wave_output_id = c_nCurrentWOC;

	DWORD volume;
	MMRESULT res = waveOutGetVolume((HWAVEOUT) wave_output_id, &volume);
	if(res == MMSYSERR_NOERROR)
	{
		lvolume = LOWORD(volume);
		rvolume = HIWORD(volume);
	}

	if(pnLeftVolume)
		*pnLeftVolume = MulDiv(lvolume , 100 , 65535);

	if(pnRightVolume)
		*pnRightVolume = MulDiv(rvolume , 100 , 65535);

	

}

void __stdcall WMp3x::GetPlayerVolume(unsigned int *pnLeftVolume, unsigned int *pnRightVolume)
{
	err(WMP3X_NO_ERROR);

	if(pnLeftVolume)
		*pnLeftVolume = c_nLeftVolume;

	if(pnRightVolume)
		*pnRightVolume = c_nRightVolume;

}




int WMp3x::_SendFadeEnter(WMp3x *instance)
{

	int callback_ret = 0;
	if(instance->callback_messages & MsgEnterVolumeSlide)			
		callback_ret = instance->send_callback_message(instance, MsgEnterVolumeSlide, 0, 0);

	if(instance->callback_messages & MsgEnterVolumeSlideAsync)
		send_callback_message(instance, MsgEnterVolumeSlideAsync, 0,0);


/*
	if(instance->c_fEnterFadeSent == 0)
	{
		instance->c_fEnterFadeSent = 1;

		if(instance->callback_messages & MsgEnterVolumeSlide)			
			callback_ret = instance->send_callback_message(instance, MsgEnterVolumeSlide, 0, 0);

		if(instance->callback_messages & MsgEnterVolumeSlideAsync)
			send_callback_message(instance, MsgEnterVolumeSlideAsync, 0,0);
	}

	*/

	return callback_ret;	

}

int WMp3x::_SendFadeExit(WMp3x *instance)
{
	int callback_ret = 0;
	if(instance->callback_messages & MsgExitVolumeSlide)
		callback_ret = send_callback_message(instance, MsgExitVolumeSlide, 0, 0);

	if(instance->callback_messages & MsgExitVolumeSlideAsync)
		send_callback_message(instance,  MsgExitVolumeSlideAsync,0,0);

/*
	if(instance->c_fEnterFadeSent == 1 &&  instance->c_fExitFadeSent == 0)
	{
		instance->c_fExitFadeSent = 1;
		if(instance->callback_messages & MsgExitVolumeSlide)
			callback_ret = send_callback_message(instance, MsgExitVolumeSlide, 0, 0);

		if(instance->callback_messages & MsgExitVolumeSlideAsync)
			send_callback_message(instance,  MsgExitVolumeSlideAsync,0,0);

	}

*/
	return callback_ret;
}




int WMp3x::_SendLoopEnter(WMp3x *instance)
{
	int callback_ret = 0;

	if(instance->c_fLoopEnterSent == 0)
	{
		instance->c_fLoopEnterSent = 1;
		if(instance->callback_messages & MsgEnterLoop)
			callback_ret = instance->send_callback_message(instance, MsgEnterLoop, 0, 0);

		if(instance->callback_messages & MsgEnterLoopAsync)
			send_callback_message(instance, MsgEnterLoopAsync,0,0);


	}

	return callback_ret;
}


int WMp3x::_SendLoopExit(WMp3x *instance)
{
	int callback_ret = 0;


	if(instance->c_fLoopExitSent == 0)
	{
		instance->c_fLoopExitSent = 1;
		if(instance->callback_messages & MsgExitLoop)
			callback_ret = instance->send_callback_message(instance, MsgExitLoop, 0, 0);

		if(instance->callback_messages & MsgExitLoopAsync)
			send_callback_message(instance, MsgExitLoopAsync, 0,0);


	}

	return callback_ret;
}




int WMp3x::send_buffer_done(WMp3x *instance, unsigned int count, unsigned int sumsize)
{
	int callback_ret = 0;

	if(instance->callback_messages & MsgStreamBufferDone)
		callback_ret = send_callback_message(instance, MsgStreamBufferDone, count, sumsize);

	if(instance->callback_messages & MsgStreamBufferDoneAsync)
		send_callback_message(instance, MsgStreamBufferDoneAsync , count, sumsize);	
								
	return callback_ret;
}



int WMp3x::send_stream_out_of_data(WMp3x *instance)
{
	int callback_ret = 0;

	if(instance->callback_messages & MsgStreamNeedMoreData)
		callback_ret = send_callback_message(instance, MsgStreamNeedMoreData, 0, 0);

	if(instance->callback_messages & MsgStreamNeedMoreDataAsync)
		send_callback_message(instance, MsgStreamNeedMoreDataAsync , 0, 0);	
								
	return callback_ret;
}



int WMp3x::send_stop(WMp3x *instance)
{
	int callback_ret = 0;
	
	if(instance->callback_messages & MsgStop)
		callback_ret = send_callback_message(instance, MsgStop, 0, 0);

	if(instance->callback_messages & MsgStopAsync)
		send_callback_message(instance, MsgStopAsync, 0, 0);

	return callback_ret;
}



int WMp3x::send_callback_message(WMp3x *instance, unsigned int message, unsigned int param1, unsigned int param2)
{
	instance->c_callback_ret = 0;
	
	if(message > 0xFFFF)
	{
		// call directly
		instance->c_callback_ret = instance->callback_func( instance, instance->c_user_callback_data, (TCallbackMessage) message, param1, param2);
	}
	else
	{
		// async call through independent thread
		PostThreadMessage(instance->c_hThreadCallbackId, message + WM_USER, param1, param2);
	}


	return instance->c_callback_ret;
}





void WMp3x::load_fft_stereo_buffer(WMp3x *instance, int *pSamples, unsigned int nNumberOfSamples)
{

	// get current playing index
	unsigned int nCurrentIndex = instance->c_nPlayingIndex;

	unsigned int nHaveSamples = instance->c_WH_WaveHeader[nCurrentIndex].dwBufferLength / 4;	// 16 bit stereo
	unsigned int nNeedSamples = nNumberOfSamples;
	int nOffset = nHaveSamples / 2;
	int *pData = (int*) instance->c_WH_WaveHeader[nCurrentIndex].lpData;
	nHaveSamples -= nOffset;

	unsigned int i;	// count wave buffers
	unsigned int j = 0;	// count samples
	

	for(i = 0; i < instance->c_nNumberOfWaveBuffers; i++)
	{
		for(j; j < nNeedSamples; j++)
		{
			if(nHaveSamples == 0)
			{
				nCurrentIndex++;
				if(nCurrentIndex >= instance->c_nNumberOfWaveBuffers)
					nCurrentIndex = 0;	
				nOffset = 0;
				pData = (int*) instance->c_WH_WaveHeader[nCurrentIndex].lpData;
				nHaveSamples = instance->c_WH_WaveHeader[nCurrentIndex].dwBufferLength / 4;

				break;
			}

			pSamples[j] = pData[nOffset];
			nOffset++;
			nHaveSamples--;
		}	
	}

	// if we don't have enough data in wave buffers, fill rest with 0
	for(j; j < nNeedSamples; j++) 		
		pSamples[j] = 0;		
	


}


void WMp3x::_MixChannels(WMp3x *instance, unsigned char *buf, unsigned int size)
{
	unsigned int j;
	short* left;
	short* right;
	int mix;

    for(j = 0; j < size; j += 4)
	{           	
		left = (short*) ( buf + j);
        right = (short*) ( buf + j + 2);
		mix = (short) ((REAL)*left * (REAL) instance->c_nMixLeftPerc + (REAL)*right * (REAL) instance->c_nMixRightPerc);
        *left = mix;
        *right = mix;
	}
}


int  WMp3x::_SetFormat(TStreamFormat nFormat)
{
	
	Stop();
	Close();

	err(WMP3X_NO_ERROR);

	WAudioDecoder *decoder = _GetAudioDecoder(nFormat);
	if(decoder == NULL)
	{
		err(WMP3X_DECODER_CREATION_FAIL);
		return 0;
	}

	if(c_decoder)
		c_decoder->Release();

	c_decoder = decoder;

	switch(nFormat)
	{

		case sfWaveIn:
		{
			// set wave device ID
			unsigned int wave_input_id;
			if(c_nCurrentWIC == WaveInWaveMapper)
				wave_input_id = WAVE_MAPPER;
			else
				wave_input_id = c_nCurrentWIC;


			WAVEIN_INIT_DATA data;
			data.nDeviceID = wave_input_id;
			data.nMixerInputLineIndex = c_user_mixer_line_id;
			data.nVolume = c_user_mixer_volume;
			data.sMixerInputLineName = c_user_mixer_line;
			data.nWaveBufferSize = c_nWaveInBufferSize;

			if(c_decoder->Initialize((int) &data, 0, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfWaveIn;
		}
		break;

		case sfPCM:
		{
			if(c_decoder->Initialize(c_nPCMSampleRate, c_nPCMChannelNumber, c_nPCMBitPerSample, c_nPCMBigEndian) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfPCM;
			c_param1 = c_nPCMSampleRate;
			c_param2 = c_nPCMChannelNumber;
			c_param3 = c_nPCMBitPerSample;
			c_param4 = c_nPCMBigEndian;
		}
		break;

#ifndef LIBZPLAY_PF_VERSION
		case sfMp3:
		{
			if(c_decoder->Initialize(c_fAccurateLength, c_fAccurateSeek, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfMp3;
			c_param1 = c_fAccurateLength;
			c_param2 = c_fAccurateSeek;
			c_param3 = 0;
			c_param4 = 0;
		}
		break;


		case sfAacADTS:
			if(c_decoder->Initialize(0, 0, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfAacADTS;
			c_param1 = 0;
			c_param2 = 0;
			c_param3 = 0;
			c_param4 = 0;


		break;

#endif

		case sfOgg:
		{

			if(c_decoder->Initialize(0, 0, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfOgg;
			c_param1 = 0;
			c_param2 = 0;
			c_param3 = 0;
			c_param4 = 0;
		}
		break;


		case sfAC3:
		{

			if(c_decoder->Initialize(c_fAccurateLength, c_fAccurateSeek, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfAC3;
			c_param1 = c_fAccurateLength;
			c_param2 = c_fAccurateSeek;
			c_param3 = 0;
			c_param4 = 0;
		}
		break;


		case sfFLAC:
		{

			if(c_decoder->Initialize(0, 0, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfFLAC;
			c_param1 = 0;
			c_param2 = 0;
			c_param3 = 0;
			c_param4 = 0;

		}
		break;

		case sfFLACOgg:
		{

			if(c_decoder->Initialize(1, 0, 0, 0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfFLACOgg;
			c_param1 = 1;
			c_param2 = 0;
			c_param3 = 0;
			c_param4 = 0;

		}
		break;
		

		case sfWav:
			if(c_decoder->Initialize(0,0,0,0) == 0)
			{
				err(c_decoder->GetError());
				c_decoder->Release();
				c_decoder = 0;
				return 0;
			}

			c_nFormat = sfWav;
			c_param1 = 0;
			c_param2 = 0;
			c_param3 = 0;
			c_param4 = 0;


		break;

		
		default:
			err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	// set global reverse mode, by default, reverse mode is disabled on new decoder instance
	if(c_fReverseMode)
		c_decoder->SetReverseMode(c_fReverseMode);

	return 1;
}




int WINAPI WMp3x::_ThreadFuncPCM(void* lpdwParam)
{
// main decoding thread
// when this thread ends wave output is closed, position is set to 0 

	WMp3x* instance = (WMp3x*) lpdwParam;
	
// ==============================================================================================
//	INITIALIZATION
// ==============================================================================================


// ==============================================================
// SEND CALLBACK MESSAGES

	
	int call_ret = 0;
	// send play callback messages
	if(instance->callback_messages & MsgPlay)
		call_ret = send_callback_message(instance, MsgPlay, 0, 0);

	// check return value from play callback message
	if(call_ret == 1)	// user request to stop playing
	{
		instance->c_fCheckFade = 0;
		instance->c_fFade = 0;
		instance->c_fLoop = 0;
		instance->c_fPlay = 0;
		instance->c_fPause = 0;
		send_stop(instance);
		instance->c_thread_no_error = 1;
		SetEvent(instance->c_hThreadStartEvent);
    	return 1;
	}
	
	
	// send async play callback message
	if(instance->callback_messages & MsgPlayAsync)
		send_callback_message(instance, MsgPlayAsync, 0, 0);


// open save file

	HANDLE hFile = INVALID_HANDLE_VALUE;

	if(instance->c_save_filename)
	{
		switch(instance->c_save_format)
		{
			default:
				instance->err(WMP3X_UNSUPPORTED_ENCODER);
				// send stop callback messages
				send_stop(instance);
				instance->c_HWO_WaveOut = 0;
				instance->c_fCheckFade = 0;
				instance->c_fFade = 0;
				instance->c_fLoop = 0;
				instance->c_fPlay = 0;
				instance->c_fPause = 0;
				instance->c_thread_no_error = 0;
				SetEvent(instance->c_hThreadStartEvent);
			return 0;

		}

		

			// open output file	
		hFile = CreateFileW(instance->c_save_filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);	
		if(hFile == INVALID_HANDLE_VALUE)
		{
			instance->err(WMP3X_CANT_OPEN_SAVE_FILE);
			// send stop callback messages
			send_stop(instance);
			instance->c_HWO_WaveOut = 0;
			instance->c_fCheckFade = 0;
			instance->c_fFade = 0;
			instance->c_fLoop = 0;
			instance->c_fPlay = 0;
			instance->c_fPause = 0;
			instance->c_thread_no_error = 0;
			SetEvent(instance->c_hThreadStartEvent);
    		return 0;
		}




	}
// ==============================================================		
// open wave output	

	// set wave device ID
	unsigned int wave_output_id;
	if(instance->c_nCurrentWOC == WaveOutWaveMapper)
		wave_output_id = WAVE_MAPPER;
	else
		wave_output_id = instance->c_nCurrentWOC;


	if(instance->c_fUseWaveOut) // if use audio output
	{
		// try to open wave output
	
		// protect
		EnterCriticalSection(&instance->c_CS_waveOut);
		MMRESULT res;
		if( (res = waveOutOpen(&instance->c_HWO_WaveOut, wave_output_id, &instance->c_wfx,
						(DWORD) _WaveOutProc, (DWORD) instance, CALLBACK_FUNCTION)) != MMSYSERR_NOERROR)
		{

			
			

			if(hFile != INVALID_HANDLE_VALUE)
				CloseHandle(hFile);


			instance->err(WMP3X_WAVEOUT_ERROR);
			// send stop callback messages
			send_stop(instance);
			instance->c_HWO_WaveOut = 0;
			instance->c_fCheckFade = 0;
			instance->c_fFade = 0;
			instance->c_fLoop = 0;
			instance->c_fPlay = 0;
			instance->c_fPause = 0;
			instance->c_thread_no_error = 0;
			LeaveCriticalSection(&instance->c_CS_waveOut);
			SetEvent(instance->c_hThreadStartEvent);
    		return 0;
		}
		LeaveCriticalSection(&instance->c_CS_waveOut);
	}
// ==============================================================
// ALL OK FOR NOW
	
	// no error in thread
	instance->c_thread_no_error = 1;
	// enable Play() function to end
	SetEvent(instance->c_hThreadStartEvent);

// =========================================================
	// INITIALIZE PLAYING VARIABLES
	// set index to first wave buffer, we will start with 0 wave buffer
	instance->c_nWaveBufferIndex = 0;
	// set current wave buffer
	instance->c_pchCurrentWaveBuffer = instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].lpData;
	// set numbr of bytes we need to fill current wave buffer
	instance->c_nHaveBytesInWaveBuffer = 0;
	// number of real bytes in wave buffer
	instance->c_nRealBytesHaveInWaveBuffer = 0;

	// reset event
	ResetEvent(instance->c_hEvent_WOM_DONE);
	

	// flags controlling fade and loop callback messages
	instance->c_nFadeSemaphore = 0;
	instance->c_fLoopEnterSent = 0;
	instance->c_fLoopExitSent = 0;

	// position calculation
	instance->c_nBytesIn = 0;
	// number of output bytes
	instance->c_nBytesOut = 0;
	// real bytes in and out
	instance->c_nRealBytesIn = 0;

// =========================================================
//	GO INTO PLAYING LOOP


	int ret;
	while(1)
	{
		EnterCriticalSection(&instance->c_CS_DecodingLoop);
		if(instance->c_fRunDecodingLoop == 0)
		{
			LeaveCriticalSection(&instance->c_CS_DecodingLoop);
			break;
		}

		if(instance->c_fClearChain)
		{
			instance->c_InputProcessor.Clear(1);
			instance->c_fClearChain = 0;
			instance->c_fRunPlayingLoop = 1;
		}

		
		
		// prepare DECODER_DATA structure

		instance->c_DecoderData.pSamples = (short*) instance->c_pchWorkingWaveBuffer;
		instance->c_DecoderData.nSamplesNum = instance->c_nSamplesForOneBuffer;
		instance->c_DecoderData.fEndOfStream = instance->c_fEndOfStream;

		// get data from decoder
		ret = instance->c_decoder->GetData(&instance->c_DecoderData);

		instance->c_DecoderData.nSongIndex = instance->c_nSongIndex;
		LeaveCriticalSection(&instance->c_CS_DecodingLoop);

		switch(ret)
		{
			case DECODER_OK:
			{
				// send buffer done callback message
				if(instance->c_DecoderData.nBufferDone)
					send_buffer_done(instance, instance->c_DecoderData.nBufferInQueue, instance->c_DecoderData.nBytesInQueue);
				
				// play data from chain
				ret = _PlayingLoop(instance, 0);
				if(ret  == PLAY_OK)	
					continue;	// get more data from decoder
				
				
			
			}
			break;

			case DECODER_END_OF_DATA:
			{

				if(instance->c_queue_decoder.GetCount())
				{
					AUDIO_DECODER ad;
					WAudioDecoder *decoder = NULL;
					if(instance->c_queue_decoder.PullFirst(&ad, sizeof(AUDIO_DECODER)))
					{
						decoder = ad.decoder;
						if(decoder)
						{
							instance->c_InputProcessor.Flush(1);
							instance->c_nSongIndex++;


							// delete old decoder, but preserve info
							char *desc = strdup(instance->c_isInfo->pchStreamDescription);
							if(desc)
							{
								if(instance->c_desc_backup)
									free(instance->c_desc_backup);

								instance->c_desc_backup = desc;
							}

							memcpy(&instance->c_isInfo_backup, instance->c_isInfo, sizeof(INPUT_STREAM_INFO));
							instance->c_isInfo_backup.pchStreamDescription = instance->c_desc_backup;
							
							if(instance->c_isInfo_backup.pchStreamDescription == NULL)
								instance->c_isInfo_backup.pchStreamDescription = "";

							instance->c_isInfo = &instance->c_isInfo_backup;

							// info backup done, delete old decoder
							instance->c_decoder->Close();
							instance->c_decoder->Release();

							// close file mapping
							_CloseFileMapping(&instance->c_MainFileMapping, &instance->cMainQueue);

							instance->c_MainFileMapping = ad.fm;
							instance->cMainQueue = *ad.queue;

							// set new decoder
							instance->c_decoder = decoder;
							instance->c_nFormat = ad.format;
							instance->c_param1 = ad.format;
							instance->c_param2 = 0;
							instance->c_param3 = 0;
							instance->c_param4 = 0;

							continue;
						}
					}
				}
			

				// play rest of data from queue
				instance->c_DecoderData.nSamplesNum = 0;
				_PlayingLoop(instance, 1);
				
				
			}
			break;

			case DECODER_MANAGED_NEED_MORE_DATA:
			{
				
				// send callback message
				ResetEvent(instance->c_hStreamEvent);
				if(instance->c_DecoderData.nBufferDone)
					send_buffer_done(instance, instance->c_DecoderData.nBufferInQueue, instance->c_DecoderData.nBytesInQueue);
				
				int callback_ret = send_stream_out_of_data(instance);
				if(callback_ret == 0)
				{
					WaitForSingleObject(instance->c_hStreamEvent, INFINITE);
					continue;
				}

				if(callback_ret == 2)
					continue;
			}
			break;
		}
		break;
	}


	// wait for all buffers to end
	if(instance->c_HWO_WaveOut)
	{
		int i;
		for(i = 0; i < (int) instance->c_nNumberOfWaveBuffers; i++ )
		{
			if((instance->c_WH_WaveHeader[i].dwFlags == 0) || ((instance->c_WH_WaveHeader[i].dwFlags & WHDR_DONE) == WHDR_DONE))
			{
				// buffer is done with playing, unprepare
				EnterCriticalSection(&instance->c_CS_waveOut);
				waveOutUnprepareHeader(instance->c_HWO_WaveOut, &instance->c_WH_WaveHeader[i], sizeof(WAVEHDR));
				instance->c_WH_WaveHeader[i].dwFlags = 0;
				LeaveCriticalSection(&instance->c_CS_waveOut);
			}
			else
			{
				// wait for event
				WaitForSingleObject(instance->c_hEvent_WOM_DONE, SINGLE_AUDIO_BUFFER_SIZE);

				i--; // try again
			}
		}

		// close wave out
		HWAVEOUT c_HWO_WaveOut_tmp = instance->c_HWO_WaveOut;
		EnterCriticalSection(&instance->c_CS_waveOut);
		instance->c_HWO_WaveOut = 0;
		waveOutClose(c_HWO_WaveOut_tmp);
		LeaveCriticalSection(&instance->c_CS_waveOut);
		
	}


	if(instance->c_nFadeSemaphore)
		_SendFadeExit(instance);



	// send callback messages
	if(instance->c_fLoopEnterSent == 1 && instance->c_fLoopExitSent == 0)
		_SendLoopExit(instance);



	instance->c_fPlay = 0;
	instance->c_fPause = 0;
	instance->c_fLoop = 0;

	instance->c_fCheckFade = 0;
	instance->c_fFade = 0;


	// seek decoder to start position

	instance->c_decoder->Seek(0);

	instance->c_nCurrentPosition = 0;
	// clear audio processor chain
	instance->c_InputProcessor.Clear(1);

	// position calculation
	instance->c_nBytesIn = 0;
	// number of output bytes
	instance->c_nBytesOut = 0;
	// real bytes in and out
	instance->c_nRealBytesIn = 0;

// ========================================



	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);


	send_stop(instance);
	return 1;	
}


int __stdcall WMp3x::EnableEcho(int fEnable)
{
	err(WMP3X_NO_ERROR);
	if(c_EchoProcessor.Enable(0, fEnable) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}

int __stdcall WMp3x::SetEchoParam(TEchoEffect *pEchoEffect, int nNumberOfEffects)
{
	err(WMP3X_NO_ERROR);
	if(c_EchoProcessor.SetParameters((ECHO_DELAY_EFFECT*) pEchoEffect, nNumberOfEffects) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}

int __stdcall WMp3x::GetEchoParam(TEchoEffect *pEchoEffect, int nNumberOfEffects)
{
	err(WMP3X_NO_ERROR);
	int num = c_EchoProcessor.GetParameters((ECHO_DELAY_EFFECT*) pEchoEffect, nNumberOfEffects);

	if(num == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return num;
}




// =========================================================================================================================




int __stdcall WMp3x::SetRate(unsigned int nRate)
{
	err(WMP3X_NO_ERROR);

	if(nRate == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(nRate != 100)
		c_SoundTouchProcessor.Enable(0, 1);
	else if(c_SoundTouchProcessor.GetPitch() == 100 && c_SoundTouchProcessor.GetTempo() == 100)
		c_SoundTouchProcessor.Enable(0, 0);	

	c_SoundTouchProcessor.SetRate(nRate);
	return 1;
}



int __stdcall WMp3x::SetPitch(unsigned int nPitch)
{
	err(WMP3X_NO_ERROR);


	if(nPitch == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(nPitch != 100)
		c_SoundTouchProcessor.Enable(0, 1);
	else if(c_SoundTouchProcessor.GetRate() == 100 && c_SoundTouchProcessor.GetTempo() == 100)
		c_SoundTouchProcessor.Enable(0, 0);

	
	c_SoundTouchProcessor.SetPitch(nPitch);
	return 1;
}

int __stdcall WMp3x::SetTempo(unsigned int nTempo)
{
	err(WMP3X_NO_ERROR);

	if(nTempo == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}



	if(nTempo != 100)
		c_SoundTouchProcessor.Enable(0, 1);
	else if(c_SoundTouchProcessor.GetRate() == 100 && c_SoundTouchProcessor.GetPitch() == 100)
		c_SoundTouchProcessor.Enable(0, 0);

	c_SoundTouchProcessor.SetTempo(nTempo);

	
	return 1;
}

unsigned int __stdcall WMp3x::GetRate()
{
	
	return c_SoundTouchProcessor.GetRate();
}

unsigned int __stdcall WMp3x::GetPitch()
{
	return c_SoundTouchProcessor.GetPitch();
}

unsigned int __stdcall WMp3x::GetTempo()
{
	return c_SoundTouchProcessor.GetTempo();
}



int WMp3x::_Open(WMp3x* instance, int nWaveBufferLengthMs)
{
	ASSERT_W(instance->c_decoder);
	EnterCriticalSection(&instance->c_CS_FFT);

// ===========================================================
	// get stream info
	instance->c_isInfo = instance->c_decoder->GetStreamInfo();


// ===================================================================================
// EQUALIZER + CHAIN CONFIGURATION

	if(instance->c_InputProcessor.Configure(1, instance->c_isInfo->nSampleRate, 2, 16) == 0)
	{
		instance->err(WMP3X_UNKNOWN_ERROR);
		LeaveCriticalSection(&instance->c_CS_FFT);
		return 0;
	}

// =========================================================================================
//	ALLOCATE VU BUFFERS

	if(_AllocateVUBuffers(instance) == 0)
	{
		instance->err(WMP3X_MEMORY_ALLOCATION_FAIL);
		LeaveCriticalSection(&instance->c_CS_FFT);
		return 0;
	}

// ===========================================================
//	INITIALIZE WAVE OUTPUT, WE ALWAYS USE 16 bit per sample

	instance->c_fUseWaveOut = 1;
	if(nWaveBufferLengthMs == 0) // if wave buffer is 0, don't use wave out device
	{
		nWaveBufferLengthMs = 500;
		instance->c_fUseWaveOut = 0;
	}

	// check buffer length, we need at least 100 ms audio output buffer,
    if(instance->c_fUseWaveOut != 0 && nWaveBufferLengthMs < 100 )
	{
		_FreeVUBuffers(instance);
		instance->err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		LeaveCriticalSection(&instance->c_CS_FFT);
		return 0;	
    }

	// now, divide wave buffer length into number of small wave buffers
	instance->c_nNumberOfWaveBuffers = MulDiv(nWaveBufferLengthMs, 1, SINGLE_AUDIO_BUFFER_SIZE);
	

	instance->c_nSamplesForOneBuffer = SINGLE_AUDIO_BUFFER_SIZE * instance->c_isInfo->nSampleRate / 1000;

	instance->c_nBytesForOneBuffer = instance->c_nSamplesForOneBuffer * 4;


	// allocate memory for working wave buffer
	instance->c_pchWorkingWaveBuffer = (char*) malloc(instance->c_nBytesForOneBuffer);
	if(instance->c_pchWorkingWaveBuffer == 0)
	{
		_FreeVUBuffers(instance);
		instance->err(WMP3X_MEMORY_ALLOCATION_FAIL);
		LeaveCriticalSection(&instance->c_CS_FFT);
		return 0;
	}


	// create event, use this event to detect when wave buffer is done with playing
	instance->c_hEvent_WOM_DONE = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(instance->c_hEvent_WOM_DONE == 0)
	{
		_FreeVUBuffers(instance);
		instance->err(WMP3X_EVENT_CREATION_FAIL);
		LeaveCriticalSection(&instance->c_CS_FFT);
		return 0;
	}
	
	// allocate WAVEHDR array
	instance->c_WH_WaveHeader = (WAVEHDR*) malloc(sizeof(WAVEHDR) * instance->c_nNumberOfWaveBuffers);
	if(instance->c_WH_WaveHeader == 0)
	{
		CloseHandle(instance->c_hEvent_WOM_DONE);
		instance->c_hEvent_WOM_DONE = 0;
		_FreeVUBuffers(instance);
		instance->err(WMP3X_MEMORY_ALLOCATION_FAIL);
		LeaveCriticalSection(&instance->c_CS_FFT);	
		return 0;
	}
	// set all to 0
	memset(instance->c_WH_WaveHeader, 0, sizeof(WAVEHDR) * instance->c_nNumberOfWaveBuffers);

	// allocate buffers
	unsigned int i;
	for(i = 0; i < instance->c_nNumberOfWaveBuffers; i++)
	{
		instance->c_WH_WaveHeader[i].lpData = (char*) malloc(instance->c_nBytesForOneBuffer + sizeof(WAVE_USER_DATA));
		
		if(instance->c_WH_WaveHeader[i].lpData == 0)
		{
			// memory allocation error, free all allocated memory
			for(i = 0; i < instance->c_nNumberOfWaveBuffers; i++)
			{
				if(instance->c_WH_WaveHeader[i].lpData)
					free(instance->c_WH_WaveHeader[i].lpData );
			}

			free(instance->c_WH_WaveHeader);
			instance->c_WH_WaveHeader = 0;

			_FreeVUBuffers(instance);
			instance->err(WMP3X_MEMORY_ALLOCATION_FAIL);
			LeaveCriticalSection(&instance->c_CS_FFT);	
			return 0;
		}


		WAVE_USER_DATA *data = (WAVE_USER_DATA*) (instance->c_WH_WaveHeader[i].lpData + instance->c_nBytesForOneBuffer);
		data->nIndex = i;
		instance->c_WH_WaveHeader[i].dwUser = (unsigned long) data;
	}

	// set all buffers data to 0
	for(i = 0; i < instance->c_nNumberOfWaveBuffers; i++)
	{
		memset(instance->c_WH_WaveHeader[i].lpData, 0,  instance->c_nBytesForOneBuffer);
		
	}



	instance->c_wfx.cbSize = 0;
	instance->c_wfx.wFormatTag = WAVE_FORMAT_PCM;
    instance->c_wfx.nChannels = 2;
    instance->c_wfx.nSamplesPerSec = instance->c_isInfo->nSampleRate;
    instance->c_wfx.nAvgBytesPerSec = instance->c_isInfo->nSampleRate * 4;
    instance->c_wfx.nBlockAlign = 4;
    instance->c_wfx.wBitsPerSample = 16;
	instance->c_nWaveBufferSize = instance->c_nSamplesForOneBuffer * instance->c_nNumberOfWaveBuffers;

// ======================================================================================================
// INTIALIZE FADE
	instance->c_fCheckFade = 0;
	instance->c_fFade = 0;
// ========================================================
//	SEEK TO START

	instance->c_decoder->Seek(0);

	instance->c_nCurrentPosition = 0;

	instance->c_nSongIndex = 0;
	instance->c_nOldSongIndex = 0;
	instance->c_nCurrentSongIndex = 0;


// ========================================================
// initialize play and pause indicators
	instance->c_fPause = 0;
	instance->c_fPlay = 0;
// ========================================================
//	WE ARE READY TO PLAY
	instance->c_fReady = 1;

	LeaveCriticalSection(&instance->c_CS_FFT);
    return 1;	
}




// -----------------------------------------------------------------------------------------------------------------


int __stdcall WMp3x::StereoCut(int fEnable, int fOutputCenter, int fBassToSides)
{
	if(fEnable)
	{
		c_nSideCut = fOutputCenter;
		if(c_CenterCutProcessor.SetParameters(fOutputCenter, fBassToSides) == 0)
		{
			err(WMP3X_UNKNOWN_ERROR);
			return 0;
		}
	}

	if(c_CenterCutProcessor.Enable(0, fEnable) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	} 

	return 1;
}




TStreamFormat __stdcall WMp3x::GetFileFormat(const char *pchFileName)
{
	err(WMP3X_NO_ERROR);

	if(pchFileName == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return sfUnknown;
	}


	wchar_t *filename = ANSIToUTF16((char*) pchFileName, -1);
	if(filename == NULL)
	{
		err(WMP3X_MEMORY_ALLOCATION_FAIL);
		return sfUnknown;
	}

	TStreamFormat format = GetFileFormatW(filename);
	free(filename);
	return format;



}

TStreamFormat __stdcall WMp3x::GetFileFormatW(const wchar_t *pchFileName)
{
	err(WMP3X_NO_ERROR);

	if(pchFileName == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return sfUnknown;
	}


// check special formats

	if(wcsncmp(pchFileName, L"wavein://", 9) == 0)
	{

		// delete custom mixer line id
		if(c_user_mixer_line)
		{
			free(c_user_mixer_line);
			c_user_mixer_line = NULL;
		}

		TStreamFormat format = sfUnknown;

		char fn[MAX_PATH + 1];
		fn[0] = 0;

		if(WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, pchFileName, -1, fn, MAX_PATH, NULL, NULL) == 0)
		{
			err(WMP3X_UNKNOWN_ERROR);
			return sfUnknown;
		}

		fn[MAX_PATH] = 0;

	
		// parse input line

		char *start = fn + 9;
		char *name = NULL;
		char *value = NULL;
		int found = 0;
		c_user_mixer_line_id = WAVE_IN_NUM;
		c_user_mixer_volume = 200; // set to 200 because this value will not change volume in WaveIn

		char *token = strtok(start, ";");
	
	
		while(token)
		{
			name = token;
			char *end = strchr(token, '=');
			if(end)
			{
				*end = 0;
				value = end + 1;

				// parse source
				if(stricmp(name, "src") == 0)
				{
					unsigned int i;
					for(i = 0; i < WAVE_IN_NUM; i++)
					{
						if(stricmp(value, g_wavein_id[i].string_id) == 0)
						{
							c_user_mixer_line_id = i;
							found = 1;
							break;

						}
					}
				}
				else if(stricmp(name, "name") == 0) // parse user name
				{
					if(*value == '(')
					{
						value = value + 1;
						char *end1 = strchr(value, ')');
						if(end1)
						{	
							*end1 = 0;
							c_user_mixer_line = strdup(value);
						}
					}
							
				}
				else if(stricmp(name, "volume") == 0) // parse user name
				{
					c_user_mixer_volume = atoi(value);
					
				}

			}
			
			token = strtok(NULL, ";");
		}

		
		if(found)
		{
			format = sfWaveIn;
		}
		else
		{
			if(c_user_mixer_line_id)
			{
				format = sfWaveIn;
			}
		}


		if(format == sfUnknown)
			err(WMP3X_UNKNOWN_FILE_FORMAT);

		return format;
	
	}


		// parse filename and try to get file format using filename extension
	wchar_t *ext = wcsrchr((wchar_t*) pchFileName, L'.');
	if(ext)
	{
		ext++;


#ifndef LIBZPLAY_PF_VERSION
		if(_wcsicmp(ext, L"mp3") == 0)
			return sfMp3;

		if(_wcsicmp(ext, L"mp2") == 0)
			return sfMp3;

		if(_wcsicmp(ext, L"mp1") == 0)
			return sfMp3;

		if(_wcsicmp(ext, L"aac") == 0)
			return sfAacADTS;

#else

		if(_wcsicmp(ext, L"mp3") == 0 || _wcsicmp(ext, L"mp2") == 0 ||
			_wcsicmp(ext, L"mp1") == 0 || _wcsicmp(ext, L"aac") == 0)
		{
			err(WMP3X_UNSUPPORTED_FORMAT_PF_VERSION);
			return sfUnknown;
		}

#endif

		if(_wcsicmp(ext, L"ogg") == 0)
			return sfOgg;

		if(_wcsicmp(ext, L"wav") == 0)
			return sfWav;

		if(_wcsicmp(ext, L"flac") == 0)
			return sfFLAC;

		if(_wcsicmp(ext, L"oga") == 0)
			return sfFLACOgg;

		if(_wcsicmp(ext, L"ac3") == 0)
			return sfAC3;



	}

	// failed to determine file format using filename extension

	// check if file has ogg mark

	// open file
	HANDLE hFile = CreateFileW(pchFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);	
	if(hFile ==  INVALID_HANDLE_VALUE)
	{
		err(WMP3X_FILE_OPEN_ERROR);
        return sfUnknown;
	}	
	
		// get file size
	unsigned int nStreamSize = GetFileSize(hFile,NULL);
	if(nStreamSize == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(WMP3X_FILE_OPEN_ERROR);
        return sfUnknown;
	}

	char tmp[4];
	DWORD dwRead;


// ====================================================================
//	CHECK IG THIS IS FLAC OGG FILE
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(WMP3X_FILE_OPEN_ERROR);
		return sfUnknown;
	}


	// check if file can be ogg, check first 4 bytes for OggS identification.
	if(ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if(strncmp(tmp, "OggS", 4) == 0)
		{
			// this can be ogg file, check if this is FLAC OGG
			if(SetFilePointer(hFile, 29, NULL, FILE_BEGIN) != 0xFFFFFFFF)
			{
				if(ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
				{
					if(strncmp(tmp, "FLAC", 4) == 0)
					{
						CloseHandle(hFile);
						return sfFLACOgg;
					}
				}		
			}	
		}
	}

// ====================================================================
//	CHECK IF THIS IS OGG FILE
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(WMP3X_FILE_OPEN_ERROR);
		return sfUnknown;
	}

	// check if file can be ogg, check first 4 bytes for OggS identification.
	if(ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if(strncmp(tmp, "OggS", 4) == 0)
		{
			// this can be ogg file
			CloseHandle(hFile);
			return sfOgg;
		}
	}

// ====================================================================
//	CHECK IG THIS IS WAVE FILE WITH RIFF HEADER
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(WMP3X_FILE_OPEN_ERROR);
		return sfUnknown;
	}

	// check if file can be ogg, check first 4 bytes for OggS identification.
	if(ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if(strncmp(tmp, "RIFF", 4) == 0)
		{
			// this can be ogg file
			CloseHandle(hFile);
			return sfWav;
		}
	}


// ====================================================================
//	CHECK IG THIS IS FLAC FILE
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		err(WMP3X_FILE_OPEN_ERROR);
		return sfUnknown;
	}

	// check if file can be ogg, check first 4 bytes for OggS identification.
	if(ReadFile(hFile, tmp, 4, &dwRead, NULL) && dwRead == 4)
	{
		if(strncmp(tmp, "fLaC", 4) == 0)
		{
			// this can be ogg file
			CloseHandle(hFile);
			return sfFLAC;
		}
	}


// =================================================




	CloseHandle(hFile);
	err(WMP3X_UNKNOWN_FILE_FORMAT);
	return sfUnknown;
}




int WMp3x::_PlayingLoop(WMp3x *instance, unsigned int fFlush)
{
// Use all data from chain and play this data, when thsi function returns PLAY_OK chain is empty
//
//	PARAMETERS:
//		instance
//			Pointer to class instance.
//
//		flush
//			Flush buffer. Don't ask for more data.
//
//	RETURN VALUES:
//		PLAY_FATAL_ERROR	- fatal error
//		PLAY_USER_STOP		- user stop
//		PLAY_OK				- all OK
	

	if(instance->c_fRunPlayingLoop == 0)
		return PLAY_OK;


	if(fFlush == 0)
	{
	
	// ===================================================================================================
	//	FADE VOLUME PROCESSING
	// ===================================================================================================
		// trigger fade effect on starting position

		EnterCriticalSection(&instance->c_csFade);


		// fade effect is waiting, check for fade position
		if(instance->c_fCheckFade == 1)
		{
			// check if fade starting position is past
			if(instance->c_DecoderData.nStartPosition >= instance->c_nFadeStartingPosition)
			{
				// activate fade in effect
				instance->c_fFade = 1;
				// prevent from multiple activation of fade effect
				instance->c_nFadeStartingPosition = 0xFFFFFFFF;
				// don't check anymore
				instance->c_fCheckFade = 0;	

				instance->c_nFadeFrameCount = 0;

				if(instance->c_nFadeSemaphore)
				{
					_SendFadeExit(instance);	// send fade exit because we are breaking previous fade
					instance->c_nFadeSemaphore--;	
				}	
			}
		}

		// fade processing
		if(instance->c_fFade == 1)
		{	
			
			// send fade callback message
			int callback_ret;
				
			if(instance->c_nFadeSemaphore == 0)
			{
				callback_ret  = _SendFadeEnter(instance);
				instance->c_nFadeSemaphore++;
			}

			if(callback_ret == 1 || instance->c_fFade == 0)
			{
				// user request to stop fade effect or new fade effect is set

				if(instance->c_nFadeSemaphore)
				{
					_SendFadeExit(instance);	// send fade exit code
					instance->c_nFadeSemaphore--;
				}

				instance->c_fFade = 0; // disable fade
			}
			else
			{
				// check frame counter
				if( instance->c_nFadeFrameCount < instance->c_nFadeFrameNum) 
				{ 
					// still in fade interval, change volume
					unsigned int nLeftVolume;
					unsigned int nRightVolume;

					if(instance->c_fFadeVolumeLeftUp)
						nLeftVolume = instance->c_nFadeStartVolumeLeft + instance->c_nFadeFrameCount * instance->c_dFadeStepLeftChannel;
					else
						nLeftVolume = instance->c_nFadeStartVolumeLeft - instance->c_nFadeFrameCount * instance->c_dFadeStepLeftChannel;

					if(instance->c_fFadeVolumeRightUp)
						nRightVolume = instance->c_nFadeStartVolumeRight + instance->c_nFadeFrameCount * instance->c_dFadeStepRightChannel;
					else
						nRightVolume = instance->c_nFadeStartVolumeRight - instance->c_nFadeFrameCount * instance->c_dFadeStepRightChannel;

					_SetPlayerVolume(instance, nLeftVolume, nRightVolume);

					instance->c_nFadeFrameCount++; // increase fade counter

				}
				else
				{
					instance->c_fFade = 0;	// disable fade
					// we are out of fade interval
					if(instance->c_nFadeSemaphore)
					{
						_SendFadeExit(instance);	// send fade exit code
						instance->c_nFadeSemaphore--;
					}
					
				}	
			}	
		}

		LeaveCriticalSection(&instance->c_csFade);
		// =======================================================================================
		//	LOOP PROCESSING
		// =======================================================================================

		// check loop parameters
		if(instance->c_fLoop)
		{
			_SendLoopEnter(instance);

			if(instance->c_nLoopSampleCounter >= instance->c_nLoopLength)
			{
				// loop is done, check if we need to repeat loop
				if(instance->c_nLoopCycleCounter + 1 < instance->c_nLoopCycleNumber)
				{
					EnterCriticalSection(&instance->c_CS_waveOut);
					if(instance->c_fRunPlayingLoop == 0)
					{
						LeaveCriticalSection(&instance->c_CS_waveOut);
						return PLAY_OK;
					}
					// we need to repeat loop
					// seek position loop starting position
					if(instance->c_decoder->Seek(instance->c_nLoopStartingPosition) == 0)
					{
						LeaveCriticalSection(&instance->c_CS_waveOut);
						return PLAY_FATAL_ERROR;
					}

				

		
					instance->c_nLoopCycleCounter++;
					instance->c_nLoopSampleCounter = 0;
					// clear audio processor chain
					instance->c_InputProcessor.Clear(1);

					// set current wave buffer
					instance->c_pchCurrentWaveBuffer = instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].lpData;
					// set number of bytes we need to fill current wave buffer
					instance->c_nHaveBytesInWaveBuffer = 0;
					instance->c_nRealBytesHaveInWaveBuffer = 0;
					
					LeaveCriticalSection(&instance->c_CS_waveOut);
					return PLAY_OK;
				}

				// end of loop
				_SendLoopExit(instance);

				// stop loop processing
				instance->c_fLoop = 0;

				instance->c_nLoopCycleCounter++;
				

				// if not flying loop, stop playing
				if(instance->c_fFlyingLoop == 0)
					return PLAY_USER_STOP;
			


				instance->c_nLoopLength = 0xFFFFFFFF;
					
			}

			if(instance->c_nLoopSampleCounter + ( instance->c_DecoderData.nSamplesNum ) > instance->c_nLoopLength)
				instance->c_DecoderData.nSamplesNum =  instance->c_nLoopLength - instance->c_nLoopSampleCounter;

			instance->c_nLoopSampleCounter +=  instance->c_DecoderData.nSamplesNum;
						

		}
	}



	// ======================================================================
	// PROCESSING CHAIN
	// send data into audio processors chain

	EnterCriticalSection(&instance->c_CS_waveOut);
	if(instance->c_fRunPlayingLoop == 0)
	{
		LeaveCriticalSection(&instance->c_CS_waveOut);
		return PLAY_OK;
	}

	PROCESSOR_AUDIO_DATA data;
	data.nSongIndex = instance->c_DecoderData.nSongIndex;

	if(fFlush)
		instance->c_InputProcessor.Flush(1);
	else
	{
		data.pSamples = instance->c_DecoderData.pSamples;
		data.nNumberOfSamples = instance->c_DecoderData.nSamplesNum;
		data.nBlockAllign = 4;
		data.nStartPosition = instance->c_DecoderData.nStartPosition;
		data.nEndPosition = instance->c_DecoderData.nEndPosition;
		data.fReverse = instance->c_DecoderData.fReverse; 
		

		if(instance->c_InputProcessor.PushSamples(&data) == 0)	
		{
			LeaveCriticalSection(&instance->c_CS_waveOut);
			return PLAY_FATAL_ERROR;
		}
	}


	

	LeaveCriticalSection(&instance->c_CS_waveOut);
		

	// get number of bytes we have in decoder buffer
	unsigned int nDecoderIn = instance->c_DecoderData.nSamplesNum * 4;

	// loop taking data from queue and play
	while(instance->c_fRunPlayingLoop)
	{
		// wait for next buffer
		while((instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].dwFlags != 0) && ((instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].dwFlags & WHDR_DONE) != WHDR_DONE))
		{
			WaitForSingleObject(instance->c_hEvent_WOM_DONE, INFINITE);
		}

		EnterCriticalSection(&instance->c_CS_waveOut);
		if(instance->c_fRunPlayingLoop == 0)
		{
			LeaveCriticalSection(&instance->c_CS_waveOut);
			return PLAY_OK;
		}



		// unprepare buffer
		if(instance->c_fUseWaveOut)
			waveOutUnprepareHeader(instance->c_HWO_WaveOut, &instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex], sizeof(WAVEHDR));


// === LOAD DATA INTO BUFFER ===========================================


		data.pSamples = instance->c_pchCurrentWaveBuffer + instance->c_nHaveBytesInWaveBuffer;
		data.nNumberOfSamples = (instance->c_nBytesForOneBuffer - instance->c_nHaveBytesInWaveBuffer) / 4;

		unsigned int nChainOut = instance->c_OutputProcessor.PullSamples(&data) * 4;

		if(instance->c_nOldSongIndex != instance->c_nCurrentSongIndex)
		{
			instance->c_nOldSongIndex = instance->c_nCurrentSongIndex;

			// set new info
			instance->c_isInfo = instance->c_decoder->GetStreamInfo();


			// break fade

			EnterCriticalSection(&instance->c_csFade);
			instance->c_nFadeFrameCount = 0;
			// start checking for starting position
			instance->c_fCheckFade = 0;
			// we are not in fade  yet.
			instance->c_fFade = 0;
			if(instance->c_nFadeSemaphore)
			{
				_SendFadeExit(instance);	// send fade exit code
				instance->c_nFadeSemaphore--;
			}

			LeaveCriticalSection(&instance->c_csFade);


			// send play callback messages
			if(instance->callback_messages & MsgNextSong)
				send_callback_message(instance, MsgNextSong, instance->c_nCurrentSongIndex, instance->c_queue_decoder.GetCount());

			// send async play callback message
			if(instance->callback_messages & MsgNextSongAsync)
				send_callback_message(instance, MsgNextSongAsync, instance->c_nCurrentSongIndex, instance->c_queue_decoder.GetCount());
						

		}



		instance->c_nHaveBytesInWaveBuffer += nChainOut;

		// check if this wave buffer is full
		if(instance->c_nHaveBytesInWaveBuffer < instance->c_nBytesForOneBuffer)
		{
			// out wave buffer isn't full, there is no data in chain, need more data from decoder
			if(fFlush) // user ask to flush
			{
				if(instance->c_nHaveBytesInWaveBuffer == 0)	// user ask for flush, no data in wave buffer, nothing to play	
				{
					LeaveCriticalSection(&instance->c_CS_waveOut);
					return PLAY_USER_STOP;
				}
			

				// user asked for flush, but we have data in wave buffer, so continue with this wave buffer
				nChainOut = 1;

			}
			else
			{
				// ask for more data
				instance->c_nRealBytesHaveInWaveBuffer += nDecoderIn;
				LeaveCriticalSection(&instance->c_CS_waveOut);
				return PLAY_OK;
			}
		}


		// ========================================================================
		// INTERNAL VOLUME CONTROL	
		if(instance->c_nLeftVolume != 100 || instance->c_nRightVolume != 100)
			_ChangeVolume(instance, (unsigned char*) instance->c_pchCurrentWaveBuffer, instance->c_nHaveBytesInWaveBuffer);
		// ========================================================================
		// MIX CHANNELS	
		if(instance->c_fMixChannels)
			_MixChannels(instance, (unsigned char*) instance->c_pchCurrentWaveBuffer, instance->c_nHaveBytesInWaveBuffer);
		// ========================================================================


		WAVE_USER_DATA *user_data = (WAVE_USER_DATA*) instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].dwUser;
		user_data->nStart = data.nStartPosition;
		user_data->nEnd =  data.nEndPosition;
		user_data->nSongIndex = data.nSongIndex;


		instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].dwFlags = 0;
		instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].dwLoops = 0;
		instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].dwBufferLength = instance->c_nHaveBytesInWaveBuffer;


		int fSendToSoundcard = instance->c_fUseWaveOut;

// ===============================================================
// save to file



		
	

// =============================================================



		// SEND CALLBACK MESSAGE
		if(instance->callback_messages & MsgWaveBuffer)
		{	
			// send callback message and get return value
			int return_value = send_callback_message(instance, MsgWaveBuffer, (unsigned int) instance->c_pchCurrentWaveBuffer, instance->c_nHaveBytesInWaveBuffer);
			// check return value
			if(return_value == 2)
			{
				LeaveCriticalSection(&instance->c_CS_waveOut);
				return PLAY_USER_STOP; // user request to stop
			}
			else if(return_value == 1) // don't send this buffer to soundcard, load data into next wave buffer
			{	
				instance->c_nCurrentSongIndex = data.nSongIndex;
				instance->c_nCurrentPosition = data.nEndPosition;
				fSendToSoundcard = 0;
			}	
		}
		

		// =======================================================================================
		//	SEND PCM OUTPUT TO SOUNDCARD
		// =======================================================================================


		if(fSendToSoundcard)
		{
			if(waveOutPrepareHeader(instance->c_HWO_WaveOut, &instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			{
				LeaveCriticalSection(&instance->c_CS_waveOut);	
				return PLAY_FATAL_ERROR;
			}
			

			if(waveOutWrite(instance->c_HWO_WaveOut, &instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			{
				waveOutUnprepareHeader(instance->c_HWO_WaveOut, &instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex], sizeof(WAVEHDR));
				LeaveCriticalSection(&instance->c_CS_waveOut);
				return PLAY_FATAL_ERROR;
			}
		}


		instance->c_nBytesIn += instance->c_nHaveBytesInWaveBuffer;
		instance->c_nRealBytesIn += instance->c_nRealBytesHaveInWaveBuffer;

		// get next index
		if(instance->c_nWaveBufferIndex + 1 >= instance->c_nNumberOfWaveBuffers)
			instance->c_nWaveBufferIndex = 0;
		else
			instance->c_nWaveBufferIndex++;

		
		// set current wave buffer
		instance->c_pchCurrentWaveBuffer = instance->c_WH_WaveHeader[instance->c_nWaveBufferIndex].lpData;
		// set number of bytes we need to fill current wave buffer
		instance->c_nHaveBytesInWaveBuffer = 0;
		instance->c_nRealBytesHaveInWaveBuffer = 0;

		LeaveCriticalSection(&instance->c_CS_waveOut);


		// continue loop, get next data from chain
	
	
	}

	return PLAY_OK;
}





int WMp3x::_AllocateVUBuffers(WMp3x *instance)
{

	unsigned int nSamplesForOneBuffer = VU_BUFFER_SIZE * instance->c_isInfo->nSampleRate / 1000;
	unsigned int nBytesForOneBuffer = nSamplesForOneBuffer * 2;
	// reallocate buffer if needed
	if(instance->c_nVUDataBufferSize != nBytesForOneBuffer)
	{
		if(instance->c_pchVUDataBufferLeft)
			free(instance->c_pchVUDataBufferLeft);

		if(instance->c_pchVUDataBufferRight)
			free(instance->c_pchVUDataBufferRight);

		instance->c_pchVUDataBufferLeft = 0;
		instance->c_pchVUDataBufferRight = 0;
		instance->c_nVUDataBufferSize = 0;
		instance->c_pchVUDataBufferLeft = (short*) malloc(nBytesForOneBuffer);
		if(instance->c_pchVUDataBufferLeft == 0)
			return 0;

		instance->c_pchVUDataBufferRight = (short*) malloc(nBytesForOneBuffer);
		if(instance->c_pchVUDataBufferRight == 0)
		{
			free(instance->c_pchVUDataBufferLeft);
			return 0;
		}

		instance->c_nVUDataBufferSize = nBytesForOneBuffer;
	}

	return 1;
}

void WMp3x::_FreeVUBuffers(WMp3x *instance)
{
	if(instance->c_pchVUDataBufferLeft)
		free(instance->c_pchVUDataBufferLeft);

	if(instance->c_pchVUDataBufferRight)
		free(instance->c_pchVUDataBufferRight);

	instance->c_pchVUDataBufferLeft = 0;
	instance->c_pchVUDataBufferRight = 0;
	instance->c_nVUDataBufferSize = 0;
}


void CALLBACK WMp3x::_WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2)
{
	WMp3x *instance = (WMp3x*) dwInstance;
	
 	switch(uMsg)
	{
	
    	case WOM_OPEN:
		
			instance->c_nPlayingIndex = 0;
			instance->c_nBytesOut = 0;
			instance->c_nBytesIn = 0;
			instance->c_nRealBytesIn = 0;
			
        break;

        case WOM_CLOSE:
		{
			instance->c_HWO_WaveOut = 0;
			instance->c_nPlayingIndex = 0;
			instance->c_nBytesOut = 0;
			instance->c_nBytesIn = 0;
			instance->c_nRealBytesIn = 0;
			::SetEvent(instance->c_hEvent_WOM_DONE);
			
		}
        break;
		

        case WOM_DONE:
		{

			WAVEHDR* header = (WAVEHDR *) dwParam1;
			WAVE_USER_DATA *data = (WAVE_USER_DATA*) header->dwUser;

			unsigned int nNewIndex = data->nIndex + 1;
			if(nNewIndex >= instance->c_nNumberOfWaveBuffers)
				nNewIndex = 0;

			instance->c_nPlayingIndex = nNewIndex;
			instance->c_nBytesOut += header->dwBufferLength;

			instance->c_nCurrentSongIndex = data->nSongIndex;
			instance->c_nCurrentPosition = data->nEnd;
			SetEvent(instance->c_hEvent_WOM_DONE);	

		}
        break;
    }
		
}




void WMp3x::err(unsigned int error_code)
{
	if(error_code > WMP3X_UNKNOWN_ERROR)
		error_code = WMP3X_UNKNOWN_ERROR;


	wcsncpy(c_error_messageW, g_error_strW[error_code], MAX_ERROR_MESSAGE);
	c_error_messageW[MAX_ERROR_MESSAGE] = 0;

	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, c_error_messageW, -1, c_error_messageA, MAX_ERROR_MESSAGE, NULL, NULL);
	c_error_messageA[MAX_ERROR_MESSAGE] = 0;
}


void WMp3x::err(DECODER_ERROR_MESSAGE *error)
{
	ASSERT_W(error);

	wcsncpy(c_error_messageW, error->errorW, MAX_ERROR_MESSAGE);
	c_error_messageW[MAX_ERROR_MESSAGE] = 0;

	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, c_error_messageW, -1, c_error_messageA, MAX_ERROR_MESSAGE, NULL, NULL);
	c_error_messageA[MAX_ERROR_MESSAGE] = 0;
}



int __stdcall WMp3x::SetEqualizerPreampGain(int nGain)
{
	err(WMP3X_NO_ERROR);
	if(c_EqualizerProcessor.SetPreampGain(nGain) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}

int __stdcall WMp3x::GetEqualizerPreampGain()
{
	err(WMP3X_NO_ERROR);
	return c_EqualizerProcessor.GetPreampGain();
}
 
int __stdcall WMp3x::SetEqualizerBandGain(int nBandIndex, int nGain)
{
	err(WMP3X_NO_ERROR);
	if(c_EqualizerProcessor.SetBandGain(nBandIndex, nGain) == 0)
	{
		err(WMP3X_UNKNOWN_ERROR);
		return 0;
	}

	return 1;
}

int __stdcall WMp3x::GetEqualizerBandGain(int nBandIndex)
{
	err(WMP3X_NO_ERROR);
	return c_EqualizerProcessor.GetBandGain(nBandIndex);

}


int __stdcall WMp3x::AddFile(const char *sFileName, TStreamFormat nFormat)
{
	err(WMP3X_NO_ERROR);

	if(sFileName == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}
	
	wchar_t *filename = ANSIToUTF16((char*) sFileName, -1);	
	if(filename == NULL)
	{
		err(WMP3X_FILE_OPEN_ERROR);
		return 0;
	}

	int ret = AddFileW(filename, nFormat);
	free(filename);
	return ret;


}

int __stdcall WMp3x::AddFileW(const wchar_t *sFileName, TStreamFormat nFormat)
{

	err(WMP3X_NO_ERROR);

	if(sFileName == 0)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	if(c_decoder == 0)
	{
		err(WMP3X_NO_DECODER_SELECTED);
		return 0;
	}

	if(c_fReady == 0)
	{
		return OpenFileW(sFileName, nFormat);
	}

	unsigned int param1 = c_param1;
	unsigned int param2 = c_param2;
	unsigned int param3 = c_param3;
	unsigned int param4 = c_param4;


	// detect stream format
	if(nFormat == sfAutodetect || nFormat == sfUnknown)
	{
		nFormat = GetFileFormatW(sFileName);
		if(nFormat == sfUnknown)
		{
			err(WMP3X_UNKNOWN_FILE_FORMAT);
			return 0;
		}

		param1 = 0;
		param2 = 0;
		param3 = 0;
		param4 = 0;
	}


	// get new decoder instance
	WAudioDecoder *decoder = _GetAudioDecoder(nFormat);
	// create decoder


	if(decoder == 0)
	{
		err(WMP3X_NO_DECODER_SELECTED);
		return 0;
	}

	// initialize decoder
	if(decoder->Initialize(param1, param2, param3, param4) == 0)
	{
		err(decoder->GetError());
		decoder->Release();
		return 0;
	}



	FILE_MAPPING fm;
	WQueue *queue = new WQueue();

	_InitFileMapping(&fm, queue, 0);


	if(_OpenFileMapping((wchar_t*) sFileName, &fm, queue) == 0)
	{
		err(WMP3X_FILE_OPEN_ERROR);
		decoder->Release();
		delete queue;
        return 0;

	}


	// open decoder, lite, third parameter is 1 
	if(decoder->OpenStream(queue, 0, 0, 0) == 0)
	{
		decoder->Release();
		delete queue;
		_CloseFileMapping(&fm, queue);
		err(WMP3X_FILE_OPEN_ERROR);
        return 0;
	}



	INPUT_STREAM_INFO *info = decoder->GetStreamInfo();
	if(info == NULL)
	{
		err(decoder->GetError());
		decoder->Close();
		decoder->Release();
		delete queue;
		_CloseFileMapping(&fm, queue);
		return 0;
	}

	// seek to song start
	if(decoder->Seek(0) == 0)
	{
		err(decoder->GetError());
		decoder->Close();
		decoder->Release();
		delete queue;
		_CloseFileMapping(&fm, queue);
		return 0;
	}


	if(c_isInfo->nSampleRate != info->nSampleRate)
	{

		err(WMP3X_SAMPLERATE_NOT_MATCHING);
		decoder->Close();
		decoder->Release();
		delete queue;
		_CloseFileMapping(&fm, queue);
		return 0;
	}

	// add this new decoder to queue
	AUDIO_DECODER ad;
	ad.decoder = decoder;
	ad.format = nFormat;
	ad.queue = queue;
	ad.fm = fm;


	if(c_queue_decoder.PushLast(&ad, sizeof(AUDIO_DECODER)) == 0)
	{
		err(WMP3X_MEMORY_ALLOCATION_FAIL);
		decoder->Close();
		decoder->Release();
		return 0;
	}

	return 1;

}


WAudioDecoder *WMp3x::_GetAudioDecoder(TStreamFormat nFormat)
{

	switch(nFormat)
	{
		case sfMp3:
			return CreateMp3Decoder();
		break;
	}

	return NULL;
}

int __stdcall WMp3x::SetSettings(TSettingID nSettingID, int nValue)
{
	switch(nSettingID)
	{
		case sidWaveInBufferSize:
		{
			Close();
			c_nWaveInBufferSize = nValue;

		}
		break;
	

		case sidWaveBufferSize:
		{
			Close();	
			if(nValue > 0 && nValue < 100)
				nValue = 100;

			c_nWaveBufferLengthMs = nValue;

		}
		break;

		case sidAccurateLength:
		{
			Close();
			c_fAccurateLength = nValue;
			c_nFormat = sfUnknown;
		}
		break;

		case sidAccurateSeek:
		{
			Close();
			c_fAccurateSeek = nValue;
			c_nFormat = sfUnknown;
		}
		break;

		case sidSamplerate:
		{
			Close();
			c_nPCMSampleRate = nValue;

		}
		break;

		case sidChannelNumber:
		{
			Close();
			c_nPCMChannelNumber = nValue;

		}
		break;

		case sidBitPerSample:
		{
			Close();
			c_nPCMBitPerSample = nValue;

		}
		break;

		case sidBigEndian:
		{
			Close();
			c_nPCMBigEndian = nValue;

		}
		break;


		default:
		return 0;
	}

	return 1;
}


int __stdcall WMp3x::GetSettings(TSettingID nSettingID)
{
	switch(nSettingID)
	{

		case sidWaveInBufferSize:
		return c_nWaveInBufferSize;

		case sidWaveBufferSize:
		return c_nWaveBufferLengthMs;

		case sidAccurateLength:
		return c_fAccurateLength;

		case sidAccurateSeek:
		return c_fAccurateSeek;

		case sidSamplerate:
		return c_nPCMSampleRate;

		case sidChannelNumber:
		return c_nPCMChannelNumber;

		case sidBitPerSample:
		return c_nPCMBitPerSample;

		case sidBigEndian:
		return c_nPCMBigEndian;


		default:
		return 0;
	}
}


void ConvertFieldUTF16ToANSI(wchar_t *src)
{

	char *tmp =  UTF16ToANSI(src, -1);

	if(tmp)
	{
		strcpy((char*) src, tmp);
		free(tmp);
			
	}
	else
		src[0] = 0;
}


void ConvertID3UTF16ToANSI(TID3InfoExW *info)
{

	ConvertFieldUTF16ToANSI(info->Album);
	ConvertFieldUTF16ToANSI(info->AlbumArtist);
	ConvertFieldUTF16ToANSI(info->Artist);
	ConvertFieldUTF16ToANSI(info->Comment);
	ConvertFieldUTF16ToANSI(info->Composer);
	ConvertFieldUTF16ToANSI(info->Copyright);
	ConvertFieldUTF16ToANSI(info->Encoder);
	ConvertFieldUTF16ToANSI(info->Genre);
	ConvertFieldUTF16ToANSI(info->OriginalArtist);
	ConvertFieldUTF16ToANSI(info->Publisher);
	ConvertFieldUTF16ToANSI(info->Title);
	ConvertFieldUTF16ToANSI(info->TrackNum);
	ConvertFieldUTF16ToANSI(info->URL);
	ConvertFieldUTF16ToANSI(info->Year);
	ConvertFieldUTF16ToANSI(info->Picture.Description);
	ConvertFieldUTF16ToANSI(info->Picture.MIMEType);
	
}


int WMp3x::_CheckEncoderFormat(TStreamFormat nFormat)
{
#ifndef LIBZPLAY_PF_VERSION
	if(nFormat != sfWav && nFormat != sfFLAC && nFormat != sfFLACOgg && nFormat != sfOgg && nFormat != sfMp3 && nFormat != sfAacADTS  && nFormat != sfPCM)
	{
		err(WMP3X_UNSUPPORTED_SAVE_FORMAT);
		return 0;
	}
#else
	if(nFormat != sfWav && nFormat != sfFLAC && nFormat != sfFLACOgg && nFormat != sfOgg && nFormat != sfPCM)
	{
		err(WMP3X_UNSUPPORTED_SAVE_FORMAT);
		return 0;
	}
#endif

	return 1;
}


int __stdcall WMp3x::SetWaveOutFileW(const wchar_t *sFileName, TStreamFormat nFormat, unsigned int fOutputToSoundcard)
{
	err(WMP3X_NO_ERROR);

	if(c_fReady == 0)
	{
		err(WMP3X_STREAM_IS_CLOSED);
		return 0;
	}


	Stop(); // stop playing to kill decoding thread

	c_save_format = sfUnknown;

	if(c_save_filename)
	{
		free(c_save_filename);
		c_save_filename = NULL;
	}

	if(sFileName == NULL)
	{
		err(WMP3X_INVALID_FUNCTION_PARAMETERS);
		return 0;
	}

	// check supported format( only Wave format for now )

	if(_CheckEncoderFormat(nFormat) == 0)
	{
		err(WMP3X_UNSUPPORTED_SAVE_FORMAT);
		return 0;
	}


	c_save_format = nFormat;
	c_save_filename = _wcsdup(sFileName);

	if(c_save_filename == NULL)
	{

		c_save_format = sfUnknown;
		err(WMP3X_MEMORY_ALLOCATION_FAIL);
		return 0;
	}


	c_fUseWaveOut = fOutputToSoundcard;

	return 1;		
}


int  EncoderCallback(void *data, unsigned int data_size, void *user_data)
{
	HANDLE hFile = (HANDLE) user_data;
	DWORD out;
	WriteFile(hFile, data, data_size, &out, NULL); 
	return 1;
}


unsigned int  WMp3x::read_callback(void *data, unsigned int data_size, void *user_data)
{
	HANDLE hFile = (HANDLE) user_data;
	DWORD dwRead = 0;
	ReadFile(hFile, data, data_size, &dwRead, NULL);
	return dwRead;		

}


unsigned int  WMp3x::write_callback(void *data, unsigned int data_size, void *user_data)
{
	HANDLE hFile = (HANDLE) user_data;
	DWORD dwWrite = 0;
	WriteFile(hFile, data, data_size, &dwWrite, NULL);
	return dwWrite;

}

unsigned int  WMp3x::seek_callback(unsigned int offset, void *user_data)
{
	HANDLE hFile = (HANDLE) user_data;
	return SetFilePointer(hFile, offset, NULL, FILE_BEGIN);

}

unsigned int  WMp3x::tell_callback(void *user_data)
{
	HANDLE hFile = (HANDLE) user_data;
	return SetFilePointer(hFile, 0, NULL, FILE_CURRENT);


}
