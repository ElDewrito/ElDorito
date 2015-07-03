/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  Audio processor base class ( WIN32 )
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
 *
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "waudioprocessor.h"
#include "debug.h"


WAudioProcessor::WAudioProcessor()
{
	c_pchErrorMessageStr[0] = 0;
	c_next = 0;
	c_output = 0;
	c_pchReturnError = 0;

	c_nSampleRate = 0;
	c_nChannel = 0;
	c_nBitPerSample = 0;
	c_nBlockAlign = 0;
	c_fEnable = 0;

}

WAudioProcessor::~WAudioProcessor()
{	

}



int WAudioProcessor::SetOutputProcessor(WAudioProcessor *instance)
{
	if(c_next)
		c_next->c_pchReturnError = 0;

	c_output = 0;
	c_next = instance;
	return 1;
}

int WAudioProcessor::SetOutputFunction(t_audio_processor_output output_func)
{
	if(c_next)
		c_next->c_pchReturnError = 0;

	c_output = output_func;
	c_next = 0;
	return 1;
}


char *WAudioProcessor::GetError()
{
	return c_pchErrorMessageStr;
}




int WAudioProcessor::Clear(int fBroadcast)
{
	strcpy(c_pchErrorMessageStr, "WAudioEffect::Clear->Not implemented in this class.");
	return 0;
}

int WAudioProcessor::Flush(int fBroadcast)
{
	strcpy(c_pchErrorMessageStr, "WAudioEffect::Flush->Not implemented in this class.");
	return 0;
}






int WAudioProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
	strcpy(c_pchErrorMessageStr, "WAudioEffect::Flush->Not implemented in this class.");
	return 0;
}

int WAudioProcessor::Enable(int fBroadcast, int fEnable)
{
	strcpy(c_pchErrorMessageStr, "WAudioEffect::Enable->Not implemented in this class.");
	return 0;
}




int WAudioProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
{
	ASSERT_W(0);
	return 0;
}


WAudioQueue::WAudioQueue()
{
	c_first = NULL;
	c_last = NULL;
	c_nNumberOfElements = 0;
	c_nSumOfSamples = 0;
}

WAudioQueue::~WAudioQueue()
{
	Clear();
}


void WAudioQueue::Clear()
{

	AUDIO_QUEUE_ELEM *elem = c_first;
	while(elem)
	{
		free(elem->pAllocatedBuffer);
		AUDIO_QUEUE_ELEM *tmp = elem;
		elem = elem->next;
		free(tmp);
	}

	c_first = NULL;
	c_last = NULL;
	c_nNumberOfElements = 0;
	c_nSumOfSamples = 0;
}


unsigned int WAudioQueue::PushSamples(PROCESSOR_AUDIO_DATA *data)
{
	if(data == NULL)
		return 0;

	AUDIO_QUEUE_ELEM *elem = (AUDIO_QUEUE_ELEM*) malloc(sizeof(AUDIO_QUEUE_ELEM));
	if(elem == NULL)
		return 0;

	unsigned int nNumberOfBytes = data->nNumberOfSamples * data->nBlockAllign;
	elem->pAllocatedBuffer = malloc(sizeof(PROCESSOR_AUDIO_DATA) + nNumberOfBytes);
	if(elem->pAllocatedBuffer == NULL)
	{
		free(elem);
		return 0;
	}

	memcpy(elem->pAllocatedBuffer, data, sizeof(PROCESSOR_AUDIO_DATA));
	elem->pDataBuffer = (char*) elem->pAllocatedBuffer + sizeof(PROCESSOR_AUDIO_DATA);
	memcpy(elem->pDataBuffer, data->pSamples, nNumberOfBytes);

	c_nNumberOfElements++;
	c_nSumOfSamples += data->nNumberOfSamples;

	elem->next = NULL;

	if(c_first == NULL) // add first element in queue
	{	
		c_first = elem;
		c_last = elem;
		return  data->nNumberOfSamples;
	}

	c_last->next = elem;
	c_last = elem;
	return data->nNumberOfSamples;
}


unsigned int WAudioQueue::PullSamples(PROCESSOR_AUDIO_DATA *data)
{
	if(data == NULL)
		return 0;

	if(c_nSumOfSamples == 0 || data->nNumberOfSamples == 0)
		return 0;


	unsigned int nSamplesRead = 0;
	unsigned int nSamplesNeed = data->nNumberOfSamples;
	char *pBuffer = (char*) data->pSamples;


	PROCESSOR_AUDIO_DATA *d = (PROCESSOR_AUDIO_DATA*) c_first->pAllocatedBuffer;
	data->nStartPosition = d->nStartPosition;
	data->nEndPosition = d->nEndPosition;

	while(1)
	{

		d = (PROCESSOR_AUDIO_DATA*) c_first->pAllocatedBuffer;

		if(d->nNumberOfSamples >= nSamplesNeed) // we have enough data
		{
			memcpy(pBuffer, c_first->pDataBuffer, nSamplesNeed * d->nBlockAllign);

			unsigned int s = d->nNumberOfSamples;
			nSamplesRead += nSamplesNeed;
			d->nNumberOfSamples -= nSamplesNeed;
			c_nSumOfSamples -= nSamplesNeed;

			// check if buffer is empty
			if(d->nNumberOfSamples == 0)	// remove this element from queue
			{
				data->nEndPosition = d->nEndPosition;

				AUDIO_QUEUE_ELEM *elem = c_first->next;
				free(c_first->pAllocatedBuffer);
				free(c_first);
				c_first = elem;
				c_nNumberOfElements--;
					

			}
			else	// element isn't empty, but we have removed some data from element
			{


				if(d->nEndPosition >= d->nStartPosition)
				{
					data->nEndPosition = d->nStartPosition +  (nSamplesNeed * (d->nEndPosition - d->nStartPosition) / s);
					d->nStartPosition = data->nEndPosition;		

				}
				else
				{
					data->nEndPosition = d->nStartPosition - (nSamplesNeed * (d->nStartPosition - d->nEndPosition) / s);
					d->nStartPosition = data->nEndPosition;
				}


				c_first->pDataBuffer = (char*) c_first->pDataBuffer +  nSamplesNeed * d->nBlockAllign;	
					
			}

			return nSamplesRead;
		
		}
		else	// we don't have enough data in this element, use what you have
		{
			memcpy(pBuffer, c_first->pDataBuffer, d->nNumberOfSamples * d->nBlockAllign);

			nSamplesRead += d->nNumberOfSamples;
			c_nSumOfSamples -= d->nNumberOfSamples;
			pBuffer += d->nNumberOfSamples * d->nBlockAllign;
			data->nEndPosition = d->nEndPosition;

			nSamplesNeed -= d->nNumberOfSamples;

			AUDIO_QUEUE_ELEM *elem = c_first->next;
			free(c_first->pAllocatedBuffer);
			free(c_first);
			c_first = elem;
			c_nNumberOfElements--;
		}

		if(nSamplesNeed == 0 || c_nSumOfSamples == 0)
			return nSamplesRead;
	}

}


int WAudioProcessor::PushData(char *pchData, unsigned int nDataSize, unsigned int nUserData)
{
	return 0;
}