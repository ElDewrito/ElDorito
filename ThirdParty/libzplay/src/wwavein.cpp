/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *  WaveIn
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
 * ver: 2.0
 * date: 24. April, 2010.
 *
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "debug.h"
#include "wwavein.h"

#ifdef _MSC_VER
#pragma comment( lib, "winmm")
#pragma comment( lib, "user32")
#pragma comment( lib, "gdi32")
#endif

#define AUDIO_BUFFER_SIZE 50


extern WAVEIN_ID g_wavein_id[WAVE_IN_NUM];

enum {
	DECODER_NO_ERROR = 0,
	DECODER_WAVE_INPUT_OPEN_ERROR,
	DECODER_CANT_SELECT_SOURCE,
	DECODER_WAVEBUFFER_TOO_SMALL,



	DECODER_FILE_OPEN_ERROR,
	DECODER_FILE_SEEK_POSITION_OUT_OF_BOUNDS,
	DECODER_FILESIZE_OUT_OF_BOUNDS,
	DECODER_NOT_VALID_PCM_STREAM,
	DECODER_UNSUPPORTED_SAMPLERATE,
	DECODER_UNSUPPORTED_CHANNEL_NUMBER,
	DECODER_UNSUPPORTED_BIT_PER_SAMPLE,
	DECODER_MEMORY_ALLOCATION_FAIL,
	DECODER_NO_ID3_DATA,
	DECODER_SEEK_NOT_SUPPORTED_MANAGED,
	DECODER_SEEK_POSITION_OUT_OF_BOUNDS,
	DECODER_REVERSE_NOT_SUPPORTED_MANAGED,
	DECODER_FUNCTION_NOT_SUPPORTED,


	DECODER_UNKNOWN_ERROR
};



wchar_t *g_recorder_error_strW[DECODER_UNKNOWN_ERROR + 1] = {
	L"WaveIn: No error.",
	L"WaveIn: Can't open wave input.",
	L"WaveIn: Can't select recording source.",
	L"WaveIN: Wave buffer too small.",

	L"WaveIn: File open error.",
	L"WaveIn: File sek position is out of bounds.",
	L"WaveIn: File size is out of bounds.",
	L"WaveIn: This is not valid PCM stream.",
	L"WaveIn: Unsupported sample rate.",
	L"WaveIn: Unsupported channel number.",
	L"WaveIn: Unsupported bits per sample.",
	L"WaveIn: Memory allocation fail."
	L"WaveIn: No ID3 data.",
	L"WaveIn: Seek is not supported on managed stream.",
	L"WaveIn: Seek position is out of bounds.",
	L"WaveIn: Reverse mod is not supported on managed stream.",
	L"WaveIn: Function not supported in this decoder.",
	L"WaveIn: Unknown error."

};

int SelectRecordingLine(unsigned int nMixerId, unsigned int nComponentID, char *user_mixer_line, unsigned int nLeftVolume, unsigned int nRightVolume);

WWaveIn::WWaveIn()
{
	err(DECODER_NO_ERROR);

	InitializeCriticalSection(&cs);

	c_hWaweIn = 0;
	c_InDeviceID = WAVE_MAPPER;
	c_user_mixer_line = NULL;
	c_nMaxLatencyInMilliseconds = 1000;

	c_line_control_id = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
	c_user_mixer_volume = 100;

	c_buffer1 = NULL;
	c_buffer2 = NULL;

	// set latency to 1 second
	c_nMaxQueueSizeInBytes = 44100  * 4;

	c_fWorking = 0;

	c_nInSampleRate = 44100;
	c_inChannelNum = 2;
	c_inBitPerSample = 16;
	c_nCurrentPosition = 0;
	c_fReady = 0;
// =================================================
//	INITIALIZE INPUT_STREAM_INFO
	memset(&c_isInfo, 0, sizeof(INPUT_STREAM_INFO));
// ==================================================

}


WWaveIn::~WWaveIn()
{
	Close();
	Uninitialize();
	DeleteCriticalSection(&cs);
}



int WWaveIn::Initialize(int param1, int param2, int param3, int param4)
{
	WAVEIN_INIT_DATA *data = (WAVEIN_INIT_DATA*) param1;

	if(data->nWaveBufferSize < AUDIO_BUFFER_SIZE)
	{
		err(DECODER_WAVEBUFFER_TOO_SMALL);
		return 0;
	}


	c_nMaxLatencyInMilliseconds = data->nWaveBufferSize;


	c_InDeviceID = data->nDeviceID;
	c_line_control_id = data->nMixerInputLineIndex;
	c_user_mixer_line = data->sMixerInputLineName;
	c_user_mixer_volume = data->nVolume;

	c_nInSampleRate = 44100;
	c_inChannelNum = 2;
	c_inBitPerSample = 16;



	return 1;
}


int WWaveIn::Uninitialize()
{
	return 1;
}


void WWaveIn::Release()
{
	delete this;
}

DECODER_ERROR_MESSAGE *WWaveIn::GetError()
{
	return &c_err_msg;
}





int WWaveIn::Close()
{

	c_fWorking = 0;
	c_user_mixer_line = NULL;

	if(c_fReady == 0)
		return 1;


	waveInReset(c_hWaweIn);
	waveInClose(c_hWaweIn);


	waveInUnprepareHeader(c_hWaweIn, &c_wavehdr[0], sizeof(WAVEHDR));
	waveInUnprepareHeader(c_hWaweIn, &c_wavehdr[1], sizeof(WAVEHDR));

	if(c_buffer1)
	{
		free(c_buffer1);
		c_buffer1 = NULL;
	}


	if(c_buffer2)
	{
		free(c_buffer2);
		c_buffer2 = NULL;
	}

	c_Queue.Clear();

// ================================

	//	INITIALIZE INPUT_STREAM_INFO
	memset(&c_isInfo, 0, sizeof(INPUT_STREAM_INFO));

// ==================================

	c_nCurrentPosition = 0;

	c_fReady = 0;

	return 1;
}



INPUT_STREAM_INFO *WWaveIn::GetStreamInfo()
{
	ASSERT_W(c_fReady);
	return &c_isInfo;
}


wchar_t **WWaveIn::GetID3Info(int version, char *pStream, unsigned int nStreamSize, int param1, int param2)
{
	ASSERT_W(c_fReady);
	err(DECODER_FUNCTION_NOT_SUPPORTED);
	return NULL;

}




int WWaveIn::Seek(unsigned int nSamples)
{
	ASSERT_W(c_fReady);
	err(DECODER_NO_ERROR);
	c_nCurrentPosition = 0;
	return 1;

}


int WWaveIn::GetData(DECODER_DATA *pDecoderData)
{
	ASSERT_W(c_fReady);
	ASSERT_W(pDecoderData);
	ASSERT_W(pDecoderData->pSamples);


	pDecoderData->nBufferDone = 0;
	pDecoderData->nBufferInQueue = 0;
	pDecoderData->nBytesInQueue = 0;

	
	unsigned int nSamplesNeed = pDecoderData->nSamplesNum;
	unsigned int nBytesNeed = pDecoderData->nSamplesNum * 4;
	unsigned int nBytesHave = 0;
	unsigned int nSamplesHave = nBytesHave / 4;


	
	unsigned int nWait = 0;
	while(nWait < 8)
	{
		EnterCriticalSection(&cs);
		nBytesHave = c_Queue.GetSizeSum();
		LeaveCriticalSection(&cs);
		nWait++;
		if(nBytesHave < nBytesNeed)
		{
			Sleep(AUDIO_BUFFER_SIZE / 4);
			continue;
		}

		break;
	}

	
	EnterCriticalSection(&cs);
	nBytesHave = c_Queue.GetSizeSum();
	LeaveCriticalSection(&cs);
	if(nBytesHave < nBytesNeed)
	{
		// return blank samples
		memset(pDecoderData->pSamples, 0, nBytesNeed);
		pDecoderData->nSamplesNum = nSamplesNeed;
	}
	else
	{	
		// get samples from queue
		int out;
		EnterCriticalSection(&cs);
		unsigned int ret = c_Queue.PullDataFifo(pDecoderData->pSamples, nBytesNeed, &out);
		LeaveCriticalSection(&cs); 

		if(ret == 0)
		{
			// return blank samples
			memset(pDecoderData->pSamples, 0, nBytesNeed);
			pDecoderData->nSamplesNum = nSamplesNeed;
		}
		else
			pDecoderData->nSamplesNum = ret / 4;
	}

	
	pDecoderData->nStartPosition = c_nCurrentPosition;
	c_nCurrentPosition += pDecoderData->nSamplesNum;
	pDecoderData->nEndPosition = c_nCurrentPosition;
	
	return DECODER_OK;

}



int WWaveIn::GetBitrate(int fAverage)
{
	return c_isInfo.nFileBitrate;
}


int WWaveIn::SetReverseMode(int fReverse)
{
	ASSERT_W(c_fReady);
	err(DECODER_REVERSE_NOT_SUPPORTED_MANAGED);
	return 0;
}








void WWaveIn::err(unsigned int error_code)
{
	if(error_code > DECODER_UNKNOWN_ERROR)
		error_code = DECODER_UNKNOWN_ERROR;
			
	c_err_msg.errorW = g_recorder_error_strW[error_code];
}




int WWaveIn::OpenStream(WQueue *pQueue, int fDynamic, int param1, int param2)
{
	err(DECODER_NO_ERROR);


	MMRESULT res;
	WAVEFORMATEX wfx;
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = c_inChannelNum;
    wfx.nSamplesPerSec = c_nInSampleRate;
    wfx.nAvgBytesPerSec = c_nInSampleRate * c_inBitPerSample / 8 * c_inChannelNum;
    wfx.nBlockAlign = c_inBitPerSample / 8 * c_inChannelNum;
    wfx.wBitsPerSample = c_inBitPerSample;


	// allocate working buffers

	if(c_buffer1)
	{
		free(c_buffer1);
		c_buffer1 = NULL;
	}

	if(c_buffer2)
	{
		free(c_buffer2);
		c_buffer2 = NULL;
	}


	c_nSamplesForOneBuffer = AUDIO_BUFFER_SIZE * c_nInSampleRate / 1000;
	c_nBytesForOneBuffer = c_nSamplesForOneBuffer * wfx.nBlockAlign;

	unsigned int nMaxQueueSizeInSamples = MulDiv(c_nMaxLatencyInMilliseconds, c_nInSampleRate, 1000);
	c_nMaxQueueSizeInBytes = nMaxQueueSizeInSamples  * wfx.nBlockAlign;

	c_buffer1 = (char*) malloc(c_nBytesForOneBuffer);
	if(c_buffer1 == NULL)
	{
		err(DECODER_MEMORY_ALLOCATION_FAIL);
		return 0;
	}

	c_buffer2 = (char*) malloc(c_nBytesForOneBuffer);
	if(c_buffer2 == NULL)
	{
		free(c_buffer1);
		c_buffer1 = NULL;
		err(DECODER_MEMORY_ALLOCATION_FAIL);
		return 0;
	}

	if( (res = waveInOpen(&c_hWaweIn, c_InDeviceID, &wfx, (DWORD) _WaveInProc, (DWORD) this, CALLBACK_FUNCTION)) != MMSYSERR_NOERROR)
	{
		err(DECODER_WAVE_INPUT_OPEN_ERROR);
		return 0;
	}





	if(SelectRecordingLine((unsigned int) c_hWaweIn, c_line_control_id, c_user_mixer_line, c_user_mixer_volume, c_user_mixer_volume) == 0)
	{
		// mixer is not working on Windows Vista and Windows 7, so let go. User can manualy select input.
		err(DECODER_WAVE_INPUT_OPEN_ERROR);
		c_line_control_id = WAVE_IN_NUM;
		c_user_mixer_line = NULL;
		//waveInClose(c_hWaweIn);
		//return 0;

	}


	c_wavehdr[0].lpData = c_buffer1;
	c_wavehdr[0].dwBufferLength = c_nBytesForOneBuffer;
	c_wavehdr[0].dwUser = (DWORD) this;
	c_wavehdr[0].dwFlags = 0;


	c_wavehdr[1].lpData = c_buffer2;
	c_wavehdr[1].dwBufferLength = c_nBytesForOneBuffer;
	c_wavehdr[1].dwUser = (DWORD) this;
	c_wavehdr[1].dwFlags = 0;


	if(waveInPrepareHeader(c_hWaweIn, &c_wavehdr[0], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
	{
		err(DECODER_WAVE_INPUT_OPEN_ERROR);
		if(c_buffer1)
		{
			free(c_buffer1);
			c_buffer1 = NULL;
		}

		if(c_buffer2)
		{
			free(c_buffer2);
			c_buffer2 = NULL;
		}

		waveInClose(c_hWaweIn);
		return 0;
	}

	if(waveInPrepareHeader(c_hWaweIn, &c_wavehdr[1], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
	{
		waveInUnprepareHeader(c_hWaweIn, &c_wavehdr[1], sizeof(WAVEHDR));
		err(DECODER_WAVE_INPUT_OPEN_ERROR);
		if(c_buffer1)
		{
			free(c_buffer1);
			c_buffer1 = NULL;
		}

		if(c_buffer2)
		{
			free(c_buffer2);
			c_buffer2 = NULL;
		}

		waveInClose(c_hWaweIn);
		return 0;
	}


	waveInAddBuffer(c_hWaweIn, &c_wavehdr[0], sizeof(WAVEHDR));
	waveInAddBuffer(c_hWaweIn, &c_wavehdr[1], sizeof(WAVEHDR));

	c_isInfo.fVbr = 0;
	c_isInfo.nBitPerSample = wfx.wBitsPerSample;
	c_isInfo.nBlockAlign = wfx.nBlockAlign;
	c_isInfo.nChannel = wfx.nChannels;
	c_isInfo.nFileBitrate = wfx.nAvgBytesPerSec * 8;
	c_isInfo.nLength = 0;
	c_isInfo.nSampleRate = wfx.nSamplesPerSec;


	if(c_user_mixer_line && c_user_mixer_line[0] != 0)
		c_isInfo.pchStreamDescription = c_user_mixer_line;
	else
	{
		if(c_line_control_id == WAVE_IN_NUM - 1)
		{
			if(c_user_mixer_line == NULL)
				c_isInfo.pchStreamDescription = "WaveIn://";
			else
				c_isInfo.pchStreamDescription = c_user_mixer_line;
		}
		else if(c_line_control_id < WAVE_IN_NUM)
			c_isInfo.pchStreamDescription = g_wavein_id[c_line_control_id].string_name1;
		else
			c_isInfo.pchStreamDescription = "WaveIn://unknown";	
	}




	
	c_fWorking = 1;

	if(waveInStart(c_hWaweIn) != MMSYSERR_NOERROR)
	{
		c_fWorking = 0;

		waveInUnprepareHeader(c_hWaweIn, &c_wavehdr[0], sizeof(WAVEHDR));
		waveInUnprepareHeader(c_hWaweIn, &c_wavehdr[1], sizeof(WAVEHDR));
		err(DECODER_WAVE_INPUT_OPEN_ERROR);
		if(c_buffer1)
		{
			free(c_buffer1);
			c_buffer1 = NULL;
		}

		if(c_buffer2)
		{
			free(c_buffer2);
			c_buffer2 = NULL;
		}

		waveInClose(c_hWaweIn);
		return 0;
	}

	c_nCurrentPosition = 0;


	c_fReady = 1;

	return 1;
}


void CALLBACK WWaveIn::_WaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2)
{
	WWaveIn *instance = (WWaveIn*) dwInstance;
	
 	switch(uMsg)
	{
	
    	case MM_WIM_OPEN:
        break;

        case MM_WIM_CLOSE:
        break;
		

        case MM_WIM_DATA:
		{
			if(instance->c_fWorking == 0)
				break;

			// add data to queue
			WAVEHDR* header = (WAVEHDR *) dwParam1;
			unsigned int nHaveRecorded = header->dwBytesRecorded;
			

			EnterCriticalSection(&instance->cs);
			// add new data to buffer
			if(nHaveRecorded)
				instance->c_Queue.PushLast(header->lpData, nHaveRecorded);

			unsigned int nHaveInQueue = instance->c_Queue.GetSizeSum();

			if(nHaveInQueue > instance->c_nMaxQueueSizeInBytes)
			{
				// need to cut
				instance->c_Queue.CutDataFifo(nHaveInQueue - instance->c_nMaxQueueSizeInBytes);
			}

			LeaveCriticalSection(&instance->cs);

		
			waveInUnprepareHeader(hwi, header, sizeof(WAVEHDR));
			header->dwFlags = 0;
			waveInPrepareHeader(hwi, header, sizeof(WAVEHDR));
			waveInAddBuffer(hwi, header, sizeof(WAVEHDR));
		



		}
        break;
    }
}



int SelectRecordingLine(unsigned int nMixerId, unsigned int nComponentID, char *user_mixer_line, unsigned int nLeftVolume, unsigned int nRightVolume)
{
	// check windows version because this can't work on VIsta and Windows 7
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osvi) == 0)
		return 1;	// don't fail, just exit function


	// check if we have Vista or WIndows /
	if(osvi.dwMajorVersion > 5)
		return 1;




	unsigned int dwVolumeControlID = 0;
	unsigned int dwControlType = 0;
	unsigned int dwSelectControlID = 0;
	unsigned int dwMultipleItems = 0;
	unsigned int dwIndex = 0xFFFFFFFF;
	HMIXER hMixer = NULL;
	MIXERCAPS mxcaps;
	::ZeroMemory(&mxcaps, sizeof(MIXERCAPS));

	if(nComponentID >= WAVE_IN_NUM)
		return 0;

	// open mixer

	if(mixerOpen(&hMixer, (unsigned int) nMixerId, 0, 0, MIXER_OBJECTF_HWAVEIN) != MMSYSERR_NOERROR)
		return 0;

	if(mixerGetDevCaps((unsigned int) hMixer, &mxcaps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR)
	{
		::mixerClose(hMixer);
		return 0;
	}


	// get control we need
	if (hMixer == NULL)
		return 0;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	if (mixerGetLineInfo((HMIXEROBJ) hMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		mixerClose(hMixer);
		return 0;
	}


	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = dwControlType;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (mixerGetLineControls((HMIXEROBJ) hMixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
	{
		// no mixer, try MUX
		dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		if (mixerGetLineControls((HMIXEROBJ) hMixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		{
			mixerClose(hMixer);
			return 0;
		}
	}


	// store dwControlID, cMultipleItems
	dwSelectControlID = mxc.dwControlID;
	dwMultipleItems = mxc.cMultipleItems;



	if (dwMultipleItems == 0)
	{
		mixerClose(hMixer);
		return 0;
	}


	DWORD dwControlID = 0;

	DWORD dwLine = 0;

	// get the index of the specified Select control
	MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText = new MIXERCONTROLDETAILS_LISTTEXT[dwMultipleItems];
	DWORD dwi = 0;
	if (pmxcdSelectText != NULL)
	{
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
		mxcd.paDetails = pmxcdSelectText;
		if (mixerGetControlDetails((HMIXEROBJ) hMixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT) == MMSYSERR_NOERROR)
		{
			for (dwi = 0; dwi < dwMultipleItems; dwi++)
			{
				// get the line information
				MIXERLINE mxl;
				mxl.cbStruct = sizeof(MIXERLINE);
				mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
				if (mixerGetLineInfo((HMIXEROBJ) hMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR
						&& mxl.dwComponentType == g_wavein_id[nComponentID].id)
				{
					// check specified user name
					if(user_mixer_line)
					{
						if(strcmp(user_mixer_line, mxl.szShortName) != 0 && strcmp(user_mixer_line, mxl.szName) != 0)
							continue;
					}

					// found, dwi is the index.
				
					dwIndex = dwi;
					dwLine = pmxcdSelectText[dwIndex].dwParam1;
					break;
				}
			}

			if (dwi >= dwMultipleItems)
			{

				if(user_mixer_line)
				{
					// could not find it using line IDs, some mixer drivers have
					// different meaning for MIXERCONTROLDETAILS_LISTTEXT.dwParam1.
					// let's try comparing the item names.
					for (dwi = 0; dwi < dwMultipleItems; dwi++)
					{
						if (stricmp(pmxcdSelectText[dwi].szName, user_mixer_line) == 0)
						{
							// found, dwi is the index.
							dwIndex = dwi;
							dwLine = pmxcdSelectText[dwIndex].dwParam1;
							break;
						}
					}


					// error
					if (dwi >= dwMultipleItems)
					{
						mixerClose(hMixer);
						delete []pmxcdSelectText;
						return 0;
					}

				}
			}


			if (dwi >= dwMultipleItems)
			{
				// could not find it using line IDs, some mixer drivers have
				// different meaning for MIXERCONTROLDETAILS_LISTTEXT.dwParam1.
				// let's try comparing the item names.
				for (dwi = 0; dwi < dwMultipleItems; dwi++)
				{
					if (stricmp(pmxcdSelectText[dwi].szName, g_wavein_id[nComponentID].string_name1) == 0)
					{
						// found, dwi is the index.
						dwIndex = dwi;
						dwLine = pmxcdSelectText[dwIndex].dwParam1;
						break;
					}
				}
			}


			if (dwi >= dwMultipleItems)
			{
				// could not find it using line IDs, some mixer drivers have
				// different meaning for MIXERCONTROLDETAILS_LISTTEXT.dwParam1.
				// let's try comparing the item names.
				for (dwi = 0; dwi < dwMultipleItems; dwi++)
				{
					if (stricmp(pmxcdSelectText[dwi].szName, g_wavein_id[nComponentID].string_name2) == 0)
					{
						// found, dwi is the index.
						dwIndex = dwi;
						dwLine = pmxcdSelectText[dwIndex].dwParam1;
						break;
					}
				}
			}

			if (dwi >= dwMultipleItems)
			{
				// could not find it using line IDs, some mixer drivers have
				// different meaning for MIXERCONTROLDETAILS_LISTTEXT.dwParam1.
				// let's try comparing the item names.
				for (dwi = 0; dwi < dwMultipleItems; dwi++)
				{
					if (stricmp(pmxcdSelectText[dwi].szName, g_wavein_id[nComponentID].string_name3) == 0)
					{
						// found, dwi is the index.
						dwIndex = dwi;
						dwLine = pmxcdSelectText[dwIndex].dwParam1;
						break;
					}
				}
			}
	
		}

		delete []pmxcdSelectText;
	}


	// select component

	if (dwIndex >= dwMultipleItems)
	{
		mixerClose(hMixer);
		return 0;
	}





	// get the line information


	int ret = 0;
	// get all the values first
	MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[dwMultipleItems];

	if (pmxcdSelectValue != NULL)
	{
		// select control
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mxcd.paDetails = pmxcdSelectValue;
		if (mixerGetControlDetails((HMIXEROBJ) hMixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
		{
			// MUX restricts the line selection to one source line at a time.
			if (dwControlType == MIXERCONTROL_CONTROLTYPE_MUX)
				ZeroMemory(pmxcdSelectValue, dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

			// set the value
			pmxcdSelectValue[dwIndex].fValue = 1;

			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			if (mixerSetControlDetails((HMIXEROBJ) hMixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
			{
				ret = 1;
			}
		}

		delete []pmxcdSelectValue;
	}


	// set volume, if volume is larger than 100 skip volume change

	if(nLeftVolume <= 100 && nRightVolume <= 100)
	{

		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwLineID = dwLine;
		if (mixerGetLineInfo((HMIXEROBJ) hMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR)
		{				
			mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
			mxlc.dwLineID = dwLine;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = &mxc;
			if (mixerGetLineControls((HMIXEROBJ) hMixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
			{	

				if(nLeftVolume > 100)
					nLeftVolume = 100;
				
				if(nRightVolume > 100)
					nRightVolume = 100;

				MIXERCONTROLDETAILS_UNSIGNED mxcdVolume[2] = { mxc.Bounds.dwMaximum * nLeftVolume / 100, mxc.Bounds.dwMaximum * nRightVolume / 100};
				MIXERCONTROLDETAILS mxcd;
				mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
				mxcd.dwControlID = mxc.dwControlID;
				mxcd.cChannels = mxl.cChannels;
				mxcd.cMultipleItems = 0;
				mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
				mxcd.paDetails = mxcdVolume;
				if (mixerSetControlDetails((HMIXEROBJ) hMixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
				{
					// error, can't set volume
				}
			}
			else
			{
				// error can't set volume
			}
		}
	}

	mixerClose(hMixer);
	return ret;
}
