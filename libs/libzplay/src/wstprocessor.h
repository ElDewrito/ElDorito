/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  Audio pitch and time stretching processor
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
 * SoundTouch library Copyright (c) Olli Parviainen 2002-2009 
 * http://www.surina.net/soundtouch
 * GNU Lesser General Public License vesrion 2.1 (LGPL.TXT )
 * ============================================================================
*/

#ifndef _W_STPROCESSOR_H_
#define _W_STPROCESSOR_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "waudioprocessor.h"
#include "../decoders/soundtouch/include/soundtouch.h"



using namespace soundtouch;


class WSoundTouchProcessor : public WAudioProcessor {
public:
	WSoundTouchProcessor();
	~WSoundTouchProcessor();

	int PushSamples(PROCESSOR_AUDIO_DATA *data);

// change virtual functions
	int Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample);
	int  Enable(int fBroadcast, int fEnable);


// ===================================================================
//	SET RATE

	int SetRate(unsigned int nRate);

	//	PARAMETERS:
	//
	//		nRate
	//			Rate parameter. Normal rate = 100, smaller values
    //			represent slower rate, larger faster rate.
	//
	//	RETURN VALUES:
	//			1	- all OK
	//			0	- error
	//
// =========================================================================
//	SET PITCH

	int SetPitch(unsigned int nPitch);

	//	PARAMETERS:
	//
	//		nPitch
	//			Rate parameter. Original pitch = 100, smaller values
    //			represent lower pitches, larger values higher pitch.
	//
	//	RETURN VALUES:
	//			1	- all OK
	//			0	- error
	//
// =========================================================================
//	SET TEMPO

	int SetTempo(unsigned int nTempo);

	//	PARAMETERS:
	//
	//		nTempo
	//			Rate parameter. Normal tempo = 100, smaller values
    //			represent slower tempo, larger faster tempo.
	//
	//	RETURN VALUES:
	//			1	- all OK
	//			0	- error
	//
// =========================================================================
//	GET RATE

	unsigned int GetRate() { return c_nRate; };

// =========================================================================
//	GET PITCH

	unsigned int GetPitch() { return c_nPitch; };

// =========================================================================
//	GET TEMPO

	unsigned int GetTempo() { return c_nTempo; };

// =========================================================================

	int Flush(int fBroadcast);
	int Clear(int fBroadcast);


	


// ===========================================================================================================================
private:

	PROCESSOR_AUDIO_DATA c_LastData;
	// control echo processing in thread
	CRITICAL_SECTION c_CriticalSection;

	SoundTouch st;


	// current rate
	unsigned int c_nRate;
	// current pitch
	unsigned int c_nPitch;
	// current tempo
	unsigned int c_nTempo;

};

#endif
