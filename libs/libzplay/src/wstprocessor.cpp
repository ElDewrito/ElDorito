/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  AUdio pitch and time stretching processor
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wstprocessor.h"
#include "debug.h"



WSoundTouchProcessor::WSoundTouchProcessor()
{

	st.setSetting(SETTING_USE_QUICKSEEK, 0);
	st.setSetting(SETTING_USE_AA_FILTER, 1);
	st.setSetting(SETTING_AA_FILTER_LENGTH, 32);

	c_nRate = 100;
	c_nPitch = 100;
	c_nTempo = 100;

	memset(&c_LastData, 0, sizeof(PROCESSOR_AUDIO_DATA));


	InitializeCriticalSection(&c_CriticalSection);
		
}

WSoundTouchProcessor::~WSoundTouchProcessor()
{
	DeleteCriticalSection(&c_CriticalSection);
}





int WSoundTouchProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
	// check input parameters
	if(nSampleRate == 0)
	{
		sprintf(c_pchErrorMessageStr, "WSoundTouchProcessor::Configure->Sample rate can't be %u.", nSampleRate);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nChannel != 1 && nChannel != 2)
	{
		sprintf(c_pchErrorMessageStr, "WSoundTouchProcessor::Configure->Nuber of channels can't be %u.", nChannel);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nBitPerSample != 8 && nBitPerSample != 16 && nBitPerSample != 24)
	{
		sprintf(c_pchErrorMessageStr, "WSoundTouchProcessor::Configure->Bit per sample can't be %u.", nBitPerSample);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
	}

	// set new parameters
	c_nSampleRate = nSampleRate;
	c_nChannel = nChannel;
	c_nBitPerSample = nBitPerSample;
	c_nBlockAlign = nChannel * (nBitPerSample / 8);

	EnterCriticalSection(&c_CriticalSection);
	st.setSampleRate(c_nSampleRate);
	st.setChannels(c_nChannel);	
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






int WSoundTouchProcessor::Enable(int fBroadcast, int fEnable)
{

	EnterCriticalSection(&c_CriticalSection);
	if(fEnable)
		c_fEnable = 1;
	else
	{

		unsigned int n = st.numUnprocessedSamples();

		c_fEnable = 0;

		st.flush();


		unsigned int nHaveSamples = st.numSamples();
		if(nHaveSamples)
		{
			if(nHaveSamples > n)
				nHaveSamples = n;

			c_LastData.pSamples =  st.ptrBegin();
			c_LastData.nNumberOfSamples = nHaveSamples;

			c_LastData.nBlockAllign = c_nBlockAlign;

			c_LastData.nStartPosition = c_LastData.nEndPosition;
			c_LastData.nEndPosition += nHaveSamples;

		
			// send data to next processor
			if(c_next)
			{	
				#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
					if(c_pchReturnError == 0)
						c_next->c_pchReturnError = c_pchErrorMessageStr;
					else
						c_next->c_pchReturnError = c_pchReturnError;
				#endif
							
				if(c_next->PushSamples(&c_LastData) == 0)
				{
					LeaveCriticalSection(&c_CriticalSection);
					return 0;
				}
			}

			else if(c_output)
			{			
				if(c_output(&c_LastData) == 0)
				{
					strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::Flush->Output function return 0.");
					if(c_pchReturnError)
							strcpy(c_pchReturnError, c_pchErrorMessageStr);

					LeaveCriticalSection(&c_CriticalSection);
					return 0;
				}
			}
			else if(c_queue.PushSamples(&c_LastData) == 0)
			{
				strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::Flush->Can't add data to queue.");
				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);

				LeaveCriticalSection(&c_CriticalSection);
				return 0;
			}
		}

		st.clear();
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


int WSoundTouchProcessor::SetRate(unsigned int nRate)
{
	if(nRate == 0)
	{
		strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::SetPitch->Rate parameter can't be 0.");
		return 0;
	}

	EnterCriticalSection(&c_CriticalSection);
	c_nRate = nRate;
	st.setRate((float) nRate / 100.0);
	LeaveCriticalSection(&c_CriticalSection);
	return 1;
}


int WSoundTouchProcessor::SetPitch(unsigned int nPitch)
{
	if(nPitch == 0)
	{
		strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::SetPitch->Pitch parameter can't be 0.");
		return 0;
	}


	EnterCriticalSection(&c_CriticalSection);
	c_nPitch = nPitch;
	st.setPitch((float) nPitch / 100.0);
	LeaveCriticalSection(&c_CriticalSection);
	return 1;
}

int WSoundTouchProcessor::SetTempo(unsigned int nTempo)
{
	if(nTempo == 0)
	{
		strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::Tempo->Rate parameter can't be 0.");
		return 0;
	}

	EnterCriticalSection(&c_CriticalSection);
	c_nTempo = nTempo;
	st.setTempo((float) nTempo / 100.0);
	LeaveCriticalSection(&c_CriticalSection);
	return 1;
}







int WSoundTouchProcessor::Flush(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WSoundTouchProcessor::Flush\n");
	#endif



	EnterCriticalSection(&c_CriticalSection);

	unsigned int nLatencyInSamples = st.numUnprocessedSamples();

	st.flush();


	unsigned int nHaveSamples = st.numSamples();
	if(nHaveSamples)
	{

		
		c_LastData.pSamples =  st.ptrBegin();

		if(nHaveSamples > nLatencyInSamples)
			nHaveSamples = 	nLatencyInSamples;

		c_LastData.nNumberOfSamples = nHaveSamples;

		c_LastData.nBlockAllign = c_nBlockAlign;

		c_LastData.nStartPosition = c_LastData.nEndPosition;
		c_LastData.nEndPosition += nHaveSamples;

		

		// send data to next processor
		if(c_next)
		{	
			#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
				if(c_pchReturnError == 0)
					c_next->c_pchReturnError = c_pchErrorMessageStr;
				else
					c_next->c_pchReturnError = c_pchReturnError;
			#endif
						
			if(c_next->PushSamples(&c_LastData) == 0)
			{
				LeaveCriticalSection(&c_CriticalSection);
				return 0;
			}
		}

		else if(c_output)
		{			
			if(c_output(&c_LastData) == 0)
			{
				strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::Flush->Output function return 0.");
				if(c_pchReturnError)
						strcpy(c_pchReturnError, c_pchErrorMessageStr);

				LeaveCriticalSection(&c_CriticalSection);
				return 0;
			}
		}
		else if(c_queue.PushSamples(&c_LastData) == 0)
		{
			strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::Flush->Can't add data to queue.");
			if(c_pchReturnError)
				strcpy(c_pchReturnError, c_pchErrorMessageStr);

			LeaveCriticalSection(&c_CriticalSection);
			return 0;
		}
	}

	st.clear();


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


int WSoundTouchProcessor::Clear(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WSoundTouchProcessor::Clear\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);
	// clear soundtouch processor
	st.clear();
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




int WSoundTouchProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WSoundTouchProcessor::PushSamples\n");
	#endif

	ASSERT_W(c_nBlockAlign);

	EnterCriticalSection(&c_CriticalSection);

	if(c_fEnable)
	{

		st.putSamples((SAMPLETYPE*) data->pSamples, data->nNumberOfSamples);

		unsigned int nHaveSamples = st.numSamples();


		if(nHaveSamples == 0)
		{
			LeaveCriticalSection(&c_CriticalSection);
			return 1;
		}


		

		data->pSamples = st.ptrBegin();
		data->nNumberOfSamples = nHaveSamples;

		st.receiveSamples(nHaveSamples);

		unsigned int nLatencyInSamples = st.numUnprocessedSamples();


		if(nLatencyInSamples)
		{
			// need to fix positions

			if(data->nEndPosition >= data->nStartPosition)
			{
				if(data->nEndPosition >= nLatencyInSamples)
					data->nEndPosition -= nLatencyInSamples;

				if(data->nStartPosition >= nLatencyInSamples)
					data->nStartPosition -= nLatencyInSamples;
			}
			else
			{
				data->nEndPosition += nLatencyInSamples;
				data->nStartPosition += nLatencyInSamples;
			}
		}

		memcpy(&c_LastData, data, sizeof(PROCESSOR_AUDIO_DATA));

	}

	

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
			strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::PushData->Output function return 0.");
			if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
			return 0;
		}

		return 1;
	}


	if(c_queue.PushSamples(data) == 0)
	{
		strcpy(c_pchErrorMessageStr, "WSoundTouchProcessor::PushData->Can't add data to queue.");
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	return 1;	
}