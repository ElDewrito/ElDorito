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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "weqprocessor.h"
#include "debug.h"





// ==================================================
// CONSTANTS


#define NUMBER_OF_DEFAULT_BANDS 10
const int cnDefaultBandPoints[NUMBER_OF_DEFAULT_BANDS - 1] = {
	115, 240, 455, 800, 2000, 4500, 9000, 13000, 15000
};


#define WB 14
// tabsize = 1 << WB 
const int TabSize  = 16384;
// winlen = (1 << (WB - 1)) - 1;
const int WinLen = 8191;

#define FACT 15
const REAL Factorial[FACT + 1] = {1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0, 40320.0, 362880.0,
								3628800.0, 39916800.0, 479001600.0, 6227020800.0,
								87178291200.0, 1307674368000.0};       



const REAL aa = 96.0;
// AlphaAA = alpha(aa);
const REAL AlphaAA = 9.620460; 
// iza = izero(alpha(aa));
REAL ZerroA = 1965.166016;

// Latency = Tabsize - 4098;
const int Latency = 12286;

// =================================================
// GLOBAL VARIABLES
int WinTableValid = 0;
REAL WinTable[WinLen];
void CreateWinTable();

// ===================================================




#define NCH 2
#define DITHERLEN 65536
#define RINT(x) ((x) >= 0 ? ((int)((x) + 0.5)) : ((int)((x) - 0.5)))


static REAL win(REAL n, int N);

static REAL alpha(REAL a)
{
  if (a <= 21) return 0;
  if (a <= 50) return (REAL)  ( 0.5842 * pow( a - 21.0, 0.4) + 0.07886 * (a - 21));
  return (REAL) (0.1102 * (a - 8.7));
}

static REAL izero(REAL x)
{
  REAL ret = 1;
  int m;

  for(m = 1; m <= FACT; m++)
  {
      REAL t;
      t = (REAL)  pow(x / 2, m ) / Factorial[m];
      ret += t * t;
  }

  return ret;
}


WEqProcessor::WEqProcessor()
{

	lires1 = NULL;
	lires2 = NULL;
	rires1 = NULL;
	rires2 = NULL;
	irest = NULL;
	fsamples = NULL;
	inbuf = NULL;
	outbuf = NULL;
	ditherbuf = NULL;
	c_bit_reversal = NULL;			// working area for bit reversal
	c_cos_sin_table = NULL;			// cos/sin table

	nbufsamples = 0;
	hm1 = 0;
	hm2 = 0;
	ditherptr = 0;
	weq_enable = 1;

	dither = 0;

	// create win table
	CreateWinTable();

	// internal gain/band parameter
	c_eqParam = NULL;
	c_nNumberOfBands = 0;

	c_nSampleRate = 44100;
	c_nPreampGain = 0;


	_CreateDefaultBands(this);
	

// =====================================
	// INITIALIZE EQUALIZER
	c_fEnable = 0;
	c_nOldSampleRate = 0;	


	// allocate memory

	c_pchOutput = 0;
	c_nOutputBufferSize = 0;

	c_nMaxLatencySamples = Latency;
	c_nCurrentEqLatency = Latency;



	memset(&c_LastData, 0, sizeof(PROCESSOR_AUDIO_DATA));

	c_nLatencyInSamples = 0;


	InitializeCriticalSection(&c_CriticalSection);
		
}

WEqProcessor::~WEqProcessor()
{

	if(c_pchOutput)
		free(c_pchOutput);

	DeleteCriticalSection(&c_CriticalSection);


	// free internal memory
	_FreeInternalMemory(this);
	// free bands memory
	_DestroyBands(this);


}


int WEqProcessor::GetEqBands(int *pnFreqPoint, int nFreqPointNumber)
{
	ASSERT_W(c_eqParam);

	if(pnFreqPoint == 0 || nFreqPointNumber == 0)
		return c_nNumberOfBands - 1;

	int i;
	int num = min(nFreqPointNumber, c_nNumberOfBands - 1);
	for(i = 0; i < num; i++)
		pnFreqPoint[i] = (int) c_eqParam[i].nFreqPoint;

	return num;

}


int WEqProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
	// check input parameters
	if(nSampleRate == 0)
	{
		sprintf(c_pchErrorMessageStr, "WEqProcessor::Configure->Sample rate can't be %u.", nSampleRate);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nChannel != 1 && nChannel != 2)
	{
		sprintf(c_pchErrorMessageStr, "WEqProcessor::Configure->Number of channels can't be %u.", nChannel);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nBitPerSample != 8 && nBitPerSample != 16 && nBitPerSample != 24)
	{
		sprintf(c_pchErrorMessageStr, "WEqProcessor::Configure->Bit per sample can't be %u.", nBitPerSample);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
	}

	// set new parameters
	c_nSampleRate = nSampleRate;
	c_nChannel = nChannel;
	c_nBitPerSample = nBitPerSample;
	c_nBlockAlign = nChannel * (nBitPerSample / 8);

	// create equ tables
	if(c_fEnable)
	{
		if(c_nOldSampleRate != c_nSampleRate && c_nSampleRate != 0)
		{
			_MakeEqTables(this);
			c_nOldSampleRate = c_nSampleRate;
		}
	}


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



int WEqProcessor::SetParameters(int nPreAmpGain, int *pnBandGain, int nNumberOfBands)
{

	ASSERT_W(c_eqParam);

	// check input parameters
	if(pnBandGain == 0)
	{
		strcpy(c_pchErrorMessageStr, "WEqProcessor::SetParameters->pnBandGain parameter can be 0.");
		return 0;
	}

	if(nNumberOfBands != c_nNumberOfBands)
	{
		sprintf(c_pchErrorMessageStr, "WEqProcessor::SetParameters->Number of bands must be %u.", c_nNumberOfBands);
		return 0;
	}


	c_nPreampGain = nPreAmpGain;
	int i;
	for(i = 0; i < nNumberOfBands; i++)
		c_eqParam[i].nGain = pnBandGain[i];
	
	
	if(c_fEnable)
		_MakeEqTables(this);
	
	return 1;
}






int WEqProcessor::GetParameters(int *pnPreAmpGain, int *pnBandGain, int nNumberOfBands)
{
	ASSERT_W(c_eqParam);

	if(pnPreAmpGain == 0 || pnBandGain == 0 || nNumberOfBands == 0)
		return c_nNumberOfBands;

	int i;
	int num = min(nNumberOfBands, c_nNumberOfBands);
	for(i = 0; i < num; i++)
		pnBandGain[i] = c_eqParam[i].nGain;
		
	*pnPreAmpGain = c_nPreampGain;
	return num;

}



int WEqProcessor::Enable(int fBroadcast, int fEnable)
{
	ASSERT_W(c_eqParam);

	if(fEnable && c_fEnable)
		return 1;

	if(fEnable)
	{
		EnterCriticalSection(&c_CriticalSection);
		c_nLatencyInSamples = 0;

		// allocate internal memory
		if(_AllocateInternalMemory(this) == 0)
		{
			strcpy(c_pchErrorMessageStr, "WEqProcessor::Enable->Memory allocation error.");
			LeaveCriticalSection(&c_CriticalSection);
			return 0;

		}

		// create equ tables, need valid c_eqParam and c_nSampleRate
		_MakeEqTables(this);

		// set current latency, need this to fix equalizer latency
		c_nMaxLatencySamples = Latency;
		c_nCurrentEqLatency = Latency;

		c_fEnable = 1;
		LeaveCriticalSection(&c_CriticalSection);

	}
	else
	{	
		c_fEnable = 0;
		Flush(0);
		
	}


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



int  WEqProcessor::CreateEqBands(int *pnFreqPoint,int nNumOfPoints)
{

	if(pnFreqPoint == 0 || nNumOfPoints == 0)// reset parameters to default values
	{
		_CreateDefaultBands(this);
	}
	else
	{
		// create user bands
		EQ_INTERNAL_PARAM *tmp = (EQ_INTERNAL_PARAM*) LocalAlloc(LMEM_FIXED, (nNumOfPoints + 1) * sizeof(EQ_INTERNAL_PARAM));

		if(tmp == NULL)
		{
			strcpy(c_pchErrorMessageStr, "WEqProcessor::Memory allocation error!");
			return 0;
		}
	
		int i;
		for(i = 0; i <	nNumOfPoints; i++)
		{
			tmp[i].nFreqPoint = pnFreqPoint[i];
			tmp[i].gain = 1.0;
			tmp[i].upper = (REAL) pnFreqPoint[i];
			tmp[i].nGain = 0;
		}

		tmp[nNumOfPoints].nFreqPoint = c_nSampleRate;
		tmp[nNumOfPoints].gain = 1.0;
		tmp[nNumOfPoints].upper = (REAL) c_nSampleRate;
		tmp[nNumOfPoints].nGain = 0;

		EQ_INTERNAL_PARAM *old_param = c_eqParam;

		c_eqParam = tmp;
		c_nNumberOfBands = nNumOfPoints + 1;
		if(old_param)
			LocalFree(old_param);
	}


	if(c_fEnable)
		_MakeEqTables(this);

	return 1;
}





int WEqProcessor::Flush(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WEqProcessor::Flush\n");
	#endif


	EnterCriticalSection(&c_CriticalSection);

	if(c_nCurrentEqLatency < c_nMaxLatencySamples)
	{

		// allocate flash buffer
		char *buf = (char*) malloc(c_nMaxLatencySamples * c_nBlockAlign);
		if(buf == 0)
		{
			strcpy(c_pchErrorMessageStr, "WEqProcessor::Flush->Memory allocation error.");
			if(c_pchReturnError)
				strcpy(c_pchReturnError, c_pchErrorMessageStr);

			LeaveCriticalSection(&c_CriticalSection);
			return 0;
		}
			
			
		memset(buf, 0, c_nMaxLatencySamples * c_nBlockAlign);
		if(c_fEnable == 0)
			weq_enable = 0;
		_ModifySamples(this, (char *) buf, buf, c_nMaxLatencySamples, c_nChannel, c_nBitPerSample); 
		weq_enable = 1;
		_ClearBuffer(this);

		c_LastData.nStartPosition = c_LastData.nEndPosition;
		c_LastData.nEndPosition += c_nMaxLatencySamples;

		c_LastData.pSamples = buf;
		c_LastData.nNumberOfSamples = c_nMaxLatencySamples;
		c_LastData.nBlockAllign = c_nBlockAlign;

		if(c_next)
		{
			if(c_next->PushSamples(&c_LastData) == 0)
			{
				free(buf);
				LeaveCriticalSection(&c_CriticalSection);
				return 0;
			}

			free(buf);
		}
		else if (c_output)
		{
			if(c_output(&c_LastData) == 0)
			{
				free(buf);
				strcpy(c_pchErrorMessageStr, "WEqProcessor::Flush->Output function return 0.");
				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
					LeaveCriticalSection(&c_CriticalSection);
					return 0;
			}

			free(buf);	
		}
		else
		{
			if(c_queue.PushSamples(&c_LastData) == 0)
			{
					strcpy(c_pchErrorMessageStr, "WEqProcessor::Flush->Can't add data to queue.");
					if(c_pchReturnError)
						strcpy(c_pchReturnError, c_pchErrorMessageStr);
					LeaveCriticalSection(&c_CriticalSection);
					return 0;
			}
	
		}
	}


	c_nLatencyInSamples = 0;

	LeaveCriticalSection(&c_CriticalSection);

	c_nCurrentEqLatency = Latency;


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


int WEqProcessor::Clear(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WEqProcessor::Clear\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);
	

	if(c_fEnable)
		_ClearBuffer(this);


	c_nCurrentEqLatency = Latency;

	c_nLatencyInSamples = 0;

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



// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================





// -(N-1)/2 <= n <= (N-1)/2
static REAL win(REAL n, int N)
{
  return izero(AlphaAA * sqrt(1 - 4 * n * n / ((N - 1) * (N - 1)))) / ZerroA;
}

static REAL sinc(REAL x)
{
  return x == 0 ? 1 : sin(x) / x;
}


static REAL hn_lpf(int n, REAL f, REAL fs)
{

  REAL t = 1 / fs;
  REAL omega = 2 * PI * f;
  return 2 * f * t * sinc(n * omega * t);
}


static REAL hn_imp(int n)
{
  return n == 0 ? 1.0 : 0.0;
}




int WEqProcessor::_CreateDefaultBands(WEqProcessor *instance)
{
	EQ_INTERNAL_PARAM *tmp = (EQ_INTERNAL_PARAM*) LocalAlloc(LMEM_FIXED, NUMBER_OF_DEFAULT_BANDS * sizeof(EQ_INTERNAL_PARAM));

	if(tmp == NULL)
	{
		strcpy(instance->c_pchErrorMessageStr, "WEqProcessor::Memory allocation error!");
		return 0;
	}
	
	int i;
	for(i = 0; i <	NUMBER_OF_DEFAULT_BANDS - 1; i++)
	{
		tmp[i].nFreqPoint = cnDefaultBandPoints[i];
		tmp[i].gain = 1.0;
		tmp[i].upper = (REAL) cnDefaultBandPoints[i];
		tmp[i].nGain = 0;
	}

	tmp[NUMBER_OF_DEFAULT_BANDS - 1].nFreqPoint = instance->c_nSampleRate;
	tmp[NUMBER_OF_DEFAULT_BANDS - 1].gain = 1.0;
	tmp[NUMBER_OF_DEFAULT_BANDS - 1].upper = (REAL) instance->c_nSampleRate;
	tmp[NUMBER_OF_DEFAULT_BANDS - 1].nGain = 0;

	EQ_INTERNAL_PARAM *old_param = instance->c_eqParam;

	instance->c_eqParam = tmp;
	instance->c_nNumberOfBands = NUMBER_OF_DEFAULT_BANDS;
	if(old_param)
		free(old_param);
	
	return 1;
}

void WEqProcessor::_DestroyBands(WEqProcessor *instance)
{
	if(instance->c_eqParam)
	{
		LocalFree(instance->c_eqParam);
		instance->c_nNumberOfBands = 0;
	}
}

int WEqProcessor::_AllocateInternalMemory(WEqProcessor *instance)
{
	// free old memory
	_FreeInternalMemory(instance);

	// allocate new memory
	instance->lires1   = (REAL *)  LocalAlloc(LMEM_FIXED, sizeof(REAL) * TabSize);
	instance->lires2   = (REAL *)  LocalAlloc(LMEM_FIXED, sizeof(REAL) * TabSize);
	instance->rires1   = (REAL *)  LocalAlloc(LMEM_FIXED, sizeof(REAL) * TabSize);
	instance->rires2   = (REAL *)  LocalAlloc(LMEM_FIXED, sizeof(REAL) * TabSize);
	instance->irest    = (REAL *)  LocalAlloc(LMEM_FIXED, sizeof(REAL) * TabSize);
	instance->fsamples = (REAL *) LocalAlloc(LMEM_FIXED, sizeof(REAL) * TabSize);
	instance->inbuf    = (short *) LocalAlloc(LPTR, WinLen * NCH * sizeof(int));
	instance->outbuf   = (REAL *) LocalAlloc(LPTR, TabSize * NCH * sizeof(REAL));
	instance->ditherbuf = (REAL *)LocalAlloc(LMEM_FIXED, sizeof(REAL)*DITHERLEN);
	instance->c_bit_reversal = (int*) LocalAlloc(LMEM_FIXED, (UINT)(2 + sqrt((double) TabSize / 2.0)) * sizeof(int));
	instance->c_bit_reversal[0] = 0;
	instance->c_cos_sin_table = (REAL*) LocalAlloc(LMEM_FIXED, TabSize / 2 * sizeof(REAL));		

	if(instance->lires1 == NULL || instance->lires2 == NULL || instance->rires1 == NULL || instance->rires2 == NULL ||
		instance->irest == NULL || instance->fsamples == NULL || instance->inbuf == NULL || instance->outbuf == NULL ||
		 instance->ditherbuf == NULL )
	{
		_FreeInternalMemory(instance);
		return 0;
	}


	
	instance->lires = instance->lires1;
	instance->rires = instance->rires1;
	instance->cur_ires = 1;
	instance->chg_ires = 1;

	int i;
	for(i = 0; i < DITHERLEN; i++)
		instance->ditherbuf[i] = (float(rand()) / RAND_MAX - 0.5);

	instance->nbufsamples = 0;
	instance->hm1 = 0;
	instance->hm2 = 0;
	instance->ditherptr = 0;


	return 1;
}

void WEqProcessor::_FreeInternalMemory(WEqProcessor *instance)
{
	if(instance->lires1 != NULL)
	{
		LocalFree(instance->lires1);
		instance->lires1 = NULL;
	}

	if(instance->lires2 != NULL)
	{
		LocalFree(instance->lires2);
		instance->lires2 = NULL;
	}

	if(instance->rires1 != NULL)
	{
		LocalFree(instance->rires1);
		instance->rires1 = NULL;
	}

	if(instance->rires2 != NULL)
	{
		LocalFree(instance->rires2);
		instance->rires2 = NULL;
	}

	if(instance->irest != NULL)
	{
		LocalFree(instance->irest);
		instance->irest = NULL;
	}

	if(instance->fsamples != NULL)
	{
		LocalFree(instance->fsamples);
		instance->fsamples = NULL;
	}

	if(instance->inbuf != NULL)
	{
		LocalFree(instance->inbuf);
		instance->inbuf = NULL;
	}

	if(instance->outbuf != NULL)
	{
		LocalFree(instance->outbuf);
		instance->outbuf = NULL;
	}

	if(instance->ditherbuf != NULL)
	{
		LocalFree(instance->ditherbuf);
		instance->ditherbuf = NULL;
	}

	if(instance->c_bit_reversal != NULL)
	{
		LocalFree(instance->c_bit_reversal);
		instance->c_bit_reversal = NULL;
	}

	if(instance->c_cos_sin_table != NULL)
	{
		LocalFree(instance->c_cos_sin_table);
		instance->c_cos_sin_table = NULL;
	}
}

void WEqProcessor::_MakeEqTables(WEqProcessor *instance)
{

	int i;
	int j;
	int cires = instance->cur_ires;
	REAL *nires;

	REAL sum;
	REAL lhn1 ;
	REAL lhn2;
	REAL fs = (REAL) instance->c_nSampleRate;
	REAL fs2 = fs / 2.0;

	for(i = 0; i < instance->c_nNumberOfBands; i++)
		instance->c_eqParam[i].gain = pow(10.0 , (double) ((double) instance->c_nPreampGain / 1000.0 + (double) instance->c_eqParam[i].nGain / 1000.0) / 20.0);
	

	instance->c_eqParam[instance->c_nNumberOfBands - 1].upper = instance->c_nSampleRate;

	int fs2index;
	for(fs2index = 0; fs2index <  instance->c_nNumberOfBands;  fs2index++)
	{
		if(instance->c_eqParam[fs2index].upper >= fs2)
			break;
	}	



	// LEFT CHANNEL
	int n;
	for(i = 0; i < WinLen; i++)
	{
	
		n = i - WinLen / 2;
		sum = 0;
		lhn1 = 0;
		for(j = 0; j < fs2index; j++)
		{
			lhn2 = hn_lpf(n, instance->c_eqParam[j].upper, fs);
			sum += instance->c_eqParam[j].gain * (lhn2 - lhn1);
			lhn1 = lhn2;
		}

		sum += instance->c_eqParam[j].gain * (hn_imp(n) - lhn1);
		instance->irest[i] = sum * WinTable[i];
		
	}


	for( ; i < TabSize; i++)
		instance->irest[i] = 0;

	rdft(TabSize, 1, instance->irest, instance->c_bit_reversal, instance->c_cos_sin_table);
	nires = cires == 1 ? instance->lires2 : instance->lires1;

	for(i = 0; i < TabSize; i++)
		nires[i] = instance->irest[i];

	// USE SAME PARAMETERS FOR RIGHT CHANNEL, WE WILL NOT PROVIDE SEPARATE CHANNEL ADJUSTMENT

	nires = cires == 1 ? instance->rires2 : instance->rires1;

	for(i = 0; i < TabSize; i++)
		nires[i] = instance->irest[i];
 
	instance->chg_ires = cires == 1 ? 2 : 1;

}





void CreateWinTable()
{
	if(WinTableValid == 0)
	{
		int i;
		for(i = 0; i < WinLen; i++)
			WinTable[i] = win(i - WinLen / 2, WinLen);

		WinTableValid = 1;
	}
}


int WEqProcessor::_ModifySamples(WEqProcessor *instance, char *input_buf, char *output_buf, int nsamples, int nch, int bps)
{
	int i,p,ch;
	REAL *ires;
	int amax =  (1 << ( bps - 1))-1;
	int amin = -(1 << ( bps - 1));
 
	if (instance->chg_ires)
	{
		instance->cur_ires = instance->chg_ires;
		instance->lires = instance->cur_ires == 1 ? instance->lires1 : instance->lires2;
		instance->rires = instance->cur_ires == 1 ? instance->rires1 : instance->rires2;
		instance->chg_ires = 0;
	}

	p = 0;

	while(instance->nbufsamples + nsamples >= WinLen)
    {
		switch(bps)
		{
			case 8:
				for(i = 0; i < (WinLen - instance->nbufsamples) * nch; i++)
				{
					instance->inbuf[instance->nbufsamples*nch+i] = ((unsigned char *)input_buf)[i+p*nch] - 0x80;
					float s = instance->outbuf[instance->nbufsamples*nch+i];
					if (instance->dither)
					{
						float u;
						s -= instance->hm1;
						u = s;
						s += instance->ditherbuf[(instance->ditherptr++) & (DITHERLEN-1)];
						if (s < amin) s = amin;
						if (amax < s) s = amax;
						s = RINT(s);
						instance->hm1 = s - u;
						((unsigned char *)output_buf)[i+p*nch] = s + 0x80;
					}
					else
					{
						if (s < amin) s = amin;
						if (amax < s) s = amax;
						((unsigned char *)output_buf)[i+p*nch] = RINT(s) + 0x80;
					}
				}

				for(i= WinLen * nch; i < TabSize * nch;i++)
					instance->outbuf[i- WinLen *nch] = instance->outbuf[i];

			break;

			 case 16:
				for(i = 0; i < (WinLen - instance->nbufsamples) * nch; i++)
				{
					instance->inbuf[instance->nbufsamples * nch + i] = ((short *)input_buf)[i + p * nch];
					float s = instance->outbuf[instance->nbufsamples * nch + i];
					if (instance->dither)
					{
						float u;
						s -= instance->hm1;
						u = s;
						s += instance->ditherbuf[(instance->ditherptr++) & (DITHERLEN-1)];
						if (s < amin) s = amin;
						if (amax < s) s = amax;
						s = RINT(s);
						instance->hm1 = s - u;
						((short *)output_buf)[i+p*nch] = s;
					}
					else
					{
						if (s < amin) s = amin;
						if (amax < s) s = amax;
						((short *)output_buf)[i + p * nch] = RINT(s);
					}
				}

				for(i = WinLen * nch; i < TabSize * nch; i++)
					instance->outbuf[i- WinLen * nch] = instance->outbuf[i];

			break;

			case 24:
				for(i=0;i<(WinLen - instance->nbufsamples)*nch;i++)
				{
					((int *)instance->inbuf)[instance->nbufsamples*nch+i] =
						(((unsigned char *)input_buf)[(i+p*nch)*3  ]      ) +
						(((unsigned char *)input_buf)[(i+p*nch)*3+1] <<  8) +
						(((  signed char *)input_buf)[(i+p*nch)*3+2] << 16) ;

					float s = instance->outbuf[instance->nbufsamples*nch+i];
					//if (dither) s += ditherbuf[(ditherptr++) & (DITHERLEN-1)];
					if (s < amin) s = amin;
					if (amax < s) s = amax;
					int s2 = RINT(s);
					((signed char *)output_buf)[(i+p*nch)*3  ] = s2 & 255; s2 >>= 8;
					((signed char *)output_buf)[(i+p*nch)*3+1] = s2 & 255; s2 >>= 8;
					((signed char *)output_buf)[(i+p*nch)*3+2] = s2 & 255;
				}

				for(i= WinLen *nch;i< TabSize*nch;i++)
					instance->outbuf[i- WinLen*nch] = instance->outbuf[i];

			break;

			default:
			return 0;
		}

		p += WinLen - instance->nbufsamples;
		nsamples -= WinLen - instance->nbufsamples;
		instance->nbufsamples = 0;

		for(ch = 0; ch < nch; ch++)
		{
			ires = ch == 0 ? instance->lires : instance->rires;

			if (bps == 24)
			{
				for(i = 0; i < WinLen;i++)
					instance->fsamples[i] = ((int *)instance->inbuf)[nch * i + ch];
			}
			else
			{
				for(i = 0; i < WinLen; i++)
					instance->fsamples[i] = instance->inbuf[nch * i + ch];
			}

			for(i = WinLen; i < TabSize; i++)
				instance->fsamples[i] = 0;

			if (instance->weq_enable)
			{

				rdft(TabSize, 1, instance->fsamples, instance->c_bit_reversal, instance->c_cos_sin_table);

				instance->fsamples[0] = ires[0] * instance->fsamples[0];
				instance->fsamples[1] = ires[1] * instance->fsamples[1]; 
			
				for( i= 1 ;i < TabSize / 2; i++)
				{
					REAL re,im;

					re = ires[i*2  ] * instance->fsamples[i*2] - ires[i*2+1] * instance->fsamples[i*2+1];
					im = ires[i*2+1] * instance->fsamples[i*2] + ires[i*2  ] * instance->fsamples[i*2+1];

					instance->fsamples[i*2  ] = re;
					instance->fsamples[i*2+1] = im;
				}

				rdft(TabSize, -1, instance->fsamples, instance->c_bit_reversal, instance->c_cos_sin_table);

			}
			else
			{
				for(i = WinLen - 1 + WinLen / 2; i >= WinLen / 2; i--)
					instance->fsamples[i] = instance->fsamples[i - WinLen / 2] * TabSize / 2;

				for( ; i >= 0; i--)
					instance->fsamples[i] = 0;
			}

			for(i = 0; i < WinLen; i++)
				instance->outbuf[i * nch + ch] += instance->fsamples[i] / TabSize * 2;

			for(i = WinLen; i < TabSize; i++)
				instance->outbuf[i * nch + ch] = instance->fsamples[i] / TabSize * 2;
		}
    }

	switch(bps)
	{
	
		case 8:
			for(i = 0; i < nsamples * nch; i++)
			{
				instance->inbuf[instance->nbufsamples*nch+i] = ((unsigned char *)input_buf)[i+p*nch] - 0x80;
				float s = instance->outbuf[instance->nbufsamples*nch+i];
				if (instance->dither)
				{
					float u;
					s -= instance->hm1;
					u = s;
					s += instance->ditherbuf[(instance->ditherptr++) & (DITHERLEN-1)];
					if (s < amin) s = amin;
					if (amax < s) s = amax;
					s = RINT(s);
					instance->hm1 = s - u;
					((unsigned char *)output_buf)[i+p*nch] = s + 0x80;
				}
				else
				{
					if (s < amin) s = amin;
					if (amax < s) s = amax;
					((unsigned char *)output_buf)[i+p*nch] = RINT(s) + 0x80;
				}
			}
		break;

		case 16:
			for(i = 0; i < nsamples * nch; i++)
			{
				instance->inbuf[instance->nbufsamples * nch + i] = ((short *)input_buf)[i + p * nch];
				float s = instance->outbuf[instance->nbufsamples * nch + i];
				if (instance->dither)
				{
					float u;
					s -= instance->hm1;
					u = s;
					s += instance->ditherbuf[(instance->ditherptr++) & (DITHERLEN - 1)];
					if (s < amin) s = amin;
					if (amax < s) s = amax;
					s = RINT(s);
					instance->hm1 = s - u;
					((short *)output_buf)[i + p * nch] = s;
				}
				else
				{
					if (s < amin) s = amin;
					if (amax < s) s = amax;
					((short *)output_buf)[i + p * nch] = RINT(s);
				}
			}
		break;

		case 24:
			for(i = 0; i < nsamples * nch; i++)
			{
				((int *)instance->inbuf)[instance->nbufsamples*nch+i] =
					(((unsigned char *)input_buf)[(i+p*nch)*3  ]      ) +
					(((unsigned char *)input_buf)[(i+p*nch)*3+1] <<  8) +
					(((  signed char *)input_buf)[(i+p*nch)*3+2] << 16) ;

				float s = instance->outbuf[instance->nbufsamples*nch+i];
				//if (dither) s += ditherbuf[(ditherptr++) & (DITHERLEN-1)];
				if (s < amin) s = amin;
				if (amax < s) s = amax;
				int s2 = RINT(s);
				((signed char *)output_buf)[(i+p*nch)*3  ] = s2 & 255; s2 >>= 8;
				((signed char *)output_buf)[(i+p*nch)*3+1] = s2 & 255; s2 >>= 8;
				((signed char *)output_buf)[(i+p*nch)*3+2] = s2 & 255;
			}
		break;

	  default:
		return 0;
	}

  p += nsamples;
  instance->nbufsamples += nsamples;

  return p;

}


void WEqProcessor::_ClearBuffer(WEqProcessor *instance)
{
	ASSERT_W(instance->outbuf);

	int i;
	instance->nbufsamples = 0;
	for(i = 0; i < TabSize * NCH; i++)
		instance->outbuf[i] = 0;
}


int WEqProcessor::SetPreampGain(int nGain)
{
	ASSERT_W(c_eqParam);

	c_nPreampGain = nGain;
	if(c_fEnable)
		_MakeEqTables(this);


	return 1;
}


	

int WEqProcessor::GetPreampGain()
{
	ASSERT_W(c_eqParam);

	return c_nPreampGain;
}

int WEqProcessor::SetBandGain(int nBandIndex, int nGain)
{
	ASSERT_W(c_eqParam);

	if(nBandIndex >= c_nNumberOfBands)
	{
		strcpy(c_pchErrorMessageStr, "Invalid input parameter.");
		return 0;
	}

	c_eqParam[nBandIndex].nGain = nGain;
	if(c_fEnable)
		_MakeEqTables(this);	


	return 1;
}


int WEqProcessor::GetBandGain(int nBandIndex)
{
	ASSERT_W(c_eqParam);
	if(nBandIndex >= c_nNumberOfBands)
		return 0;

	return c_eqParam[nBandIndex].nGain;
}

int WEqProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WEqProcessor::PushData\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);


	unsigned int nDataSize = data->nNumberOfSamples * data->nBlockAllign;
	char *pchData = (char*) data->pSamples;

	if(c_fEnable)
	{
		
		// check if we need to reallocate output buffer
		if(c_nOutputBufferSize < nDataSize)
		{
			char *buf = (char*) malloc(nDataSize);
			if(buf == 0)
			{
				strcpy(c_pchErrorMessageStr, "WEqProcessor::PushData->Memory allocation error.");
				LeaveCriticalSection(&c_CriticalSection);
				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
				return 0;
			}
			
			if(c_pchOutput)
				free(c_pchOutput);

			c_pchOutput = buf;
			c_nOutputBufferSize = nDataSize;
		}


		unsigned int nInputSamples = data->nNumberOfSamples;

		_ModifySamples(this, (char *) pchData, c_pchOutput, data->nNumberOfSamples, c_nChannel, c_nBitPerSample);


		if(c_nCurrentEqLatency)
		{
			if(c_nCurrentEqLatency >= data->nNumberOfSamples)
			{
				c_nCurrentEqLatency -= data->nNumberOfSamples;
				c_nLatencyInSamples += data->nNumberOfSamples;
				LeaveCriticalSection(&c_CriticalSection);
				return 1;
			}

			data->pSamples = c_pchOutput + c_nCurrentEqLatency * c_nBlockAlign;
			data->nNumberOfSamples -= c_nCurrentEqLatency;

			c_nLatencyInSamples = c_nLatencyInSamples + (nInputSamples - data->nNumberOfSamples);

			c_nCurrentEqLatency = 0;
		}
		else
		{
			data->pSamples = c_pchOutput;
		}

		
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

	if(c_output)
	{			
		if(c_output(data) == 0)
		{
			strcpy(c_pchErrorMessageStr, "WEqProcessor::PushData->Output function return 0.");
			if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
			return 0;
		}

		return 1;
	}

	if(c_queue.PushSamples(data) == 0)
	{
		strcpy(c_pchErrorMessageStr, "WEqProcessor::PushData->Can't add data to queue.");
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}


	return 1;	
}