/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  WQueue class
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <malloc.h>

#include "wqueue.h"




WQueue::WQueue()
{
	c_first = 0;
	c_last = 0;
	c_num = 0;
	c_sum_size = 0;
	c_fUnBuffered = 0;
}


WQueue::~WQueue()
{
	Clear();
}


int WQueue::SetMemoryMode(unsigned int fBuffered)
{
	Clear();

	c_fUnBuffered = 0;

	if(fBuffered == 0)
		c_fUnBuffered = 1;

	return 1;
}




unsigned int WQueue::PushFirst(void *pSource, unsigned int nSize)
{
	// check input parameters

	if(pSource == 0 || nSize == 0)
		return 0;

	// allocate memory for element
	QUEUE_ELEMENT *elem = (QUEUE_ELEMENT*) malloc(sizeof(QUEUE_ELEMENT));
	if(elem == 0)	// memory allocation error 
		return 0;


	if(c_fUnBuffered)
	{
		elem->alloc_buf = 0;
		elem->data = pSource;
	}
	else
	{
		// allocate memory for data
		void *data = malloc(nSize);
		if(data == 0)	// memory allocation error
		{
			free(elem);
			return 0;
		}

		memcpy(data, pSource, nSize);

		elem->alloc_buf = data;
		elem->data = data;
	}

	elem->data_id = pSource;
	elem->size = nSize;
	c_sum_size += nSize;
	c_num++;

	if(c_first == 0) // add first element in queue
	{
		elem->prev = 0;
		elem->next = 0;
		c_first = elem;
		c_last = elem;
		return nSize;
	}

	elem->prev = 0;
	elem->next = c_first;
	c_first->prev = elem;
	c_first = elem;

	return nSize;
}


unsigned int WQueue::PushLast(void *pSource, unsigned int nSize)
{
	// check input parameters
	if(pSource == 0 || nSize == 0)
		return 0;

	// allocate memory for element
	QUEUE_ELEMENT *elem = (QUEUE_ELEMENT*) malloc(sizeof(QUEUE_ELEMENT));
	if(elem == 0)	// memory allocation error 
		return 0;

	if(c_fUnBuffered)
	{
		elem->alloc_buf = 0;
		elem->data = pSource;
		
	}
	else
	{

		// allocate memory for data
		void *data = malloc(nSize);
		if(data == 0)	// memory allocation error
		{
			free(elem);
			return 0;
		}

		memcpy(data, pSource, nSize);

		elem->alloc_buf = data;
		elem->data = data;
		
	}

	elem->data_id = pSource;
	elem->size = nSize;
	c_sum_size += nSize;
	c_num++;


	if(c_last == 0) // add first element in queue
	{
		elem->prev = 0;
		elem->next = 0;
		c_first = elem;
		c_last = elem;
		return nSize;
	}

	elem->prev = c_last;
	elem->next = 0;
	c_last->next = elem;
	c_last = elem;

	return nSize;

}


unsigned int WQueue::GetCount()
{
	return c_num;
}


unsigned int WQueue::PullFirst(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0)
		return c_first->size;

	

	// get smaller value of size
	unsigned int size = nSize < c_first->size ? nSize : c_first->size;

	if(size)
		memcpy(pDest, c_first->data, size);
	
	c_num--;
	c_sum_size -= c_first->size;


	QUEUE_ELEMENT *next = c_first->next;
	
	if(next != 0)
	{
		next->prev = 0;
		if(c_first->alloc_buf)
			free(c_first->alloc_buf);

		free(c_first);
		c_first = next;
	}
	else
	{
		if(c_first->alloc_buf)
			free(c_first->alloc_buf);

		free(c_first);
		c_first = 0;
		c_last = 0;
	}


	return size;
}



unsigned int WQueue::PullLast(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0)
		return c_last->size;


	// get smaller value of size
	unsigned int size = nSize < c_last->size ? nSize : c_last->size;

	if(size)
		memcpy(pDest, c_last->data, size);

	c_num--;
	c_sum_size -= c_last->size;


	QUEUE_ELEMENT *prev = c_last->prev;
	if(prev)
	{
		prev->next = 0;
		if(c_last->alloc_buf)
			free(c_last->alloc_buf);

		free(c_last);
		c_last = prev;
	}
	else
	{
		if(c_last->alloc_buf)
			free(c_last->alloc_buf);

		free(c_last);
		c_first = 0;
		c_last = 0;
	}

	return size;

}


unsigned int WQueue::QueryFirst(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0 || nSize == 0)
		return c_first->size;

	// get smaller value of size
	unsigned int size = nSize < c_first->size ? nSize : c_first->size;
	memcpy(pDest, c_first->data, size); 

	return size;
}



unsigned int WQueue::QueryLast(void *pDest, unsigned int nSize)
{
	if(c_num == 0)
		return 0;

	if(pDest == 0 || nSize == 0)
		return c_last->size;

	// get smaller value of size
	unsigned int size = nSize < c_last->size ? nSize : c_last->size;
	memcpy(pDest, c_last->data, size); 

	return size;

}



unsigned int WQueue::Clear()
{
	if(c_num) // free memory
	{
		QUEUE_ELEMENT *elem = c_first;
		QUEUE_ELEMENT *next = c_first;
		while(next)
		{
			if(next->alloc_buf)
				free(next->alloc_buf);

			next = next->next;
			free(elem);
			elem = next;	
		}
	}

	c_first = 0;
	c_last = 0;
	c_num = 0;
	c_sum_size = 0;
	return c_num;
}



int WQueue::FindFromFirst(void *data)
{
	if(c_num)
	{
		QUEUE_ELEMENT *next = c_first;
		while(next)
		{
			if(next->data_id == data)
				return 1;
			next = next->next;	
		}
	}

	return 0;
}


int WQueue::FindFromLast(void *data)
{
	if(c_num)
	{
		QUEUE_ELEMENT *prev = c_last;
		while(prev)
		{
			if(prev->data_id == data)
				return 1;
			prev = prev->prev;	
		}
	}

	return 0;
}






unsigned int WQueue::GetSizeSum()
{
	return c_sum_size;
}



unsigned int WQueue::PullDataFifo(void *pOutputBuffer, unsigned int nBytesToRead, int *pnBufferIsOut)
{

	unsigned int nOutBufferNum = 0;
	if(c_num == 0 || c_sum_size == 0 || nBytesToRead == 0)
		return 0;
	

	unsigned int nBytesRead = 0;
	unsigned int nByteNeed = nBytesToRead;
	char *pBuffer = (char*) pOutputBuffer;

	while(1)
	{
		if(c_first->size >= nByteNeed) // we have enough data
		{
			memcpy(pBuffer, c_first->data, nByteNeed);
			nBytesRead += nByteNeed;
			
			c_first->size -= nByteNeed;
			c_sum_size -= nByteNeed;

			// check if buffer is empty
			if(c_first->size == 0)	// remove this element from queue
			{
				nOutBufferNum++;
				PullFirst((void*) 1, 0);
			}
			else	// element isn't empty, but we have removed some data from element
			{
				c_first->data = (char*)c_first->data +  nByteNeed;	
		
			}

			nByteNeed = 0;
		
		}
		else
		{
			memcpy(pBuffer, c_first->data, c_first->size);
			nBytesRead += c_first->size;
			pBuffer += c_first->size;
			nByteNeed -= c_first->size;
			c_sum_size -= c_first->size;
			c_first->size = 0;
			nOutBufferNum++;

			PullFirst((void*) 1, 0);	

		}


		if(nByteNeed == 0 || c_num == 0)
		{
			if(pnBufferIsOut)
				*pnBufferIsOut = nOutBufferNum;

			return nBytesRead;
		}

	}

}






unsigned int WQueue::QueryFirstPointer(void **ppDest, unsigned int *pnSize)
{
	if(c_num == 0)
		return 0;

	if(ppDest == 0 || pnSize == 0)
		return 0;

	if(c_fUnBuffered)
		*ppDest = c_first->data_id;
	else
		*ppDest = c_first->alloc_buf;

	*pnSize = c_first->size;

	return 1;
}


unsigned int WQueue::QueryLastPointer(void **ppDest, unsigned int *pnSize)
{
	if(c_num == 0)
		return 0;

	if(ppDest == 0 || pnSize == 0)
		return 0;

	if(c_fUnBuffered)
		*ppDest = c_last->data_id;
	else
		*ppDest = c_last->alloc_buf;

	*pnSize = c_last->size;

	return 1;
}


int WQueue::QueryData(unsigned int nOffset, void *pOutputBuffer, unsigned int nBytesToRead, unsigned int *nBytesRead)
{

	// check input parameters
	if(pOutputBuffer == 0 || nBytesToRead == 0)
		return 0;
	// check if queue is empty
	if(c_num == 0 || c_sum_size == 0)
		return 0;
	// check offset
	if(nOffset > c_sum_size)
		return 0;

	QUEUE_ELEMENT *elem = c_first;
	unsigned int nNeedOffset = nOffset;
	unsigned int nNeedData = nBytesToRead;
	char *output = (char*) pOutputBuffer;
	char *data;
	unsigned int nHaveData;
	while(elem)
	{
		if(nNeedOffset >= elem->size) // we don't have enough data in this element
		{
			nNeedOffset -= elem->size;
			elem = elem->next;	// go to next element
			continue;
		}

			
		// start is in this element, get data
		data = (char*) elem->data + nNeedOffset;
		nHaveData = elem->size - nNeedOffset;

		if(nNeedData <= nHaveData) // we have all data we need
		{
			memcpy(output, data, nNeedData); 
			*nBytesRead = nBytesToRead;
			return 1;
		}

		// we don't have all data we need in this element, copy what we have and go to next element
		memcpy(output, data, nHaveData);
		nNeedData -= nHaveData;
		output += nHaveData;
		elem = elem->next;
		break;
	}
	
	// get rest of data
	while(elem)
	{
		if(nNeedData <= elem->size) // we have all data we need
		{
			memcpy(output, elem->data, nNeedData); 
			*nBytesRead = nBytesToRead;
			return 1;
		}
		
		// we don't have all data we need in this element, copy what we have and go to next element
		memcpy(output, elem->data, elem->size);
		nNeedData -= elem->size;
		output += elem->size;
		elem = elem->next;
			
	}

	// no more data in queue

	*nBytesRead = nBytesToRead - nNeedData;
	return 1;

}



int WQueue::CutDataFifo(unsigned int nBytesToCut)
{

	if(c_num == 0 || c_sum_size == 0)
		return 0;

	if(nBytesToCut == 0)
		return 1;
	
	unsigned int nByteNeed = nBytesToCut;

	while(c_sum_size)
	{
		if(c_first->size >= nByteNeed) // we have enough data
		{
			// check if buffer is empty
			if(c_first->size == nByteNeed)	// remove this element from queue
				PullFirst((void*) 1, 0);
			else	// element isn't empty, but we have removed some data from element
			{
				c_sum_size -= nByteNeed;
				c_first->size -= nByteNeed;
				c_first->data = (char*)c_first->data +  nByteNeed;
			}	
		
			return 1;
		
		}
		else
		{
			nByteNeed -= c_first->size;
			PullFirst((void*) 1, 0);	
		}
	}

	return 0;

}



DelayLine::DelayLine()
{
	// main buffer is 0
	c_pchMainBuffer = 0;
	c_nMainBufferSize = 0;
	// window is 0
	c_pchWindowStart = 0;		
	c_pchWindowEnd = 0;
	c_nWindowSize = 0;
	c_nShift = 0;

	c_nChunkSize = 0;
}


DelayLine::~DelayLine()
{
	Free();
}

int DelayLine::Allocate(unsigned int nMaxDelaySize, unsigned int nChunkSize)
{
	if(nMaxDelaySize == 0 || nChunkSize == 0)
		return 0;

	// calculate all size, max delay * 2 + chunk size
	unsigned int nAllSize = nMaxDelaySize * 2 + nChunkSize;
	// allocate memory for double delay data + input data size
	char *main = (char*) malloc(nAllSize);
	if(main == 0)
		return 0;

	Free();

	// assign allocated space to main buffer
	c_pchMainBuffer = main;
	// set main buffer size
	c_nMainBufferSize = nAllSize;
	// pointer to new data
	c_pchNewData = c_pchMainBuffer + nMaxDelaySize * 2;
	// initialize window
	c_pchWindowStart = c_pchMainBuffer;
	c_nWindowSize = nMaxDelaySize;
	c_pchWindowEnd = c_pchWindowStart + c_nWindowSize;
	// set window data to 0
	memset(c_pchWindowStart, 0, c_nWindowSize);
	// set chunk size
	c_nChunkSize = nChunkSize;
	// load is 0
	c_nLoad = 0;
	// no shifting
	c_nShift = 0;


	return 1;
}


int DelayLine::ReAllocate(unsigned int nMaxDelaySize, unsigned int nChunkSize)
{
	// check input parameters
	if(nMaxDelaySize == 0 || nChunkSize == 0)
		return 0;
	// check if we need to reallocate
	if(nMaxDelaySize == c_nWindowSize && nChunkSize == c_nChunkSize)
		return 1;


	// calculate all size, max delay * 2 + chunk size
	unsigned int nAllSize = nMaxDelaySize * 2 + nChunkSize;
	// allocate memory for double delay data + input data size
	char *main = (char*) malloc(nAllSize);
	if(main == 0)
		return 0;

		// shift data left, e.g. move window to right
	if(c_nShift)
	{
		c_pchWindowStart += c_nShift;
		c_pchWindowEnd += c_nShift;
	}

	c_nShift = 0;

	// copy memory from window to reallocated space

	memset(main, 0, nMaxDelaySize);
	 
	if(c_nWindowSize)
		memcpy(main, c_pchWindowStart, c_nWindowSize);
	else
		memset(main, 0, nMaxDelaySize);

	// free old memory
	Free();

	// assign allocated space to main buffer
	c_pchMainBuffer = main;
	// set main buffer size
	c_nMainBufferSize = nAllSize;
	// pointer to new data
	c_pchNewData = c_pchMainBuffer + nMaxDelaySize * 2;
	// initialize window
	c_pchWindowStart = c_pchMainBuffer;
	c_nWindowSize = nMaxDelaySize;
	c_pchWindowEnd = c_pchWindowStart + c_nWindowSize;
	// set chunk size
	c_nChunkSize = nChunkSize;
	// no shifting 
	c_nShift = 0;

	return 1;
}


int DelayLine::Free()
{
	// free main buffer memory
	if(c_pchMainBuffer)
		free(c_pchMainBuffer);
	
	c_pchMainBuffer = 0;
	c_nMainBufferSize = 0;
	c_pchNewData = 0;
	c_nLoad = 0;
	// set window to 0
	c_pchWindowStart = 0;		
	c_pchWindowEnd = 0;
	c_nWindowSize = 0;

	c_nChunkSize = 0;
	c_nShift = 0;

	return 1;
}


void DelayLine::Clear()
{
	// initialize window
	c_pchWindowStart = c_pchMainBuffer;
	c_pchWindowEnd = c_pchWindowStart + c_nWindowSize;
	// set window data to 0
	memset(c_pchWindowStart, 0, c_nWindowSize);
	c_nLoad = 0;
	c_nShift = 0;
}


int DelayLine::AddData(char *pchData, unsigned int nDataSize)
{
	// check if we have allocated space
	if(c_pchMainBuffer == 0)
		return 0;

	// check input parameters
	if(pchData == 0 || nDataSize == 0)
		return 0;



	// check if we need to reallocate memory
	if(nDataSize > c_nChunkSize)
	{
		if(ReAllocate(c_nWindowSize, nDataSize) == 0)
			return 0;
	}

	// shift data left, e.g. move window to right
	if(c_nShift)
	{
		c_pchWindowStart += c_nShift;
		c_pchWindowEnd += c_nShift;
	}

	// check if we need to move window to beginning of main buffer
	if(c_pchWindowEnd > c_pchNewData)
	{
		// shift window to beginning of main data buffer
		memcpy(c_pchMainBuffer, c_pchWindowStart, c_nWindowSize);
		c_pchWindowStart = c_pchMainBuffer;
		c_pchWindowEnd = c_pchWindowStart + c_nWindowSize;
	}

	// add new data into delay line
	memcpy(c_pchWindowEnd, pchData, nDataSize);

	c_nLoad += nDataSize;
	if(c_nLoad > c_nWindowSize)
		c_nLoad = c_nWindowSize;

	c_nShift = nDataSize;

	return 1;

}

char *DelayLine::GetData(unsigned int nDelay)
{
	// check input parameters
	if(c_pchMainBuffer == 0 || nDelay > c_nWindowSize)
		return 0;	

	// calculate delay position
	return ( c_pchWindowStart + c_nWindowSize - nDelay );

}

