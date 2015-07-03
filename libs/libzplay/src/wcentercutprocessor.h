/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  Audio center cut processor
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
 * ver: 1.14
 * date: 15. April, 2010.
 *
 * SUPPORTED BY:
 * ============================================================================
 * Center Cut DSP Plugin for Winamp 2.x 
 * Copyright (C) 2004  Moitah (moitah@yahoo.com)
 * GNU General Public License ( GPL.TXT )
 * ============================================================================
*/

#ifndef _W_CENTERCUTPROCESSOR_H_
#define _W_CENTERCUTPROCESSOR_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "waudioprocessor.h"

typedef float MYREAL;

#define WINDOW_SIZE_CT 8192
#define OVERLAP_COUNT_CT 4


typedef MYREAL REALMATRIX[2];
typedef MYREAL REALMATRIX1[WINDOW_SIZE_CT / OVERLAP_COUNT_CT];





class WCenterCutProcessor : public WAudioProcessor {
public:
	WCenterCutProcessor();
	~WCenterCutProcessor();

	int PushSamples(PROCESSOR_AUDIO_DATA *data);

// change virtual functions
	int Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample);
	int  Enable(int fBroadcast, int fEnable);

// ================================================================
//	SET PARAMETERS

	int SetParameters(int fOutputCenter, int fBassToSides);

	//	PARAMETERS:
	//		fOutputCenter
	//			Output center or cut center. Value 1 outputs center and cuts sides.
	//			Value 0 outputs sides and cut center.
	//
	//		fBassToSides
	//			Value 1 enables bass to sides, value 0 cuts bass from sides.
	//
	//	RETURN VALUES:
	//		1 - all OK
	//		0 - error, call GetError() to get error string

// ================================================================

	int Flush(int fBroadcast);
	int Clear(int fBroadcast);


	


// ===========================================================================================================================
private:

	PROCESSOR_AUDIO_DATA c_LastData;

	// control echo processing in thread
	CRITICAL_SECTION c_CriticalSection;

	// output buffer
	char * c_pchOutputBuffer;
	// output buffer size
	unsigned int c_nOutputBufferSize;

	int c_nLatencyInSamples;

	BOOL c_fOutputCenter;
	BOOL c_fBassToSides;


	static int CenterCutProcessSamples(WCenterCutProcessor *instance, unsigned char *inSamples, int inSampleCount, unsigned char *outSamples, int outSampleCount);
	static BOOL CenterCut_Run(WCenterCutProcessor *instance);
	static BOOL CenterCut_Start(WCenterCutProcessor *instance);
	static void OutputBufferInit(WCenterCutProcessor *instance);
	static void OutputBufferFree(WCenterCutProcessor *instance);
	static void OutputBufferReadComplete(WCenterCutProcessor *instance);
	static BOOL OutputBufferBeginWrite(WCenterCutProcessor *instance);
	static int _Flush(WCenterCutProcessor *instance);

	unsigned int mInputSamplesNeeded;
	int	mOutputReadSampleOffset;
	int	mOutputBufferCount;  // How many buffers are actually in use (there may be more
									 // allocated than in use)
	MYREAL **mOutputBuffer;
	int	mOutputDiscardBlocks;
	unsigned int mInputPos;

	int freqBelowToSides;
	int mOutputMaxBuffers;

	REALMATRIX *mInput;
	REALMATRIX1 *mOverlapC;
	MYREAL *mTempLBuffer;
	MYREAL *mTempRBuffer;
	MYREAL *mTempCBuffer;



};






#endif

