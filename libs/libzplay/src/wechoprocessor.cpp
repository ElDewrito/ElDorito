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

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "debug.h"
#include "wechoprocessor.h"



WEchoProcessor::WEchoProcessor()
{
	
	c_echoEffects = (ECHO_EFFECT_INTERNAL*) malloc(sizeof(ECHO_EFFECT_INTERNAL));
	c_echoEffects[0].nLeftDelay = 1000;
	c_echoEffects[0].nLeftSrcVolume = 70;
	c_echoEffects[0].nLeftEchoVolume = 30;
	c_echoEffects[0].nRightDelay = 1000;
	c_echoEffects[0].nRightSrcVolume = 70;
	c_echoEffects[0].nRightEchoVolume = 30;
	c_echoEffects[0].nLeftDelayBackup = 1000;
	c_echoEffects[0].nRightDelayBackup = 1000;
	c_nEchoEffectsNumber = 1;

	c_nEchoMaxDelay = 0;



	c_nInputBufferSize = 0;
	c_nOldEchoMaxDelay = 0;
	c_pchOutput = 0;
	c_nOutputBufferSize = 0;
	InitializeCriticalSection(&c_CriticalSection);	
}

WEchoProcessor::~WEchoProcessor()
{
//	FREE ECHO DELAY LINE
	if(c_echoEffects)
		free(c_echoEffects);

	c_dlEchoDelayLine.Free();
	if(c_pchOutput)
		free(c_pchOutput);

	DeleteCriticalSection(&c_CriticalSection);
}




int WEchoProcessor::SetParameters(ECHO_DELAY_EFFECT *pEchoEffect, int nNumberOfEffects)
{
	if(nNumberOfEffects == 0)
	{
		strcpy(c_pchErrorMessageStr, "WEchoProcessor::SetParameters->Number of effects can't be 0.");
		return 0;
	}

	if(pEchoEffect == 0)
	{
		strcpy(c_pchErrorMessageStr, "WEchoProcessor::SetParameters->pEchoEffect parameter san't be 0.");
		return 0;
	}


	// allocate memory for echo effects
	ECHO_EFFECT_INTERNAL *effect = (ECHO_EFFECT_INTERNAL*) malloc(nNumberOfEffects * sizeof(ECHO_EFFECT_INTERNAL));
	if(effect == 0)
	{
		strcpy(c_pchErrorMessageStr, "WEchoProcessor::SetParameters->Memory allocation error.");
		return 0;
	}

	int i;

	// if Configure data set
	if(c_nBlockAlign)
	{
		// fill internal effects array
		for(i = 0; i < nNumberOfEffects; i++)
		{
			effect[i].nLeftSrcVolume = pEchoEffect[i].nLeftSrcVolume;
			effect[i].nRightSrcVolume = pEchoEffect[i].nRightSrcVolume;

			effect[i].nLeftEchoVolume = pEchoEffect[i].nLeftEchoVolume;
			effect[i].nRightEchoVolume = pEchoEffect[i].nRightEchoVolume;

			effect[i].nLeftDelayBackup = pEchoEffect[i].nLeftDelay;
			effect[i].nRightDelayBackup = pEchoEffect[i].nRightDelay;

			// convert delay from milliseconds to bytes
			effect[i].nLeftDelay = 	MulDiv(pEchoEffect[i].nLeftDelay, c_nSampleRate, 1000) * c_nBlockAlign;
			effect[i].nRightDelay = MulDiv(pEchoEffect[i].nRightDelay, c_nSampleRate, 1000) * c_nBlockAlign;


		}

		// calculate maximum delay
		int nMaxDelay = 0;
			
		for(i = 0; i < nNumberOfEffects; i++)
		{
			if(effect[i].nLeftDelay > nMaxDelay)
				nMaxDelay = effect[i].nLeftDelay;

			if(effect[i].nRightDelay > nMaxDelay)
				nMaxDelay = effect[i].nRightDelay;

		}

		EnterCriticalSection(&c_CriticalSection);

		if(c_echoEffects)
			free(c_echoEffects);

		c_echoEffects = effect;
		c_nEchoEffectsNumber = nNumberOfEffects;
		c_nEchoMaxDelay = nMaxDelay;
		LeaveCriticalSection(&c_CriticalSection);
	}
	else
	{
		// fill internal effects array
		for(i = 0; i < nNumberOfEffects; i++)
		{
			
			effect[i].nLeftDelay = 	pEchoEffect[i].nLeftDelay;
			effect[i].nRightDelay = pEchoEffect[i].nRightDelay;

			effect[i].nLeftSrcVolume = pEchoEffect[i].nLeftSrcVolume;
			effect[i].nRightSrcVolume = pEchoEffect[i].nRightSrcVolume;

			effect[i].nLeftEchoVolume = pEchoEffect[i].nLeftEchoVolume;
			effect[i].nRightEchoVolume = pEchoEffect[i].nRightEchoVolume;

			effect[i].nLeftDelayBackup = pEchoEffect[i].nLeftDelay;
			effect[i].nRightDelayBackup = pEchoEffect[i].nRightDelay;


		}

		// calculate maximum delay
	
		EnterCriticalSection(&c_CriticalSection);

		if(c_echoEffects)
			free(c_echoEffects);

		c_echoEffects = effect;
		c_nEchoEffectsNumber = nNumberOfEffects;
		c_nEchoMaxDelay = 0;
		LeaveCriticalSection(&c_CriticalSection);
	}


	return 1;

}


int WEchoProcessor::GetParameters(ECHO_DELAY_EFFECT *pEchoEffect, int nNumberOfEffects)
{
	if(pEchoEffect == 0 || nNumberOfEffects == 0)
		return c_nEchoEffectsNumber;


	if(c_echoEffects == 0)
		return 0;

	unsigned int i;	
	unsigned int num = min(nNumberOfEffects, c_nEchoEffectsNumber);	
	for(i = 0; i < num; i++)
	{
		pEchoEffect[i].nLeftDelay = c_echoEffects[i].nLeftDelayBackup;
		pEchoEffect[i].nRightDelay = c_echoEffects[i].nRightDelayBackup;

		pEchoEffect[i].nLeftSrcVolume = c_echoEffects[i].nLeftSrcVolume;
		pEchoEffect[i].nRightSrcVolume = c_echoEffects[i].nRightSrcVolume;

		pEchoEffect[i].nLeftEchoVolume = c_echoEffects[i].nLeftEchoVolume;
		pEchoEffect[i].nRightEchoVolume = c_echoEffects[i].nRightEchoVolume;
	}

	return num;

}

int WEchoProcessor::Enable(int fBroadcast, int fEnable)
{
	if(fEnable)
	{
		EnterCriticalSection(&c_CriticalSection);

		unsigned int i;
		// convert echo delay from millseconds to bytes
		if(c_echoEffects && c_nEchoMaxDelay == 0 && c_nBlockAlign)
		{
			for(i = 0; i < c_nEchoEffectsNumber; i++)
			{
				// convert delay from milliseconds to bytes
				c_echoEffects[i].nLeftDelay = MulDiv(c_echoEffects[i].nLeftDelay, c_nSampleRate, 1000) * c_nBlockAlign;
				c_echoEffects[i].nRightDelay = MulDiv(c_echoEffects[i].nRightDelay, c_nSampleRate, 1000) * c_nBlockAlign;
			}
		

			// calculate maximum delay
			int nMaxDelay = 0;	
			for(i = 0; i < c_nEchoEffectsNumber; i++)
			{
				if(c_echoEffects[i].nLeftDelay > nMaxDelay)
					nMaxDelay = c_echoEffects[i].nLeftDelay;

				if(c_echoEffects[i].nRightDelay > nMaxDelay)
					nMaxDelay = c_echoEffects[i].nRightDelay;
			}

			c_nEchoMaxDelay = nMaxDelay;
		}
		
		c_fEnable = 1;
		LeaveCriticalSection(&c_CriticalSection);

	}
	else
	{
		// disable echo
		EnterCriticalSection(&c_CriticalSection);
		c_fEnable = 0;

		c_nInputBufferSize = 0;
		c_dlEchoDelayLine.Free();
		LeaveCriticalSection(&c_CriticalSection);
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


int WEchoProcessor::Clear(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WEchoProcessor::Clear\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);
	// clear output queue
	c_queue.Clear();

	// clear delay line
	c_dlEchoDelayLine.Clear();

	if(c_pchOutput)
		free(c_pchOutput);

	c_pchOutput = 0;
	c_nOutputBufferSize = 0;
		

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

int WEchoProcessor::Flush(int fBroadcast)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WEchoProcessor::Flush\n");
	#endif

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


int WEchoProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
	// check input parameters
	if(nSampleRate == 0)
	{
		sprintf(c_pchErrorMessageStr, "WEchoProcessor::Configure->Sample rate can't be %u.", nSampleRate);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nChannel != 1 && nChannel != 2)
	{
		sprintf(c_pchErrorMessageStr, "WEchoProcessor::Configure->Nuber of channels can't be %u.", nChannel);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}

	if(nBitPerSample != 8 && nBitPerSample != 16 && nBitPerSample != 24)
	{
		sprintf(c_pchErrorMessageStr, "WEchoProcessor::Configure->Bit per sample can't be %u.", nBitPerSample);
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
	}

	// set new parameters
	c_nSampleRate = nSampleRate;
	c_nChannel = nChannel;
	c_nBitPerSample = nBitPerSample;
	c_nBlockAlign = nChannel * (nBitPerSample / 8);




	unsigned int i;
	// convert echo delay from millseconds to bytes
	if(c_echoEffects && c_nEchoMaxDelay == 0 && c_fEnable)
	{
		for(i = 0; i < c_nEchoEffectsNumber; i++)
		{
			// convert delay from milliseconds to bytes
			c_echoEffects[i].nLeftDelay = MulDiv(c_echoEffects[i].nLeftDelay, c_nSampleRate, 1000) * c_nBlockAlign;
			c_echoEffects[i].nRightDelay = MulDiv(c_echoEffects[i].nRightDelay, c_nSampleRate, 1000) * c_nBlockAlign;
		}
		

		// calculate maximum delay
		int nMaxDelay = 0;	
		for(i = 0; i < c_nEchoEffectsNumber; i++)
		{
			if(c_echoEffects[i].nLeftDelay > nMaxDelay)
				nMaxDelay = c_echoEffects[i].nLeftDelay;

			if(c_echoEffects[i].nRightDelay > nMaxDelay)
				nMaxDelay = c_echoEffects[i].nRightDelay;
		}

		c_nEchoMaxDelay = nMaxDelay;
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


int WEchoProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WEcho::PushSamples\n");
	#endif

	EnterCriticalSection(&c_CriticalSection);

	unsigned int nDataSize = data->nNumberOfSamples * data->nBlockAllign;
	char *pchData = (char*) data->pSamples;


	if(c_fEnable && c_nEchoMaxDelay)
	{
		
		// =========================================================================================
		// check if we need to reallocate delay line

		if(c_nEchoMaxDelay > c_nOldEchoMaxDelay || nDataSize > c_nInputBufferSize)
		{
			if(c_dlEchoDelayLine.ReAllocate(c_nEchoMaxDelay, nDataSize) == 0)
			{
				strcpy(c_pchErrorMessageStr, "WEchoProcessor::PushSamples->Memory allocation error.");

				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);

				LeaveCriticalSection(&c_CriticalSection);
				return 0;
			}
				
			c_nOldEchoMaxDelay = c_nEchoMaxDelay;
			c_nInputBufferSize = nDataSize;

		}
		// =========================================================================================
		// check if we need to reallocate output buffer
		if(nDataSize > c_nOutputBufferSize)
		{
			char *buffer = (char*) malloc(nDataSize);
			if(buffer == 0)
			{
				LeaveCriticalSection(&c_CriticalSection);
				strcpy(c_pchErrorMessageStr, "WEchoProcessor::PushSamples->Memory allocation error.");
				if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
				return 0;
			}

			if(c_pchOutput)
				free(c_pchOutput);

			c_pchOutput = buffer;
			c_nOutputBufferSize = nDataSize;
		}



		// add data into echo queue
		if(c_dlEchoDelayLine.AddData(pchData, nDataSize) == 0)
		{
			LeaveCriticalSection(&c_CriticalSection);
			strcpy(c_pchErrorMessageStr, "WEchoProcessor::PushSamples->Memory allocation error.");
			if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
			return 0;
		}
		// do echo processing


		unsigned int i;
		unsigned int j;
		// get delay buffers
		for(i = 0; i < c_nEchoEffectsNumber; i++)
		{
			c_echoEffects[i].int_buffer_left = c_dlEchoDelayLine.GetData(c_echoEffects[i].nLeftDelay);
			c_echoEffects[i].int_buffer_right = c_dlEchoDelayLine.GetData(c_echoEffects[i].nRightDelay);
		}

		int original_sample;
		int echo_sample;
		int destination_sample;

		short *input_sample;
		short *output_sample;

		if(c_nChannel == 2)	// stereo
		{
			// left channel
			for(i = 0; i < nDataSize; i += 4)
			{
				input_sample = (short*) (pchData + i);
				output_sample = (short*) (c_pchOutput + i); 
				destination_sample = 0;
				original_sample = (int) (*input_sample);


				for(j = 0; j < c_nEchoEffectsNumber; j++)
				{
					echo_sample = (int) (*((short*) (c_echoEffects[j].int_buffer_left + i)));
					destination_sample += original_sample * c_echoEffects[j].nLeftSrcVolume / 100 + 
											echo_sample * c_echoEffects[j].nLeftEchoVolume / 100;
				}

				if(destination_sample > 32767)
					destination_sample = 32767;
				else if	(destination_sample < -32768)
					destination_sample = -32768;

				*output_sample = (short) destination_sample;
				
			}

			// right channel
			for(i = 2; i < nDataSize; i += 4)
			{
				input_sample = (short*) (pchData + i);
				output_sample = (short*) (c_pchOutput + i); 
				destination_sample = 0;
				original_sample = (int) (*input_sample);

				for(j = 0; j < c_nEchoEffectsNumber; j++)
				{
					echo_sample = (int) (*((short*) (c_echoEffects[j].int_buffer_right + i)));
					destination_sample += original_sample * c_echoEffects[j].nRightSrcVolume / 100 + 
											echo_sample * c_echoEffects[j].nRightEchoVolume / 100;
				}

				if(destination_sample > 32767)
					destination_sample = 32767;
				else if	(destination_sample < -32768)
					destination_sample = -32768;

				*output_sample = (short) destination_sample;
			
			}
		}
		else	// mono
		{
			for(i = 0; i < nDataSize; i += 2)
			{
				input_sample = (short*) (pchData + i);
				output_sample = (short*) (c_pchOutput + i); 
				destination_sample = 0;
				original_sample = (int) (*input_sample);

				for(j = 0; j < c_nEchoEffectsNumber; j++)
				{
					echo_sample = (int) (*((short*) (c_echoEffects[j].int_buffer_left + i)));
					destination_sample += original_sample * c_echoEffects[j].nLeftSrcVolume / 100 + 
											echo_sample * c_echoEffects[j].nLeftEchoVolume / 100;
				}

				if(destination_sample > 32767)
					destination_sample = 32767;
				else if	(destination_sample < -32768)
					destination_sample = -32768;

				*output_sample = (short) destination_sample;	
			}
		}


		data->pSamples = c_pchOutput;	
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
			strcpy(c_pchErrorMessageStr, "WEchoProcessor::PushSamples->Output function return 0.");
			if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
			return 0;
		}

		return 1;
	}

	if(c_queue.PullSamples(data) == 0)
	{
		strcpy(c_pchErrorMessageStr, "WEchoProcessor::PushSamples->Can't add data to queue.");
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}


	return 1;
}