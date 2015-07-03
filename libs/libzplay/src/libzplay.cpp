/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *
 * ver: 2.00
 * date: 22. April, 2010.
 *
 *  Copyright (C) 2003-2010 Zoran Cindori
 *
 *  Author: Zoran Cindori
 *  Web: http://libzplay.sourceforge.net/
 *  Email: zcindori@inet.hr
 *
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
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "wmp3x.h"

using namespace libZPlay;

// =============================================================================================================

ZPlay * __stdcall  libZPlay::CreateZPlay() 
{ 
    return new WMp3x(); 
}

// =============================================================================================================

// =============================================================================================================

ZPLAY_HANDLE __stdcall libZPlay::zplay_CreateZPlay()
{
	WMp3x* instance = new WMp3x; // create instance of WMp3 class
	return (ZPLAY_HANDLE) instance;
}

// =============================================================================================================

int __stdcall libZPlay::zplay_DestroyZPlay(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;
		
	delete instance;
	return 1;
}

// =============================================================================================================

int __stdcall libZPlay::zplay_GetVersion(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;
		
	return instance->GetVersion();
}

// =============================================================================================================

char * __stdcall libZPlay::zplay_GetError(ZPLAY_HANDLE handle)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetError();
}

// =============================================================================================================

wchar_t * __stdcall libZPlay::zplay_GetErrorW(ZPLAY_HANDLE handle)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetErrorW();
}


// =============================================================================================================

TStreamFormat __stdcall libZPlay::zplay_GetFileFormat(
							ZPLAY_HANDLE handle,
							const char *pchFileName)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return sfUnknown;

	return instance->GetFileFormat(pchFileName);

}

// =============================================================================================================

TStreamFormat __stdcall libZPlay::zplay_GetFileFormatW(
							ZPLAY_HANDLE handle,
							const wchar_t *pchFileName)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return sfUnknown;

	return instance->GetFileFormatW(pchFileName);

}



// =============================================================================================================

int __stdcall libZPlay::zplay_EnumerateWaveOut(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->EnumerateWaveOut();

}

// =============================================================================================================

int __stdcall libZPlay::zplay_GetWaveOutInfo(
			ZPLAY_HANDLE handle,
			unsigned int nIndex,			// index of wave out device
			TWaveOutInfo *pWaveOutInfo		// pointer to  WAVEOUT_INFOW structure
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetWaveOutInfo(nIndex, pWaveOutInfo);

}

// =============================================================================================================

int __stdcall libZPlay::zplay_GetWaveOutInfoW(
			ZPLAY_HANDLE handle,
			unsigned int nIndex,			// index of wave out device
			TWaveOutInfoW *pWaveOutInfo		// pointer to  WAVEOUT_INFOW structure
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetWaveOutInfoW(nIndex, pWaveOutInfo);

}
// =============================================================================================================

int __stdcall libZPlay::zplay_SetWaveOutDevice(
			ZPLAY_HANDLE handle,
			unsigned int nIndex	// wave out index
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetWaveOutDevice(nIndex);
}

// =============================================================================================================

int __stdcall libZPlay::zplay_OpenFile(
			ZPLAY_HANDLE handle,
			const char *sFileName,
			TStreamFormat nFormat
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->OpenFile(sFileName, nFormat); 

}

// =============================================================================================================
int __stdcall libZPlay::zplay_OpenFileW(
			ZPLAY_HANDLE handle,
			const wchar_t *sFileName,
			TStreamFormat nFormat	
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->OpenFileW(sFileName, nFormat); 

}
// =============================================================================================================


int __stdcall libZPlay::zplay_OpenStream(
			ZPLAY_HANDLE handle,
			int fBuffered,		// flag specifying buffered stream
			int fManaged,		// flag flag for enabling or disabling managed stream
			const void *sMemStream,// pointer to first memory block
			unsigned int nStreamSize,	// size of memory block in bytes
			TStreamFormat nFormat
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->OpenStream(fBuffered, fManaged, sMemStream, nStreamSize, nFormat);  

}


int __stdcall libZPlay::zplay_PushDataToStream(
			ZPLAY_HANDLE handle,
			const void *sMemNewData,	// pointer to memory block
			unsigned int nNewDataize	// size of memory block in bytes
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->PushDataToStream(sMemNewData, nNewDataize);

}


int __stdcall libZPlay::zplay_IsStreamDataFree(
			ZPLAY_HANDLE handle,
			const void *sMemNewData	// pointer to memory block
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->IsStreamDataFree(sMemNewData);

}


void __stdcall libZPlay::zplay_GetDynamicStreamLoad(
			ZPLAY_HANDLE handle,
			TStreamLoadInfo *pStreamLoadInfo// pointer to TStreamLoadInfo structure
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetDynamicStreamLoad(pStreamLoadInfo);

}


void __stdcall libZPlay::zplay_GetStreamInfo(
			ZPLAY_HANDLE handle,
			TStreamInfo *pInfo	// pointer to STREAM_INFO structure
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetStreamInfo(pInfo);

}

void __stdcall libZPlay::zplay_GetStreamInfoW(
			ZPLAY_HANDLE handle,
			TStreamInfoW *pInfo	// pointer to STREAM_INFO structure
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetStreamInfoW(pInfo);

}

int __stdcall libZPlay::zplay_Close(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->Close();

}

int __stdcall libZPlay::zplay_Play(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->Play();
}


int __stdcall libZPlay::zplay_PlayLoop(
			ZPLAY_HANDLE handle,
			TTimeFormat fFormatStartTime,// flag specifying start time format
			TStreamTime *pStartTime,// pointer to TStreamTime structure specifying start time
			TTimeFormat fFormatEndTime,	// flag specifying end time format
			TStreamTime *pEndTime,	// pointer to TStreamTime structure specifying end time
			unsigned int nNumOfCycles,	// number of loop cycles
			unsigned int fContinuePlaying
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->PlayLoop(fFormatStartTime, pStartTime, fFormatEndTime, pEndTime, nNumOfCycles, fContinuePlaying); 

}


int __stdcall libZPlay::zplay_Seek(
			ZPLAY_HANDLE handle,
			TTimeFormat fFormat,// flag specifying time format
			TStreamTime *pTime,	// pointer to TStreamTime structure specifying time
			TSeekMethod nMoveMethod	// how to move 
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->Seek(fFormat, pTime, nMoveMethod);

}

int __stdcall libZPlay::zplay_ReverseMode(
			ZPLAY_HANDLE handle,
			int fEnable	//  flag for enabling or disabling reverse mode
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->ReverseMode(fEnable);

}

int __stdcall libZPlay::zplay_Stop(ZPLAY_HANDLE handle)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->Stop();
}

int __stdcall libZPlay::zplay_Pause(ZPLAY_HANDLE handle)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->Pause();
}

int __stdcall libZPlay::zplay_Resume(ZPLAY_HANDLE handle)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->Resume();
}

void __stdcall libZPlay::zplay_GetPosition(
			ZPLAY_HANDLE handle,
			TStreamTime *pTime	// pointer to TStreamTime structure
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetPosition(pTime);
}


int __stdcall libZPlay::zplay_SetMasterVolume(
			ZPLAY_HANDLE handle,
			unsigned int nLeftVolume,	// left channel volume
			unsigned int nRightVolume	// right channel volume
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetMasterVolume(nLeftVolume, nRightVolume); 

}


int __stdcall libZPlay::zplay_SetPlayerVolume(
			ZPLAY_HANDLE handle,
			unsigned int nLeftVolume,	// left channel volume
			unsigned int nRightVolume	// right channel volume
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetPlayerVolume(nLeftVolume, nRightVolume); 

}


void __stdcall libZPlay::zplay_GetMasterVolume(
			ZPLAY_HANDLE handle,
			unsigned int *pnLeftVolume,	// pointer to variable receiving left volume
			unsigned int *pnRightVolume	// pointer to variable receiving right volume
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetMasterVolume(pnLeftVolume, pnRightVolume); 

}

void __stdcall libZPlay::zplay_GetPlayerVolume(
			ZPLAY_HANDLE handle,
			unsigned int *pnLeftVolume,	// pointer to variable receiving left volume
			unsigned int *pnRightVolume	// pointer to variable receiving right volume
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetPlayerVolume(pnLeftVolume, pnRightVolume); 

}

int __stdcall libZPlay::zplay_GetBitrate(
			ZPLAY_HANDLE handle,
			int fAverage	// get average bitrate of decoded frame  or current bitrate of decoded frame
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetBitrate(fAverage);
}


void __stdcall libZPlay::zplay_GetStatus(
			ZPLAY_HANDLE handle,
			TStreamStatus *pStatus	// pointer to TStreamStatus structure
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetStatus(pStatus);
}

int __stdcall libZPlay::zplay_MixChannels(
			ZPLAY_HANDLE handle,
			int fEnable,		// flag for enabling or disabling channel mixing
			unsigned int nLeftPercent,	// percent of left channel in resulting sound ( 0 - 100 )
			unsigned int nRightPercent	// percent of right channel in resulting sound ( 0 - 100 )
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->MixChannels(fEnable, nLeftPercent, nRightPercent);

}


void __stdcall libZPlay::zplay_GetVUData(
			ZPLAY_HANDLE handle,
			unsigned int *pnLeftChannel,	// pointer to integer receiving left channel data
			unsigned int *pnRightChannel	// pointer to integer receiving right channel data
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return;

	instance->GetVUData(pnLeftChannel, pnRightChannel);

}

int __stdcall libZPlay::zplay_SlideVolume(
			ZPLAY_HANDLE handle,
			int fFormatStart,		// flag specifying time format of starting position
			TStreamTime *pTimeStart,// pointer to TStreamTime structure specifying start fade position
			unsigned int nStartVolumeLeft,	// starting fade volume of left channel
			unsigned int nStartVolumeRight,	// starting fade volume of right channel
			int fFormatEnd,			// flag specifying time format of end position
			TStreamTime *pTimeEnd,	// pointer to TStreamTime structure specifying end fade position
			unsigned int nEndVolumeLeft,	// end fade volume of left channel
			unsigned int nEndVolumeRight	// end fade volume of right channel
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SlideVolume(fFormatStart, pTimeStart, nStartVolumeLeft, nStartVolumeRight, 
											fFormatEnd, pTimeEnd, nEndVolumeLeft, nEndVolumeRight);

}

int __stdcall libZPlay::zplay_EnableEqualizer(
			ZPLAY_HANDLE handle,
			int fEnable	//  flag for enabling or disabling equalizer
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->EnableEqualizer(fEnable);

}


int __stdcall libZPlay::zplay_SetEqualizerPoints(
			ZPLAY_HANDLE handle,
			int *pnFreqPoint,	// pointer to array of integers
			int nNumOfPoints	// number of elements in pnFreqPoint array( min value is 2)
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetEqualizerPoints(pnFreqPoint, nNumOfPoints);
}

int __stdcall libZPlay::zplay_GetEqualizerPoints(
			ZPLAY_HANDLE handle,
			int *pnFreqPoint,	// pointer to array of integers
			int nFreqPointNumber
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetEqualizerPoints(pnFreqPoint, nFreqPointNumber);

}

int __stdcall libZPlay::zplay_SetEqualizerParam(
			ZPLAY_HANDLE handle,
			int nPreAmpGain,	// preamp gain ( millidecibel)
			int *pnBandGain,	// array of gain for each band ( millidecibels )
			int nNumberOfBands	// number of frequency bands
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetEqualizerParam(nPreAmpGain, pnBandGain, nNumberOfBands);

}

int __stdcall libZPlay::zplay_GetEqualizerParam(
			ZPLAY_HANDLE handle,
			int *pnPreAmpGain,	// pointer to integer receiving preamp gain ( millidecibels)
			int *pnBandGain,	// pointer to variable receiving gain for each band ( millidecibels)
			int nNumberOfBands
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetEqualizerParam(pnPreAmpGain, pnBandGain, nNumberOfBands);

}


int __stdcall libZPlay::zplay_EnableEcho(
			ZPLAY_HANDLE handle,
			int fEnable		// flag for enabling or disabling sfx processing
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->EnableEcho(fEnable);

}

int __stdcall libZPlay::zplay_SetEchoParam(
			ZPLAY_HANDLE handle,
			TEchoEffect *pEchoEffect,	// pointer to array of echo effects
			int nNumberOfEffects// number of effects in pEchoEffect array
			
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetEchoParam(pEchoEffect, nNumberOfEffects);

}

int __stdcall libZPlay::zplay_GetEchoParam(
			ZPLAY_HANDLE handle,
			TEchoEffect *pEchoEffect,	// pointer to array of echo effects
			int nNumberOfEffects
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetEchoParam(pEchoEffect, nNumberOfEffects);
}


int __stdcall libZPlay::zplay_SetCallbackFunc(
			ZPLAY_HANDLE handle,
			TCallbackFunc pCallbackFunc, // pointert to callback function
			TCallbackMessage nMessages, // specify callback mesages
			void *user_data
)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetCallbackFunc(pCallbackFunc, nMessages, user_data);

}


int __stdcall libZPlay::zplay_SetRate(
			ZPLAY_HANDLE handle,
			unsigned int nRate	// rate value
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetRate(nRate);

}

unsigned int __stdcall libZPlay::zplay_GetRate(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetRate();
}

int __stdcall libZPlay::zplay_SetPitch(
			ZPLAY_HANDLE handle,
			unsigned int nPitch	// rate value
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetPitch(nPitch);
}


unsigned int __stdcall libZPlay::zplay_GetPitch(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetPitch();
}


int __stdcall libZPlay::zplay_SetTempo(
			ZPLAY_HANDLE handle,
			unsigned int nTempo	// rate value
		)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetTempo(nTempo);

}

int __stdcall libZPlay::zplay_StereoCut(
			ZPLAY_HANDLE handle,
			int fEnable,
			int fOutputCenter,
			int fBassToSides
		)
{

	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->StereoCut(fEnable, fOutputCenter, fBassToSides);
}


int __stdcall libZPlay::zplay_SetEqualizerPreampGain(ZPLAY_HANDLE handle, int nGain)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetEqualizerPreampGain(nGain);
}

int __stdcall libZPlay::zplay_GetEqualizerPreampGain(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->GetEqualizerPreampGain();
}
 
int __stdcall libZPlay::zplay_SetEqualizerBandGain(ZPLAY_HANDLE handle, int nBandIndex, int nGain)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;

	return instance->SetEqualizerBandGain(nBandIndex, nGain);
}

int __stdcall libZPlay::zplay_GetEqualizerBandGain(ZPLAY_HANDLE handle, int nBandIndex)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->GetEqualizerBandGain(nBandIndex);

}


unsigned int __stdcall libZPlay::zplay_strlen(const char *s)
{
	return strlen(s);
}

unsigned int __stdcall libZPlay::zplay_wcslen(const wchar_t *s)
{
	return wcslen(s);
}

void __stdcall libZPlay::zplay_memcpy(void *dest, const void *src, unsigned int n)
{
	memcpy(dest, src, n);
}

void __stdcall libZPlay::zplay_memmove(void *dest, const void *src, unsigned int n)
{
	memmove(dest, src, n);
}

char * __stdcall libZPlay::zplay_strcpy(char *dest, const char *src)
{
	return strcpy(dest, src);
}

wchar_t * __stdcall libZPlay::zplay_wcscpy(wchar_t *dest, const wchar_t *src)
{
	return wcscpy(dest, src);
}


void * __stdcall libZPlay::zplay_memset(void *s, int c, unsigned int n)
{
	return memset(s, c, n);
}


int __stdcall libZPlay::zplay_AddFile(
			ZPLAY_HANDLE handle,
			const char *sFileName,
			TStreamFormat nFormat
			)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->AddFile(sFileName, nFormat); 

}



int __stdcall libZPlay::zplay_AddFileW(
			ZPLAY_HANDLE handle,
			const wchar_t *sFileName,
			TStreamFormat nFormat
			)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->AddFileW(sFileName, nFormat); 

}


int __stdcall libZPlay::zplay_SetSettings(ZPLAY_HANDLE handle, TSettingID nSettingID, int nValue)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->SetSettings(nSettingID, nValue); 
}


int __stdcall libZPlay::zplay_GetSettings(ZPLAY_HANDLE handle, TSettingID nSettingID)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->GetSettings(nSettingID); 

}


int __stdcall zplay_EnumerateWaveIn(ZPLAY_HANDLE handle)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->EnumerateWaveIn(); 
}


int __stdcall zplay_GetWaveInInfo(ZPLAY_HANDLE handle, unsigned int nIndex, TWaveInInfo *pWaveInInfo)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->GetWaveInInfo(nIndex, pWaveInInfo); 
}

int __stdcall zplay_GetWaveInInfoW(ZPLAY_HANDLE handle, unsigned int nIndex, TWaveInInfoW *pWaveInInfo)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->GetWaveInInfoW(nIndex, pWaveInInfo); 
}

int __stdcall zplay_SetWaveInDevice(ZPLAY_HANDLE handle, unsigned int nIndex)
{
	WMp3x *instance = (WMp3x*) handle;
	if(instance == 0)
		return 0;


	return instance->SetWaveInDevice(nIndex); 
}
