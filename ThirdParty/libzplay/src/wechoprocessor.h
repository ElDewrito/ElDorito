/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  Echo processor
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
*/

#ifndef _W_ECHO_H_
#define _W_ECHO_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "waudioprocessor.h"
#include "wqueue.h"


typedef struct {
	int nLeftDelay;				// delay of left channel in milliseconds
	int nLeftSrcVolume;			// percent of original volume for left channel
	int nLeftEchoVolume;		// percent of echo volume for left channel
	int nRightDelay;			// delay of right channel in milliseconds
	int nRightSrcVolume;		// percent of original volume for right channel
	int nRightEchoVolume;		// percent of echo volume for right channel
} ECHO_DELAY_EFFECT;

typedef struct {
	int nLeftDelay;
	int nLeftSrcVolume;
	int nLeftEchoVolume;
	int nRightDelay;
	int nRightSrcVolume;
	int nRightEchoVolume;
	char *int_buffer_left;
	char *int_buffer_right;
	int nLeftDelayBackup;
	int nRightDelayBackup;
} ECHO_EFFECT_INTERNAL;

class WEchoProcessor : public WAudioProcessor {
public:
	WEchoProcessor();
	~WEchoProcessor();

	int PushSamples(PROCESSOR_AUDIO_DATA *data);
	int Clear(int fBroadcast);
	int Flush(int fBroadcast);
	int Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample);
	int Enable(int fBroadcast, int fEnable);

// ============================================================================================================
//	configure echo
	int SetParameters(ECHO_DELAY_EFFECT *pEchoEffect, int nNumberOfEffects); 

	//	PARAMETERS:
	//
	//		nNumberOfEffects
	//			Number of echo effects in pEchoEffect array.
	//
	//		pEchoEffect
	//			Pointer to ECHO_EFFECT array.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error

// =============================================================================================================
// ============================================================================================================
//	get echo parameters

	int GetParameters(ECHO_DELAY_EFFECT *pEchoEffect, int nNumberOfEffects); 

	//	PARAMETERS:
	//
	//		pEchoEffect
	//			Pointer to ECHO_EFFECT array.
	//
	//	RETURN VALUES:
	//		Number of echo parameters.

private:

	// max echo delay in milliseconds
	unsigned int c_nEchoMaxDelay;
	// number of echo effects
	unsigned int c_nEchoEffectsNumber;
	// array of echo effects
	ECHO_EFFECT_INTERNAL *c_echoEffects;
	// delay line
	DelayLine c_dlEchoDelayLine;
	// control echo processing in thread
	CRITICAL_SECTION c_CriticalSection;
	// input buffer size
	unsigned int c_nInputBufferSize;
	// old max echo delay
	unsigned int c_nOldEchoMaxDelay;

	// output buffer
	char *c_pchOutput;
	// output buffer size
	unsigned int c_nOutputBufferSize;

 

};







#endif
