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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "debug.h"
#include "wcentercutprocessor.h"


#define LATENCY 6772

typedef signed __int64		sint64;
typedef unsigned __int64	uint64;
typedef signed int			sint32;
typedef unsigned int		uint32;
typedef signed short		sint16;
typedef unsigned short		uint16;
typedef signed char			sint8;
typedef unsigned char		uint8;

typedef sint64				int64;
typedef sint32				int32;
typedef sint16				int16;
typedef sint8				int8;

// ===============================================================================================
// CONSTANTS


const int kWindowSize = WINDOW_SIZE_CT;
const int kOverlapCount = OVERLAP_COUNT_CT;
const int kPostWindowPower = 2;  // Maximum power including pre-window is kOverlapCount-1,
									   // which means this can be kOverlapCount-2 at most
const int kHalfWindow = kWindowSize / 2;
const int kOverlapSize = kWindowSize / kOverlapCount;

const MYREAL twopi = 6.283185307179586476925286766559;
const MYREAL invsqrt2 = 0.70710678118654752440084436210485;
const MYREAL nodivbyzero = 0.000000000000001;

const int mOutputSampleCount = kOverlapSize;


const int BYTES_TO_REAL  = 1;
const int REAL_TO_BYTES = 0;


const MYREAL SampleScaleInv = 32768.0;
const MYREAL SampleScale = 1.0 / SampleScaleInv;
const MYREAL SampleMin = -2147483648.0;
const MYREAL SampleMax = 2147483647.0;


// =====================================================================================================


MYREAL 		mPreWindow[kWindowSize];
MYREAL 		mPostWindow[kWindowSize];
unsigned	mBitRev[kWindowSize];
MYREAL 		mSineTab[kWindowSize];




void ConvertSamples(int type, uint8 *sampB, MYREAL *sampD, int sampleCount, int bitsPerSample, int chanCount);
BOOL BPSIsValid(int bitsPerSample);
void CenterCut_Finish();


WCenterCutProcessor::WCenterCutProcessor()
{
	c_pchOutputBuffer = 0;
	c_nOutputBufferSize = 0;



	mInput = 0;
	mOverlapC = 0;
	mTempLBuffer = 0;
	mTempRBuffer = 0;
	mTempCBuffer = 0;
	mOutputBuffer = 0;

	c_fOutputCenter = 0;
	c_fBassToSides = 0;

	freqBelowToSides = 0;

	mOutputMaxBuffers = 4;

	memset(&c_LastData, 0, sizeof(PROCESSOR_AUDIO_DATA));

	c_nLatencyInSamples = 0;

	CenterCut_Start(this);
	InitializeCriticalSection(&c_CriticalSection);
		
}

WCenterCutProcessor::~WCenterCutProcessor()
{
	if(c_pchOutputBuffer)
		free(c_pchOutputBuffer);

	CenterCut_Finish();


	if(mInput)
		free(mInput);

	if(mOverlapC)
		free(mOverlapC);

	if(mTempLBuffer)
		free(mTempLBuffer);

	if(mTempRBuffer)
		free(mTempRBuffer);

	if(mTempCBuffer)
		free(mTempCBuffer);


	OutputBufferFree(this);

	if(mOutputBuffer)
		free(mOutputBuffer);


	DeleteCriticalSection(&c_CriticalSection);
}



int WCenterCutProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
	// check input parameters
	if(nSampleRate == 0)
	{
		sprintf(c_pchErrorMessageStr, "WCenterCutProcessor::Configure->Sample rate can't be %u.", nSampleRate);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nChannel != 1 && nChannel != 2)
	{
		sprintf(c_pchErrorMessageStr, "WCenterCutProcessor::Configure->Number of channels can't be %u.", nChannel);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nBitPerSample != 8 && nBitPerSample != 16 && nBitPerSample != 24)
	{
		sprintf(c_pchErrorMessageStr, "WCenterCutProcessor::Configure->Bit per sample can't be %u.", nBitPerSample);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
	}

	EnterCriticalSection(&c_CriticalSection);

	// set new parameters
	c_nSampleRate = nSampleRate;
	c_nChannel = nChannel;
	c_nBitPerSample = nBitPerSample;
	c_nBlockAlign = nChannel * (nBitPerSample / 8);

	freqBelowToSides = (int)((200.0 / ((MYREAL ) c_nSampleRate / kWindowSize)) + 0.5);


	LeaveCriticalSection(&c_CriticalSection);


	// send this data to next processor

	if(fBroadcast && c_next)
	{
		#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
			if(c_pchReturnError == 0)
				c_next->c_pchReturnError = c_pchErrorMessageStr;
			else
				c_next->c_pchReturnError = c_pchReturnError;
		#endif

		return c_next->Configure(fBroadcast, nSampleRate, nChannel, nBitPerSample);
	}

	return 1;
}


int WCenterCutProcessor::SetParameters(int fOutputCenter, int fBassToSides)
{
	EnterCriticalSection(&c_CriticalSection);
	c_fOutputCenter = fOutputCenter;
	c_fBassToSides = fBassToSides;
	LeaveCriticalSection(&c_CriticalSection);
	return 1;
}




int WCenterCutProcessor::Enable(int fBroadcast, int fEnable)
{

	EnterCriticalSection(&c_CriticalSection);
	
	if(fEnable)
	{
		c_nLatencyInSamples = 0;
		if(c_fEnable == 0)
		{

			mOutputMaxBuffers = 4;

			mInput = (REALMATRIX*) malloc(kWindowSize * 2 * sizeof(MYREAL));
			mOverlapC = (REALMATRIX1*) malloc( (kOverlapCount - 1) * (kWindowSize / kOverlapCount) * sizeof(MYREAL));
			mTempLBuffer = (MYREAL*) malloc(kWindowSize * sizeof(MYREAL));
			mTempRBuffer = (MYREAL*) malloc(kWindowSize * sizeof(MYREAL));
			mTempCBuffer = (MYREAL*) malloc(kWindowSize * sizeof(MYREAL));
			mOutputBuffer = (MYREAL**) malloc(mOutputMaxBuffers * sizeof(MYREAL*));

			if(mInput == 0 || mOverlapC == 0 || mTempLBuffer == 0 || mTempRBuffer == 0 || mTempCBuffer == 0 || mOutputBuffer == 0)
			{
				strcpy(c_pchErrorMessageStr, "WCenterCutProcessor::Enable->Memory allocation error.");
				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);

				LeaveCriticalSection(&c_CriticalSection);
				return 0;

			}

			CenterCut_Start(this);

			OutputBufferInit(this);

			memset(mInput, 0, kWindowSize * 2 * sizeof(MYREAL));
			memset(mOverlapC, 0, (kOverlapCount - 1) * (kWindowSize / kOverlapCount) * sizeof(MYREAL));

			mInputSamplesNeeded = kOverlapSize;
			mInputPos = 0;

			mOutputDiscardBlocks = kOverlapCount - 1;

			c_fEnable = 1;
		}

	}
	else
	{
		if(c_fEnable == 1)
		{
			_Flush(this);

			c_nLatencyInSamples = 0;

			free(mInput);
			free(mOverlapC);
			free(mTempLBuffer);
			free(mTempRBuffer);
			free(mTempCBuffer);

			OutputBufferFree(this);
			free(mOutputBuffer);

			mInput = 0;
			mOverlapC = 0;
			mTempLBuffer = 0;
			mTempRBuffer = 0;
			mTempCBuffer = 0;
			mOutputBuffer = 0;

			c_fEnable = 0;
		}
	}


	LeaveCriticalSection(&c_CriticalSection);



	if(fBroadcast && c_next)
	{
		#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
			if(c_pchReturnError == 0)
				c_next->c_pchReturnError = c_pchErrorMessageStr;
			else
				c_next->c_pchReturnError = c_pchReturnError;
		#endif

		return c_next->Enable(fBroadcast, fEnable);
	}

	
	return 1;

}




int WCenterCutProcessor::Flush(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WCenterCutProcessor::Flush\n");
	#endif


	EnterCriticalSection(&c_CriticalSection);

	

	if(c_fEnable)
	{

		if(_Flush(this) == 0)
		{
			c_nLatencyInSamples = 0;
			LeaveCriticalSection(&c_CriticalSection);
			return 0;
		}
	}

	c_nLatencyInSamples = 0;

	LeaveCriticalSection(&c_CriticalSection);


	if(fBroadcast && c_next)
	{
		#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
			if(c_pchReturnError == 0)
				c_next->c_pchReturnError = c_pchErrorMessageStr;
			else
				c_next->c_pchReturnError = c_pchReturnError;
		#endif

		return c_next->Flush(fBroadcast);
	}

	return 1;
}


int WCenterCutProcessor::Clear(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WCenterCutProcessor::Clear\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);

	c_nLatencyInSamples = 0;

	OutputBufferFree(this);
	OutputBufferInit(this);

	if(mInput)
		memset(mInput, 0, kWindowSize * 2 * sizeof(MYREAL));

	if(mOverlapC)
		memset(mOverlapC, 0, (kOverlapCount - 1) * (kWindowSize / kOverlapCount) * sizeof(MYREAL));

	if(c_pchOutputBuffer)
		free(c_pchOutputBuffer);

	c_pchOutputBuffer = 0;
	c_nOutputBufferSize = 0;

	// clear output queue
	c_queue.Clear();


	LeaveCriticalSection(&c_CriticalSection);


	if(fBroadcast && c_next)
	{
		#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
			if(c_pchReturnError == 0)
				c_next->c_pchReturnError = c_pchErrorMessageStr;
			else
				c_next->c_pchReturnError = c_pchReturnError;
		#endif

		return c_next->Clear(fBroadcast);
	}

	return 1;
}



// ===========================================================================================================================






int WCenterCutProcessor::CenterCutProcessSamples(WCenterCutProcessor *instance, unsigned char *inSamples, int inSampleCount, unsigned char *outSamples, int outSampleCount)
{
	int bytesPerSample = instance->c_nBitPerSample / 8;
	
	int maxOutSampleCount = outSampleCount;
	int copyCount;

	outSampleCount = 0;


	// process all input data
	while (inSampleCount > 0)
	{
		copyCount = min((int) instance->mInputSamplesNeeded, inSampleCount);

		ConvertSamples(BYTES_TO_REAL , inSamples, &instance->mInput[instance->mInputPos][0], copyCount, instance->c_nBitPerSample, 2);

		inSamples += copyCount * bytesPerSample * 2;
		inSampleCount -= copyCount;
		instance->mInputPos = (instance->mInputPos + copyCount) & (kWindowSize - 1);
		instance->mInputSamplesNeeded -= copyCount;

		if (instance->mInputSamplesNeeded == 0)
		{
			CenterCut_Run(instance);
		}
	}



	while ((instance->mOutputBufferCount > 0) && (outSampleCount < maxOutSampleCount))
	{
		MYREAL *sampD = instance->mOutputBuffer[0];
		if (sampD == 0)
			return 0; // memory allocation error

		copyCount = min(mOutputSampleCount - instance->mOutputReadSampleOffset,
			maxOutSampleCount - outSampleCount);

		ConvertSamples(REAL_TO_BYTES, outSamples, sampD + (instance->mOutputReadSampleOffset * 2), copyCount, instance->c_nBitPerSample, 2);

		outSamples += copyCount * bytesPerSample * 2;
		outSampleCount += copyCount;
		instance->mOutputReadSampleOffset += copyCount;
		if (instance->mOutputReadSampleOffset == mOutputSampleCount)
		{
			OutputBufferReadComplete(instance);
		}
	}

	return outSampleCount;
}

void ConvertSamples(int type, uint8 *sampB, MYREAL *sampD, int sampleCount, int bitsPerSample, int chanCount)
{


	int bytesPerSample, shiftCount;
	sint32 xor;
	uint8 *max;

	bytesPerSample = (bitsPerSample + 7) / 8;
	shiftCount = (4 - bytesPerSample) * 8;
	xor = (bytesPerSample == 1) ? (1 << 31) : 0;
	max = sampB + (sampleCount * bytesPerSample * chanCount);

	if (type == BYTES_TO_REAL )
	{
		sint32 tempI;

		while (sampB < max)
		{
			tempI = (*((sint32*)sampB) << shiftCount) ^ xor;
			*sampD = (MYREAL )tempI * SampleScale;

			sampB += bytesPerSample;
			sampD += 1;
		}
	}
	else
	{
		uint8 *maxw = max - 3;
		MYREAL tempD;
		uint32 tempI;

		while (sampB < max)
		{
			tempD = *sampD * SampleScaleInv;
			if (tempD > 0.0)
			{
				if (tempD > SampleMax)
				{
					tempD = SampleMax;
				}
				tempD += 0.5;
			}
			else
			{
				if (tempD < SampleMin)
				{
					tempD = SampleMin;
				}
				tempD -= 0.5;
			}
			tempI = (uint32)((sint32)tempD ^ xor) >> shiftCount;

			if (sampB < maxw)
			{
				*((uint32*)sampB) = tempI;
			}
			else
			{
				memcpy(sampB, &tempI, bytesPerSample);
			}

			sampB += bytesPerSample;
			sampD += 1;
		}
	}
}

void WCenterCutProcessor::OutputBufferInit(WCenterCutProcessor *instance)
{
	if(instance->mOutputBuffer)
	{
		for (int i = 0; i < instance->mOutputMaxBuffers; i++)
		{
			instance->mOutputBuffer[i] = 0;
		}
	}

	instance->mOutputBufferCount = 0;
	instance->mOutputReadSampleOffset = 0;

}

void WCenterCutProcessor::OutputBufferFree(WCenterCutProcessor *instance)
{
	if(instance->mOutputBuffer)
	{
		for (int i = 0; i < instance->mOutputMaxBuffers; i++)
		{
			if (instance->mOutputBuffer[i])
			{
				delete[] instance->mOutputBuffer[i];
				instance->mOutputBuffer[i] = 0;
			}
		}
	}
}

void WCenterCutProcessor::OutputBufferReadComplete(WCenterCutProcessor *instance)
{
	instance->mOutputBufferCount--;
	instance->mOutputReadSampleOffset = 0;
	if (instance->mOutputBufferCount > 0)
	{
		int i;
		MYREAL *moveToEnd = instance->mOutputBuffer[0];

		// Shift the buffers so that the current one for reading is at index 0
		for (i = 1; i < instance->mOutputMaxBuffers; i++)
		{
			instance->mOutputBuffer[i - 1] = instance->mOutputBuffer[i];
		}
		instance->mOutputBuffer[instance->mOutputMaxBuffers - 1] = 0;

		// Move the previous first buffer to the end (first null pointer)
		for (i = 0; i < instance->mOutputMaxBuffers; i++)
		{
			if (!instance->mOutputBuffer[i])
			{
				instance->mOutputBuffer[i] = moveToEnd;
				break;
			}
		}
	}
}

BOOL WCenterCutProcessor::OutputBufferBeginWrite(WCenterCutProcessor *instance)
{
	if (instance->mOutputBufferCount == instance->mOutputMaxBuffers)
	{
		// reallocate buffers array, add new 4 buffers
		MYREAL **buffer = (MYREAL**) malloc((instance->mOutputMaxBuffers + 4) * sizeof(MYREAL*));
		if(buffer == 0)
			return FALSE;

		// initialize new array copy old buffers to new array
		int i;
		for(i = 0; i < instance->mOutputMaxBuffers + 4; i++)
			buffer[i] = 0;
		
		for(i = 0; i < instance->mOutputMaxBuffers; i++)
			buffer[i] = instance->mOutputBuffer[i];
	

		instance->mOutputBuffer = buffer;
		instance->mOutputMaxBuffers += 4;

	}

	int i = instance->mOutputBufferCount;
	if (instance->mOutputBuffer[i] == 0)
	{
		// No buffer exists at this index, make a new one
		instance->mOutputBuffer[i] = new MYREAL [mOutputSampleCount * 2];
		if (instance->mOutputBuffer[i] == 0)
		{
			return FALSE;
		}
	}

	instance->mOutputBufferCount++;
	return TRUE;
}

BOOL BPSIsValid(int bitsPerSample)
{
	// Bits per sample must be between 8 and 32 bits, and a multiple of 8
	return (bitsPerSample >= 8) && (bitsPerSample <= 32) && ((bitsPerSample & 7) == 0);
}

unsigned IntegerLog2(unsigned v)
{
	unsigned i = 0;

	while(v>1)
	{
		++i;
		v >>= 1;
	}

	return i;
}

unsigned RevBits(unsigned x, unsigned bits)
{
	unsigned y = 0;

	while(bits--)
	{
		y = (y+y) + (x&1);
		x >>= 1;
	}

	return y;
}

void VDCreateRaisedCosineWindow(MYREAL *dst, int n, MYREAL power)
{
	const MYREAL twopi_over_n = twopi / n;
	const MYREAL scalefac = 1.0 / n;

	for(int i=0; i<n; ++i)
	{
		dst[i] = scalefac * pow(0.5 * (1.0 - cos(twopi_over_n * (i+0.5))), (double) power);
	}
}

void VDCreateHalfSineTable(MYREAL *dst, int n)
{
	const MYREAL twopi_over_n = twopi / n;

	for(int i=0; i<n; ++i)
	{
		dst[i] = sin(twopi_over_n * i);
	}
}

void VDCreateBitRevTable(unsigned *dst, int n)
{
	unsigned bits = IntegerLog2(n);

	for(int i=0; i<n; ++i)
	{
		dst[i] = RevBits(i, bits);
	}
}

void CreatePostWindow(MYREAL *dst, int windowSize, int power)
{
	const MYREAL powerIntegrals[8] = { 1.0, 1.0/2.0, 3.0/8.0, 5.0/16.0, 35.0/128.0,
									   63.0/256.0, 231.0/1024.0, 429.0/2048.0 };
	const MYREAL scalefac = (MYREAL )windowSize * (powerIntegrals[1] / powerIntegrals[power+1]);

	VDCreateRaisedCosineWindow(dst, windowSize, (MYREAL )power);

	for(int i=0; i<windowSize; ++i)
	{
		dst[i] *= scalefac;
	}
}

void VDComputeFHT(MYREAL *A, int nPoints, const MYREAL *sinTab)
{
	int i, n, n2, theta_inc;

	// FHT - stage 1 and 2 (2 and 4 points)

	for(i=0; i<nPoints; i+=4)
	{
		const MYREAL 	x0 = A[i];
		const MYREAL 	x1 = A[i+1];
		const MYREAL 	x2 = A[i+2];
		const MYREAL 	x3 = A[i+3];

		const MYREAL 	y0 = x0+x1;
		const MYREAL 	y1 = x0-x1;
		const MYREAL 	y2 = x2+x3;
		const MYREAL 	y3 = x2-x3;

		A[i]	= y0 + y2;
		A[i+2]	= y0 - y2;

		A[i+1]	= y1 + y3;
		A[i+3]	= y1 - y3;
	}

	// FHT - stage 3 (8 points)

	for(i=0; i<nPoints; i+= 8) {
		MYREAL alpha, beta;

		alpha	= A[i+0];
		beta	= A[i+4];

		A[i+0]	= alpha + beta;
		A[i+4]	= alpha - beta;

		alpha	= A[i+2];
		beta	= A[i+6];

		A[i+2]	= alpha + beta;
		A[i+6]	= alpha - beta;

		alpha	= A[i+1];

		const MYREAL beta1 = invsqrt2*(A[i+5] + A[i+7]);
		const MYREAL beta2 = invsqrt2*(A[i+5] - A[i+7]);

		A[i+1]	= alpha + beta1;
		A[i+5]	= alpha - beta1;

		alpha	= A[i+3];

		A[i+3]	= alpha + beta2;
		A[i+7]	= alpha - beta2;
	}

	n = 16;
	n2 = 8;
	theta_inc = nPoints >> 4;

	while(n <= nPoints) {
		for(i=0; i<nPoints; i+=n) {
			int j;
			int theta = theta_inc;
			MYREAL alpha, beta;
			const int n4 = n2>>1;

			alpha	= A[i];
			beta	= A[i+n2];

			A[i]	= alpha + beta;
			A[i+n2]	= alpha - beta;

			alpha	= A[i+n4];
			beta	= A[i+n2+n4];

			A[i+n4]		= alpha + beta;
			A[i+n2+n4]	= alpha - beta;

			for(j=1; j<n4; j++) {
				MYREAL 	sinval	= sinTab[theta];
				MYREAL 	cosval	= sinTab[theta + (nPoints>>2)];

				MYREAL 	alpha1	= A[i+j];
				MYREAL 	alpha2	= A[i-j+n2];
				MYREAL 	beta1	= A[i+j+n2]*cosval + A[i-j+n ]*sinval;
				MYREAL 	beta2	= A[i+j+n2]*sinval - A[i-j+n ]*cosval;

				theta	+= theta_inc;

				A[i+j]		= alpha1 + beta1;
				A[i+j+n2]	= alpha1 - beta1;
				A[i-j+n2]	= alpha2 + beta2;
				A[i-j+n]	= alpha2 - beta2;
			}
		}

		n *= 2;
		n2 *= 2;
		theta_inc >>= 1;
	}
}

BOOL WCenterCutProcessor::CenterCut_Start(WCenterCutProcessor *instance)
{
	VDCreateBitRevTable(mBitRev, kWindowSize);
	VDCreateHalfSineTable(mSineTab, kWindowSize);

	instance->mInputSamplesNeeded = kOverlapSize;
	instance->mInputPos = 0;

	instance->mOutputDiscardBlocks = kOverlapCount - 1;

	//memset(instance->mInput, 0, sizeof instance->mInput);
	//memset(instance->mOverlapC, 0, sizeof instance->mOverlapC);

	MYREAL *tmp = new MYREAL [kWindowSize];
	if (!tmp) return FALSE;
	VDCreateRaisedCosineWindow(tmp, kWindowSize, 1.0);
	for(unsigned i=0; i< kWindowSize; ++i) {
		// The correct Hartley<->FFT conversion is:
		//
		//	Fr(i) = 0.5(Hr(i) + Hi(i))
		//	Fi(i) = 0.5(Hr(i) - Hi(i))
		//
		// We omit the 0.5 in both the forward and reverse directions,
		// so we have a 0.25 to put here.

		mPreWindow[i] = tmp[mBitRev[i]] * 0.5 * (2.0 / (MYREAL )kOverlapCount);
	}
	delete[] tmp;

	CreatePostWindow(mPostWindow, kWindowSize, kPostWindowPower);

	return TRUE;
}

void CenterCut_Finish()
{
}

BOOL WCenterCutProcessor::CenterCut_Run(WCenterCutProcessor *instance)
{
	unsigned i;
	

	// copy to temporary buffer and FHT

	for(i = 0; i < kWindowSize; ++i)
	{
		const unsigned j = mBitRev[i];
		const unsigned k = (j + instance->mInputPos) & (kWindowSize - 1);
		const MYREAL w = mPreWindow[i];

		instance->mTempLBuffer[i] = instance->mInput[k][0] * w;
		instance->mTempRBuffer[i] = instance->mInput[k][1] * w;
	}

	VDComputeFHT(instance->mTempLBuffer, kWindowSize, mSineTab);
	VDComputeFHT(instance->mTempRBuffer, kWindowSize, mSineTab);

	// perform stereo separation

	instance->mTempCBuffer[0] = 0;
	instance->mTempCBuffer[1] = 0;
	for(i = 1; i < kHalfWindow; i++)
	{
		MYREAL lR = instance->mTempLBuffer[i] + instance->mTempLBuffer[kWindowSize - i];
		MYREAL lI = instance->mTempLBuffer[i] - instance->mTempLBuffer[kWindowSize - i];
		MYREAL rR = instance->mTempRBuffer[i] + instance->mTempRBuffer[kWindowSize - i];
		MYREAL rI = instance->mTempRBuffer[i] - instance->mTempRBuffer[kWindowSize - i];

		MYREAL sumR = lR + rR;
		MYREAL sumI = lI + rI;
		MYREAL diffR = lR - rR;
		MYREAL diffI = lI - rI;

		MYREAL sumSq = sumR * sumR + sumI * sumI;
		MYREAL diffSq = diffR * diffR + diffI * diffI;
		MYREAL alpha = 0.0;

		if (sumSq > nodivbyzero)
		{
			alpha = 0.5 - sqrt(diffSq / sumSq) * 0.5;
		}

		MYREAL cR = sumR * alpha;
		MYREAL cI = sumI * alpha;

		if (instance->c_fBassToSides && (i < instance->freqBelowToSides))
		{
			cR = cI = 0.0;
		}
		

		instance->mTempCBuffer[mBitRev[i              ]] = cR + cI;
		instance->mTempCBuffer[mBitRev[kWindowSize - i]] = cR - cI;
	}

	// reconstitute left/right/center channels

	VDComputeFHT(instance->mTempCBuffer, kWindowSize, mSineTab);

	// apply post-window

	for (i = 0; i < kWindowSize; i++)
	{
		instance->mTempCBuffer[i] *= mPostWindow[i];
	}

	// writeout

	if (instance->mOutputDiscardBlocks > 0)
	{
		instance->mOutputDiscardBlocks--;
	}
	else
	{
		int currentBlockIndex, nextBlockIndex, blockOffset;

		if (OutputBufferBeginWrite(instance) == 0)
			return 0;


		MYREAL *outBuffer = instance->mOutputBuffer[instance->mOutputBufferCount - 1];
		if (outBuffer == 0)
			return 0;

		for(i = 0; i < kOverlapSize; ++i)
		{
			MYREAL c = instance->mOverlapC[0][i] + instance->mTempCBuffer[i];
			MYREAL l = instance->mInput[instance->mInputPos + i][0] - c;
			MYREAL r = instance->mInput[instance->mInputPos + i][1] - c;


			if (instance->c_fOutputCenter)
			{
				outBuffer[0] = c;
				outBuffer[1] = c;
			}
			else
			{
				outBuffer[0] = l;
				outBuffer[1] = r;
			}

			outBuffer += 2;
		
			// overlapping

			currentBlockIndex = 0;
			nextBlockIndex = 1;
			blockOffset = kOverlapSize;
			while (nextBlockIndex < kOverlapCount - 1)
			{
				instance->mOverlapC[currentBlockIndex][i] = instance->mOverlapC[nextBlockIndex][i] +
															instance->mTempCBuffer[blockOffset + i];
				
				currentBlockIndex++;
				nextBlockIndex++;
				blockOffset += kOverlapSize;
			}

			instance->mOverlapC[currentBlockIndex][i] = instance->mTempCBuffer[blockOffset + i];
		}
	}

	instance->mInputSamplesNeeded = kOverlapSize;

	return TRUE;
}

int WCenterCutProcessor::_Flush(WCenterCutProcessor *instance)
{
	
		if(instance->c_nLatencyInSamples > 0)
		{
			unsigned int nInputSizeInSamples = 10000;
			unsigned char *input = (unsigned char*) malloc(nInputSizeInSamples * instance->c_nBlockAlign);
			if(input == NULL)
				return 0;

			unsigned char *output = (unsigned char*) malloc( instance->c_nLatencyInSamples * instance->c_nBlockAlign);

			if(output == NULL)
			{
				free(input);
				return 0;
			}

			memset(input, 0, nInputSizeInSamples * instance->c_nBlockAlign);

			unsigned char *output_ptr = output;
			int nHaveSamples = 0;

			while(instance->c_nLatencyInSamples > 0)
			{
					int nOutSamples = CenterCutProcessSamples(instance, input, nInputSizeInSamples , (unsigned char *) output_ptr, instance->c_nLatencyInSamples);
					instance->c_nLatencyInSamples -= nOutSamples;
					nHaveSamples += nOutSamples;
					output_ptr += (nOutSamples * instance->c_nBlockAlign);

			}

			free(input);

			instance->c_LastData.nStartPosition = instance->c_LastData.nEndPosition;
			instance->c_LastData.nEndPosition += nHaveSamples;


			instance->c_LastData.pSamples = output;
			instance->c_LastData.nNumberOfSamples = nHaveSamples;
			instance->c_LastData.nBlockAllign = instance->c_nBlockAlign;


	

				// send data to next processor
			if(instance->c_next)
			{
				if(instance->c_next->PushSamples(&instance->c_LastData) == 0)
				{
					// error occurs
					free(output);
					return 0;
				}
			}
			else if (instance->c_output)
			{
				if(instance->c_output(&instance->c_LastData) == 0)
				{
					// error occurs
					free(output);
					strcpy(instance->c_pchErrorMessageStr, "WCenterCutProcessor::Flush->Output function return 0.");
					if(instance->c_pchReturnError)
						strcpy(instance->c_pchReturnError, instance->c_pchErrorMessageStr);
					return 0;
				}
			}
			else
			{
				if(instance->c_queue.PushSamples(&instance->c_LastData) == 0)
				{
					free(output);
					strcpy(instance->c_pchErrorMessageStr, "WCenterCutProcessor::Flush->Can't add data to queue.");
					if(instance->c_pchReturnError)
						strcpy(instance->c_pchReturnError, instance->c_pchErrorMessageStr);
					return 0;
				}
			}
	
			free(output);
			return 1;
		}

	return 1;
}


int WCenterCutProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WCenterCutProcessor::PushSamples\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);

	
	if(c_fEnable && c_nChannel != 0)
	{

		unsigned int nDataSize = data->nNumberOfSamples * data->nBlockAllign;

		// reallocate output buffer
		if(c_nOutputBufferSize < nDataSize)
		{
			char *buffer = (char*) malloc(nDataSize);
			if(buffer == 0)
			{
				strcpy(c_pchErrorMessageStr, "WCenterCutProcessor::PushData->Memory allocation error.");

				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);

				LeaveCriticalSection(&c_CriticalSection);
				return 0;
			}

			if(c_pchOutputBuffer)
				free(c_pchOutputBuffer);

			c_pchOutputBuffer = buffer;
			c_nOutputBufferSize = nDataSize;
		}



		int nRet = CenterCutProcessSamples(this, (unsigned char *) data->pSamples, data->nNumberOfSamples, (unsigned char *) c_pchOutputBuffer, data->nNumberOfSamples);
	
		c_nLatencyInSamples = c_nLatencyInSamples + ((int) data->nNumberOfSamples - nRet); 

		if(nRet == 0)
		{
			LeaveCriticalSection(&c_CriticalSection);
			return 1;
		}

		data->pSamples = c_pchOutputBuffer;
		data->nNumberOfSamples = nRet;

		if(c_nLatencyInSamples)
		{
			// need to fix positions

			if(data->nEndPosition >= data->nStartPosition)
			{
				if(data->nEndPosition >= c_nLatencyInSamples)
					data->nEndPosition -= c_nLatencyInSamples;

				if(data->nStartPosition >= c_nLatencyInSamples)
					data->nStartPosition -= c_nLatencyInSamples;
			}
			else
			{
				data->nEndPosition += c_nLatencyInSamples;
				data->nStartPosition += c_nLatencyInSamples;
			}
		}
	}

	memcpy(&c_LastData, data, sizeof(PROCESSOR_AUDIO_DATA));

	LeaveCriticalSection(&c_CriticalSection);

	
	// send data to next processor
	if(c_next)
	{	
		#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
			if(c_pchReturnError == 0)
				c_next->c_pchReturnError = c_pchErrorMessageStr;
			else
				c_next->c_pchReturnError = c_pchReturnError;
		#endif
				
		return c_next->PushSamples(data);
	}


	// send data to output function
	if(c_output)
	{			
		if(c_output(data) == 0)
		{
			strcpy(c_pchErrorMessageStr, "WCenterCutProcessor::PushData->Output function return 0.");
			if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
			return 0;
		}

		return 1;
	}

	if(c_queue.PushSamples(data) == 0)
	{
		strcpy(c_pchErrorMessageStr, "WCenterCutProcessor::PushData->Can't add data to queue.");
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}


	return 1;	
}