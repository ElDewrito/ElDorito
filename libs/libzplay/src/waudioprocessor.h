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

#ifndef _W_AUDIOPROCESSOR_H_
#define _W_AUDIOPROCESSOR_H_




#define AUDIO_PROCESSOR_RETURN_ERROR_STR

// output function





typedef struct {
	void * pSamples;
	unsigned int nNumberOfSamples;
	unsigned int nStartPosition;
	unsigned int nEndPosition;
	unsigned int nBlockAllign;
	unsigned int fReverse;
	unsigned int nSongIndex;
} PROCESSOR_AUDIO_DATA;



typedef struct audio_elem {
	void *pAllocatedBuffer;
	void *pDataBuffer;
	struct audio_elem *next; 
} AUDIO_QUEUE_ELEM;


typedef int  (*t_audio_processor_output)(PROCESSOR_AUDIO_DATA *data);

class WAudioQueue {
	public:
		WAudioQueue();
		~WAudioQueue();

		void Clear();
		unsigned int PushSamples(PROCESSOR_AUDIO_DATA *data);
		unsigned int PullSamples(PROCESSOR_AUDIO_DATA *data);

	

	private:
		AUDIO_QUEUE_ELEM *c_first;
		AUDIO_QUEUE_ELEM *c_last;
		unsigned int c_nNumberOfElements;
		unsigned int c_nSumOfSamples;
		 

		
	protected:




};

class WAudioProcessor {
public:
	// constructor
	WAudioProcessor();
	// destructor
	~WAudioProcessor();


	virtual int PushSamples(PROCESSOR_AUDIO_DATA *data);

		virtual int PushData(char *pchData, unsigned int nDataSize, unsigned int nUserData);



// =====================================================================
	// SET OUTPUT PROCESSOR 

	int SetOutputProcessor(WAudioProcessor *instance);

	//	PARAMETERS:
	//		instance
	//			Instance of output processor.
	//			This parameter can be 0 if you don't need output processor.
	//			In this case processed data will be filled into internal output buffer and
	//			you can use PullData function and GetDataSize to get data from internal buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
	//
	//	REMARKS:
	//		By default, each processor will use internal output buffer to store processed
	//		data. But you can bypass internal buffer and spcify output processor. In this case
	//		processed data will be send to output processor as input data and this output processor
	//		will continue to process this data farther.
	//
	//		This function disables SetOutputFunction.

// =====================================================================
	// SET OUTPUT FUNCTION 

	int SetOutputFunction(t_audio_processor_output output_func);

	//	PARAMETERS:
	//		output_func
	//			Pointer to output function.
	//			typedef int (*t_audio_processor_output)(char *pchData, unsigned int nDataSize);
	//				pchData		- pointer to output data
	//				nDataSize	- size of output data in bytes
	//
	//			This parameter can be 0 if you don't need output function.
	//			In this case processed data will be filled into internal output buffer and
	//			you can use PullData function and GetDataSize to get data from internal buffer.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
	//
	//	REMARKS:
	//		By default, each processor will use internal output buffer to store processed
	//		data. But you can bypass internal buffer and spcify output function. In this case
	//		processed data will be send to output function.
	//
	//		This function disables SetOutputProcessor function.

// =============================================================================================
// check if processor is enabled

	int IsEnabled() { return c_fEnable; };


// =====================================================================
//	VITRUAL FUNCTIONS - DERIVED CLASS MUST IMPLEMENT THIS 
// =====================================================================


// ========================================================================================
// CONFIGURE BASIC PARAMETERS

	virtual int Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample);

	//	PARAMETERS:
	//		fBroadcast
	//			Set this to 1 if you need to broadcast this data across whole chain.
	//			Set this to 0 if you need to apply this data only on this processor.
	//		
	//		nSampleRate
	//			Sample rate.
	//
	//		nChannel
	//			Number of channels.
	//
	//		nBitPerSample
	//			Bit per sample.
	//
	//	RETURN VALUES:
	//		1 - all OK
	//		0 - error, call GetError() to get error string
	//
	//	REMARKS:
	//		Call this function to set basic parameters for PCM stream.
	//		This function will propagate through whole chain.

// ==========================================================================================================
	// enable or disable processor
	//
	
	virtual int Enable(int fBroadcast, int fEnable);	
	
	//	PARAMETERS:
	//		fBroadcast
	//			Set this to 1 if you need to broadcast this data across whole chain.
	//			Set this to 0 if you need to apply this data only on this processor.
	//
	//		fEnable
	//			Specifies whether to enable or disable processor.
	//			Value 1 enables equalizer, 0 disables procesor.
	//
	//	RETURN VALUES:
	//		1 - all OK
	//		0 - error, call GetError() to get error string



// =====================================================================
//	clear internal and output buffers

	virtual int Clear(int fBroadcast);

	//	PARAMETERS:
	//		fBroadcast
	//			Set this to 1 if you need to broadcast this data across whole chain.
	//			Set this to 0 if you need to apply this data only on this processor.
	//
	//	RETURN VALUES:
	//		1	- all ok
	//		0	- error

// =====================================================================
//	flush internal buffers to output buffer, all data are available in output buffer

	virtual int Flush(int fBroadcast);
	//	PARAMETERS:
	//		fBroadcast
	//			Set this to 1 if you need to broadcast this data across whole chain.
	//			Set this to 0 if you need to apply this data only on this processor.
	//
	//	RETURN VALUES:
	//		1	- all ok
	//		0	- error	


// =====================================================================
	// get error string
	char *GetError();

	//	PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		Pointer to error message string.

// =====================================================================



	#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
		// return error message
		char *c_pchReturnError;
	#endif
	
private:	

protected:
	// error message
	char c_pchErrorMessageStr[128];
	// pointer to next processor in chain
	WAudioProcessor *c_next;
	// output function
	t_audio_processor_output c_output;

	// samplerate
	unsigned int c_nSampleRate;
	// number of channels
	unsigned int c_nChannel;
	// bits per sample
	unsigned int c_nBitPerSample;
	// block align
	unsigned int c_nBlockAlign;

	// indicates enabled or disabled processor
	int c_fEnable;


	// ============================================

	WAudioQueue c_queue;

			
};





#endif
