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


#ifndef __W_QUEUE_H__
#define __W_QUEUE_H__



typedef struct {
	unsigned int nStart;
	unsigned int nEnd;
	unsigned int nSamplesNum;
	unsigned int nSeekPosition;
	unsigned int fReverse;
	void *pSamples;
	unsigned int nSize;
} QUEUE_SAMPLES_BUFFER;



typedef struct w_queue_elem {
	void *alloc_buf;
	void *data;
	void *data_id;
	unsigned int size;
	struct w_queue_elem *prev; 
	struct w_queue_elem *next; 
} QUEUE_ELEMENT;




class WQueue {
public:
	WQueue();
	~WQueue();

	// set queue memory mode

	int SetMemoryMode(unsigned int fBuffered);
	
	//	PARAMETERS:
	//		fBuffered
	//			Buffered mode. Queue will allocate internal memory for input data and copy input
	//			data into this memory. This is defalult mode.
	//			If fBuffered is 0, queue will not allocate internal memory for data. Queue will just save
	//			pointer to input data. In this case user is responsible for memory management of this data.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
	//
	//	REMARKS:
	//		This function will clear all data in queue. Call this function before you push data into queue.
	//		By default, queue is set in buffered memory mode.

// ==================================================================================================

	// push data into queue at queue start

	unsigned int PushFirst(void *pSource, unsigned int nSize);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you need to push into queue.
	//			This parameter is also queue element identifier and you can use this
	//			value to check if this block of data is still in queue using functions
	//			FindFromFirst and FindFromLast.
	//			This value can't be 0. If this value is 0 function fails and returns 0.
	//
	//		nSize
	//			Specifies the size, in bytes, of the block of memory to push into queue.
	//			This value can't be 0. If this value is 0 function fails and returns 0.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied into queue.
	//		Function returns 0 if memory allocation error occurs.
	//		
	//
	//	REMARKS:
	//		Queue allocates memory to store data and copies data into this memory.
	//		After you push data into queue you can destroy data pointed by pSource.	


// ==================================================================================================

	// push data into queue at the queue end

	unsigned int PushLast(void *pSource, unsigned int nSize);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you need to push into queue.
	//			This parameter is also queue element identifier and you can use this
	//			value to check if this block of data is still in queue using functions
	//			FindFromFirst and FindFromLast.
	//			This value can't be 0. If this value is 0 function fails and returns 0.
	//
	//		nSize
	//			Specifies the size, in bytes, of the block of memory to push into queue.
	//			This value can't be 0. If this value is 0 function fails and returns 0.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied into queue.
	//		Function returns 0 if memory allocation error occurs.
	//		
	//
	//	REMARKS:
	//		Queue allocates memory to store data and copies data into this memory.
	//		After you push data into queue you can destroy data pointed by pSource.	


// ==================================================================================================

	//	copy data from first element into memory block and remove first element
	//	from queue. 

	unsigned int PullFirst(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function copies no data and element is removed
	//			from queue.
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in first
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data and rest of data is lost.
	//
	//		If you need to remove element from queue without data retrieving, call
	//		this function with pDest set to 1 and nSize set to 0.


// ==================================================================================================


	//	copy data from last element into memory block and remove last element
	//	from queue. 

	unsigned int PullLast(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in last element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function copies no data and element is removed
	//			from queue.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in last
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data and rest of data is lost because element
	//		is removed from queue.
	//
	//		If you need to remove element from queue without data retrieving, call
	//		this function with pDest set to 1 and nSize set to 0.


// ==================================================================================================


// ==================================================================================================

	//	copy data from first element into memory block, but DON'T REMOVE element from queue

	unsigned int QueryFirst(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in first
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data.


// ==================================================================================================


	//	copy data from last element into memory block, but DON'T REMOVE element from queue

	unsigned int QueryLast(void *pDest, unsigned int nSize);

	//	PARAMETERS:
	//		pDest
	//			Points to the starting address of the destination block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in first element of queue.
	//
	//		nSize
	//			Specifies the size, in bytes, of the destination memory block.
	//			If this value is 0, function returns size, in bytes, of data block
	//			stored in last element of queue.		
	//	
	//	RETURN VALUES:
	//		This function returns number of bytes copied from queue element into
	//		destination memory block.
	//		If pDest is 0, function return size, in bytes, of data stored in last
	//		element of queue. Use this value to allocate needed memory for pDest.
	//
	//		If return value is 0, queue is empty.
	//		
	//
	//	REMARKS:
	//		If nSize parameter is smaller than data stored in queue element, function
	//		copies only nSize bytes of data.


// ==================================================================================================


	// check if data buffer is still in queue, search from first element

	int FindFromFirst(void *pSource);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you pushed into queue
	//			with PushFirst od PushLast function.
	//			This parameter can't be 0.	
	//	
	//	RETURN VALUES:
	//		If buffer identified with pSource is still in queue, function returns 1.
	//		If buffer isn't in queue anymore, function returns 0
	//		
	//	REMARKS:
	//		This function search from first element of queue.

// ==================================================================================================


	// check if data buffer is still in queue, search from last element

	int FindFromLast(void *pSource);

	//	PARAMETERS:
	//		pSource
	//			Points to the starting address of the block you pushed into queue
	//			with PushFirst od PushLast function.
	//			This parameter can't be 0.	
	//	
	//	RETURN VALUES:
	//		If buffer identified with pSource is still in queue, function returns 1.
	//		If buffer isn't in queue anymore, function returns 0
	//		
	//	REMARKS:
	//		This function search from last element of queue.


// ==================================================================================================

	// clear queue, free all memory and remove all elements from queue.

	unsigned int Clear();

	//	PARAMETERS:
	//		This function has no parameters
	//
	//	RETURN VALUES:
	//		Function returns number of elements removed from queue.
	//		If queue was empty, function returns 0.	

// ==================================================================================================


	// get number of elements in queue

	unsigned int GetCount();

	//	PARAMETERS:
	//		This function has no parameters
	//
	//	RETURN VALUES:
	//		Function returns number of elements in queue.


// ==================================================================================================

	// get size of all data into queue

	unsigned int GetSizeSum();

	//	PARAMETERS:
	//		This function has no parameters
	//
	//	RETURN VALUES:
	//		Function returns size of all data in queue


// ==================================================================================================


	// copy data from queue into buffer, start from first element in queue
	// this function treats queue as one large continuous buffer

	unsigned int PullDataFifo(void *pOutputBuffer, unsigned int nBytesToRead, int *pnBufferIsOut);

	//	PARAMETERS:
	//		pOutputBuffer
	//			Points to the starting address of the destination block.
	//			This value can't be 0.
	//
	//		nBytesToRead
	//			Number of bytes to copy into pOutputBuffer. If first element doesn't contain
	//			enough data, function copies data from second buffer, third ...
	//			This function deals with queue as with one large memory buffer which contains
	//			all data you pushed into queue. If you read all data from some element, this
	//			element is removed from queue.
	//
	//		pnBytesRead
	//			Number of bytes to read.
	//
	//		pnBufferIsOut
	//			Number of buffers thrown from queue.	
	//
	//	RETURN VALUES:
	//		Function returns number of data read from queue.
	//		If function returns 0, queue is empty or error occurs.
	//		
	//
	//	REMARKS:
	//		If you need more data than you have in queue, function copies all data from queue, and
	//		then queue is empty.
	
	
	// ==================================================================================================


	// get pointer to first data buffer and get data buffer size
	unsigned int QueryFirstPointer(void **ppDest, unsigned int *pnSize);	

	//	PARAMETERS:
	//		ppDest
	//			Points to variable receiving addres of first data block
	//
	//		pnSize
	//			Points to variable receiving size of data block		
	//	
	//	RETURN VALUES:
	//
	//		If return value is 0, queue is empty.
	//		
	//

	// ==================================================================================================


	// get pointer to last data buffer and get data buffer size
	unsigned int QueryLastPointer(void **ppDest, unsigned int *pnSize);	

	//	PARAMETERS:
	//		ppDest
	//			Points to variable receiving addres of last data block
	//
	//		pnSize
	//			Points to variable receiving size of data block		
	//	
	//	RETURN VALUES:
	//
	//		If return value is 0, queue is empty.
	//		
	//


	// ==================================================================================================


	int QueryData(unsigned int nOffset, void *pOutputBuffer, unsigned int nBytesToRead, unsigned int *nBytesRead);


// ==================================================================================================
// Cut data from FIFO queuy

//	PARAMETERS:
//		nBytesToCut - number of bytes to cut from queue
//
//	RETURN VALUES:
//		1	- OK
//		0	- error

	int CutDataFifo(unsigned int nBytesToCut);




private:
	QUEUE_ELEMENT *c_first;		// pointer to first element
	QUEUE_ELEMENT *c_last;		// pointer to last element
	unsigned int c_num;			// number of elements in queue
	unsigned int c_sum_size;	// size of all data in queue
	unsigned int c_fUnBuffered;	// unbuffered memory mode




};





class DelayLine {
public:
	DelayLine();
	~DelayLine();
	int Allocate(unsigned int nMaxDelaySize, unsigned int nChunkSize);
	int ReAllocate(unsigned int nMaxDelaySize, unsigned int nChunkSize);
	int Free();
	int GetLoad() {return c_nLoad; };
	int AddData(char *pchData, unsigned int nDataSize);
	char *GetData(unsigned int nDelay);
	void Clear();

private:
	// allocated memory for main buffer
	char *c_pchMainBuffer;	
	// size of allocated main buffer
	unsigned int c_nMainBufferSize;	
	// first byte of window
	char *c_pchWindowStart;
	// window end		
	char *c_pchWindowEnd;
	// pointer to new data
	char *c_pchNewData;
	// window size
	unsigned int c_nWindowSize;
	// size of allocated chunk buffer
	unsigned int c_nChunkSize;	
	// number of data loaded in delay line	
	unsigned int c_nLoad;
	// number of bytes to shift window
	unsigned int c_nShift;			


};









#endif

