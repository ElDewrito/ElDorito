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

#ifndef _W_WWAVEIN_H_
#define _W_WWAVEIN_H_

#include <mmsystem.h>
#include "wdecoder.h"




class WWaveIn : public WAudioDecoder {
public:

	WWaveIn();
	~WWaveIn();

	int OpenStream(WQueue *pQueue, int fDynamic, int param1, int param2);

	INPUT_STREAM_INFO *GetStreamInfo();

	wchar_t **GetID3Info(int version, char *pStream, unsigned int nStreamSize, int param1, int param2);

	int Close();

// ======================================================================
	int  Initialize(int param1, int param2, int param3, int param4);
	//
	//
	//			param1
	//				Specify WaveIn device number.	
	//
	//			param2
	//				Specifies line control( mic, line, aux, ...)
	//
	//			param3
	//				Specifies bit per sample ( 8 or 16 ):
	//
	//			param4
	//				Big-endian indicator. If this value is 0, little-endian is used.
	//				If this value is 1, big-endian is used.
// ===========================================================================

	int Uninitialize();
	void  Release();
	int GetData(DECODER_DATA *pDecoderData);

	// seek current position
	int Seek(unsigned int nSamples);

	int GetBitrate(int fAverage);

	DECODER_ERROR_MESSAGE *GetError();

	int SetReverseMode(int fReverse);

private:


	HWAVEIN c_hWaweIn;
	unsigned int c_InDeviceID;
	WAVEHDR c_wavehdr[2];
	char *c_buffer1;
	char *c_buffer2;
	int c_fWorking;
	CRITICAL_SECTION cs;

	unsigned int c_nMaxLatencyInMilliseconds;

	unsigned int c_line_control_id;

	unsigned int c_nSamplesForOneBuffer;
	unsigned int c_nBytesForOneBuffer;

	unsigned int c_nMaxQueueSizeInBytes;

	// callback function
	static void CALLBACK _WaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2);

	// input data
	int c_nInSampleRate;
	int c_inChannelNum;
	int c_inBitPerSample;

	// indicates that stream is ready
	unsigned int c_fReady;

	// info about input stream
	INPUT_STREAM_INFO c_isInfo;

	WQueue c_Queue;

	void err(unsigned int error_code);
	DECODER_ERROR_MESSAGE c_err_msg;
	
	unsigned int c_nCurrentPosition;

	char *c_user_mixer_line;
	unsigned int c_user_mixer_volume;

};




#endif
