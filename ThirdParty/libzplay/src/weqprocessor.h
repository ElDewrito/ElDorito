/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  Equalizer processor
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
 *	SUPPORTED BY:
 * ============================================================================
 * Shibatch Super Equalizer ver 0.03 for winamp
 * written by Naoki Shibata  shibatch@users.sourceforge.net
 * http://shibatch.sourceforge.net/
 *
 * Shibatch Super Equalizer (except FFT part) is distributed under LGPL.
 * See LGPL.txt for details.
 * FFT part is a routine made by Mr.Ooura. This routine is a freeware.
 * Contact Mr.Ooura for details of distributing licenses.
 * http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html
 * ==================================================================================
 *
*/


#ifndef _W_EQUPROCESSOR_H_
#define _W_EQUPROCESSOR_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "waudioprocessor.h"
#include "wfft.h"




#define DEF_NBANDS_POINTS 9


typedef struct {
	REAL upper;
	REAL gain;
	int nFreqPoint;	
	int nGain;
} EQ_INTERNAL_PARAM;



void rdft(int n, int isgn, REAL *a, int *ip, REAL *w);

class WEqProcessor : public WAudioProcessor {
public:
	WEqProcessor();
	~WEqProcessor();

	int PushSamples(PROCESSOR_AUDIO_DATA *data);
	int Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample);
	int  Enable(int fBroadcast, int fEnable);
	int Flush(int fBroadcast);
	int Clear(int fBroadcast);


// ========================================================
// get eq bands limit frequencies for external equelizer

	int GetEqBands(int *pnFreqPoint, int nFreqPointNumber);

	//	PARAMETERS:
	//		pnFreqPoint
	//			Pointer to array of integers receiving band limit points.
	//			If this value is NULL, the function returns the required number of elements
	//			in pnFreqPoint array.
	//
	//		nFreqPointNumber
	//			Number of freq points.
	//			If this value is NULL, the function returns the required number of elements
	//			in pnFreqPoint array.
	//
	//	RETURN VALUES:
	//		Function returns the required number of elements
	//		in pnFreqPoint array.
	//
	//

// ==================================================================================================================

// create new equalizer bands

	int  CreateEqBands(int *pnFreqPoint,int nNumOfPoints);

		//	PARAMETERS:
		//		pnFreqPoint
		//			Specifies frequency points to divide whole frequency range into bands.
		//			You have frequency range from 0 Hz to SampleRate/2 Hz. You need to specify
		//			frequency point to divide this range into bands. One point will divide range into
		//			2 bands, 2 points will divide range into 3 bands ...
		//
		//			Example: if you specify one point of 5000 Hz, you will have 2 bands:
		//			band 1: 0 Hz to 5000 Hz
		//			band 2: 5000 Hz to SampleRate/2 
		//
		//			If this value is NULL, class will restore default bands.
		//	
		//		nNumOfPoints
		//			Number of points in pnFreqPoint. Must be at least 1 because you need to divide
		//			frequency range at least into 2 bands.
		//
		//			If this value is 0, class will restore default bands.
		//
		//	RETURN VALUES:
		//		1 - all OK
		//		0 - error, call GetError() to get error string
		//
		//	REMARKS:
		//
		//		Class has 10 frequency bands by default. You can create your bands.
		//		There is no limit on number of bands.
		//
		//	Default bands on new created class: 
		//
		//			band:			  0     1     2     3     4      5      6      7       8       9 
		//			FreqPoint (Hz): 0 - 115 - 240 - 455 - 800 - 2000 - 4500 - 9000 - 13000 - 15000 - SampleRate/2
		//
		// Example 1: ( create your frequency bands )
		//
		//	int freq_points[7] = { 100, 400, 1000, 3000, 5000, 10000, 15000};
		//	CreateEqBands(freq_points, 7);
		//
		// this function creates 8 bands: 
		//
		//			band:		      0     1      2      3      4       5     6       7
		//			FreqPoint (Hz): 0 - 100 - 400 - 1000 - 3000 - 5000 - 10000 - 15000 - SampleRate/2 
		//
		//

// =============================================================================================================



// =============================================================================================================
// set equalizer parameters

	int SetParameters(int nPreAmpGain, int *pnBandGain, int nNumberOfBands); 

	//	PARAMETERS:
	//
	//		nPreAmpGain		
	//			Preamp gain in millidecibels ( thousandth of decibel ). Value 0 specifies no change. Positive values boost volume,
	//			negative value drops volume. This value has effect on all bands.
	//
	//		pnBandGain		
	//			Array of gain for each band millidecibels ( thousandth of decibel ). Positive values boost volume,
	//			negative value drops volume.
	//
	//		nNumberOfBands	 
	//			Number of frequency bands. Class has by default 10 eq bands, but you can create
	//			new bands using CreateEqBands function ( only for external equalizer ).
	//
	//
	//	RETURN VALUES:
	//		1 - all OK
	//		0 - error, call GetError() to get error string
	//
	//	REMARKS:
	//
	//			SUBBAND FREQUENCY ( default values )
	//
	//			index:		       0     1      2      3      4      5      6        7       8       9 
	//			FreqPoint (Hz):  0 - 600 - 1600 - 2000 - 3000 - 4000 - 6000 - 10000  - 13000 - 17000 - EQ_MAX_FREQ
	//
	//
	//		If you use your own eq bands ( CreateEqBands ) you need to send gain for each your band.
	//		If you create 20 eq bands, then you need to send pointer to 20 values as pnBandValue parameter
	//
	//
	// Example:
	//
	// int eq_values[10];
	// eq_values[0] = -12000;
	// eq_values[1] = -12000;  // cut bas 12 db
	// eq_values[2] = 0;
	// eq_values[3] = 0;  // unchanged
	// eq_values[4] = 0;
	// eq_values[5] = 0;
	// eq_values[6] = 0;
	// eq_values[7] = 12000; // boost high tones 12 db
	// eq_values[8] = 12500;
	// eq_values[9] = 0;
	// SetParameters(0, 10, eq_values);

// ========================================================================================================================
// get equalizer parameters

	int GetParameters(int *pnPreAmpGain, int *pnBandGain, int nNumberOfBands); 

	//	PARAMETERS:
	//
	//		pnPreAmpGain		
	//			Pointer to integer receiving preamp gain in millidecibels.
	//			If this value is NULL function returns number of equalizer bands.
	//
	//		pnBandGain		
	//			Pointer to variable receiving gain for each band in millidecibels.
	//			If this value is NULL function returns number of equalizer bands.
	//
	//		nNumberOfBands
	//			Number of bands.
	//
	//
	//	RETURN VALUES:
	//		number of bands

// =======================================================================================================================
	// set preamp gain in decibels
	int SetPreampGain(int nGain);
	
	// PARAMETERS:
	//		nGain
	//			Preamp gain in millidecibels. Positive values boost volume, negative values decrease volume.
	//
	//		
	//	RETURN VALUES:
	//		1 - all OK
	//		0 - error, call GetError() to get error string

// ========================================================================================================================
	// get preamp gain
	int GetPreampGain();

	// PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		Preamp gain in millidecibels. Positive values boost volume, negative values decrease volume.

// ========================================================================================================================
	// set band for specified gain
	int SetBandGain(int nBandIndex, int nGain);

	//	PARAMETERS:
	//		nBandIndex
	//			Index of band to set gain.
	//
	//		nGain
	//			Band gain in millidecibels. Positive values boost volume, negative values decrease volume.
	//
	//	RETURN VALUES:
	//		1 - all OK
	//		0 - error, call GetError() to get error string


// ========================================================================================================================
	// get band gain
	int GetBandGain(int nBandIndex);

	//	PARAMETERS:
	//		nBandIndex
	//			Index of band to get gain.
	//
	//
	//	RETURN VALUES:
	//		Band gain. Positive values boost volume, negative values decrease volume.

// ===========================================================================================================================
private:

	PROCESSOR_AUDIO_DATA c_LastData;

	// internal latency counter, need this to enter eq processing, to skip bytes	
	unsigned int c_nCurrentEqLatency;
	// old sampling rate
	unsigned int c_nOldSampleRate;	


	// control echo processing in thread
	CRITICAL_SECTION c_CriticalSection;


	int c_nLatencyInSamples;


	// output buffer
	char *c_pchOutput;
	// output buffer size
	unsigned int c_nOutputBufferSize;

	// max latency in samples
	unsigned int c_nMaxLatencySamples;

// ================================================
	REAL *lires;
	REAL *lires1;
	REAL *lires2;
	REAL *rires;
	REAL *rires1;
	REAL *rires2;
	REAL *irest;
	REAL *fsamples;
	short *inbuf;
	REAL *outbuf;
	float hm1;
	float hm2;
	int ditherptr;
	int weq_enable;
	REAL *ditherbuf;
	int *c_bit_reversal;			// working area for bit reversal
	REAL *c_cos_sin_table;			// cos/sin table

	int nbufsamples;
	int dither;

	int volatile cur_ires;
	int volatile chg_ires;

	int c_nPreampGain;

	
	// band/gain param
	EQ_INTERNAL_PARAM *c_eqParam;	// array of EQ_INTERNAL_PARAM params
	unsigned int c_nNumberOfBands;	// number of bands, size of c_eqParam array

	// create default eq bands, allocate memory for bands data
	static int _CreateDefaultBands(WEqProcessor *instance);
	// free allocated memory for bands
	static void _DestroyBands(WEqProcessor *instance);

	// allocate internal memory
	static int _AllocateInternalMemory(WEqProcessor *instance);
	// free internal memory
	static void _FreeInternalMemory(WEqProcessor *instance);
	
	// make equ tables, set samplerate and band gains and call this function
	static void _MakeEqTables(WEqProcessor *instance);

	static void _ClearBuffer(WEqProcessor *instance);
	static int _ModifySamples(WEqProcessor *instance, char *input_buf, char *output_buf, int nsamples, int nch, int bps);

	

};

#endif
