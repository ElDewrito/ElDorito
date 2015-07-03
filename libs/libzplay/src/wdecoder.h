/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *  Audio decoder
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
 * ver: 2.00
 * date: 24. April, 2010.
 *
*/


#ifndef _W_DECODER_H_
#define _W_DECODER_H_


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "debug.h"
#include "wqueue.h"

#define WAVE_IN_NUM 15

typedef struct {
	char string_id[20];
	unsigned int id;
	char string_name1[20];
	char string_name2[20];
	char string_name3[20];
} WAVEIN_ID;

typedef struct {
	wchar_t *errorW;
} DECODER_ERROR_MESSAGE;

typedef struct {
	unsigned int nSampleRate;		// PCM samplerate
	unsigned int nChannel;			// number of PCM channels ( decoder must output always 2 channel data )
	unsigned int nBitPerSample;		// PCM bit per sample ( decoder must output always 16 bit per sample data )
	unsigned int nBlockAlign;		// Block alignment, in bytes. ( decoder must output always 4 byte block align )
	unsigned int nFileBitrate;		// file bitrate in bits per second
	unsigned int fVbr;				// variable or constant bitrate
	unsigned int nLength;			// length of stream in PCM samples
	char *pchStreamDescription;		// stream description
} INPUT_STREAM_INFO;


#define ID3_FIELD_NUMBER 7

#define ID3_FIELD_NUMBER_EX 20

#define ID3_INFO_TITLE 0
#define ID3_INFO_ARTIST 1
#define ID3_INFO_ALBUM 2
#define ID3_INFO_GENRE 3
#define ID3_INFO_COMMENT 4
#define ID3_INFO_YEAR 5
#define ID3_INFO_TRACK 6

#define ID3_INFO_COMPOSER 7
#define ID3_INFO_ORIGINAL_ARTIST 8
#define ID3_INFO_COPYRIGHT 9
#define ID3_INFO_URL 10
#define ID3_INFO_ENCODER 11
#define ID3_INFO_PUBLISHER 12
#define ID3_INFO_BPM 13
#define ID3_INFO_ALBUM_ARTIST 14
#define ID3_INFO_PICTURE_MIME 15
#define ID3_INFO_PICTURE_TYPE 16
#define ID3_INFO_PICTURE_DESCRIPTION 17
#define ID3_INFO_PICTURE_DATA 18
#define ID3_INFO_PICTURE_DATA_SIZE 19



// decoder error codes

#define DECODER_OK 0
#define DECODER_END_OF_DATA 1
#define DECODER_FATAL_ERROR 2
#define DECODER_MANAGED_NEED_MORE_DATA 3


typedef struct {
	unsigned int msgFromDecoder;
	unsigned int nBufferInQueue;
	unsigned int nBytesInQueue;
	unsigned int nBufferDone;

	short *pSamples;			// pointer to buffer for samples, user need to allocate this buffer
	unsigned int nSamplesNum;	// number of samples user need from buffer
								// on return this value contains number of samples retreived from buffer

	unsigned int nStartPosition;
	unsigned int nEndPosition;
	unsigned int nSeekPosition;
	unsigned int fReverse;
	unsigned int nSongIndex;

	unsigned int fEndOfStream;

} DECODER_DATA;




typedef struct {
	unsigned int nDeviceID;	// Identifier of the waveform-audio input device to open.
	unsigned int nMixerInputLineIndex;	// index of mixer input line from g_wavein_id table
	char *sMixerInputLineName;	// mixer line name
	unsigned int nVolume;	// volume of input line
	unsigned int nWaveBufferSize;	// size of wave buffer(max latency) in milliseconds
} WAVEIN_INIT_DATA;


// ===================================================================================================
// convert ANSI to UTF-16
int ANSIToUTF16(char *src, int nCharacterNumber, wchar_t *dest, int nDestSizeInBytes); 

// PARAMETERS:
//		src
//			Pointer to ANSI string.
//
//		nCharacterNumber
//			Number of ANSI characters ( bytes ) in src buffer.
//
//		dest
//			Pointer to buffer for destination UTF-16 string.
//			This string will be always null terminated.
//
//		nDestSizeInBytes
//			Size of destination buffer in UTF-16 characters.
//			Null terminator is also included in size.
//
//	RETURN VALUES:
//		Number of UNICODE characters written in dest buffer ( null terminator is included ).
//		If nDestSizeInBytes is 0, function returns number of UTF-16 characters needed for dest buffer.
//
//	REMARKS:
//		Returned string is always null terminated.


// ===================================================================================================
// convert UTF-8 to UTF-16
int UTF8ToUNICODE(char *src, int nBytes, wchar_t *dest, int nDestSizeInBytes); 

// PARAMETERS:
//		src
//			Pointer to UTF-8 string.
//
//		nBytes
//			Number of  bytes in src buffer.
//
//		dest
//			Pointer to buffer for destination UTF-16 string.
//			This string will be always null terminated.
//
//		nDestSizeInBytes
//			Size of destination buffer in UNICODE characters.
//			Null terminator is also included in size.
//
//	RETURN VALUES:
//		Number of UNICODE characters written in dest buffer ( null terminator is included ).
//		If nDestSizeInBytes is 0, function returns number of UTF-16 characters needed for dest buffer.
//
//	REMARKS:
//		Returned string is always null terminated.

// ===================================================================================================
// convert UTF-16 string to ANSI string using ANSI code page
int UTF16ToANSI(wchar_t *src, int nCharacterNumber, char *dest, int nDestSizeInBytes); 

// PARAMETERS:
//		src
//			Pointer to UTF-16 string.
//
//		nCharacterNumber
//			Number of UTF-16 characters in src buffer.
//
//		dest
//			Pointer to buffer for destination ANSI string.
//			This string will be always null terminated.
//
//		nDestSizeInBytes
//			Size of destination buffer in bytes ( ANSI characters ).
//			Null terminator is also included in size.
//
//	RETURN VALUES:
//		Number of bytes written in dest buffer ( null terminator is included ).
//		If nDestSizeInBytes is 0, function returns number of bytes needed for dest buffer.
//
//	REMARKS:
//		Returned string is always null terminated.	


// ===================================================================================================
// convert UFF-8 string to ANSI string using ANSI code page
int UTF8ToANSI(char *src, int nBytes, char *dest, int nDestSizeInBytes); 

// PARAMETERS:
//		src
//			Pointer to UTF-8 string.
//
//		nBytes
//			Number of bytes in src buffer.
//
//		dest
//			Pointer to buffer for destination ANSI string.
//			This string will be always null terminated.
//
//		nDestSizeInBytes
//			Size of destination buffer in bytes ( ANSI characters ).
//			Null terminator is also included in size.
//
//	RETURN VALUES:
//		Number of bytes written in dest buffer ( null terminator is included ).
//		If nDestSizeInBytes is 0, function returns number of bytes needed for dest buffer.
//
//	REMARKS:
//		Returned string is always null terminated.	



// ===================================================================================================

// reverse PCM 16 bit stereo
void PCM16StereoReverse(int *inSamples, unsigned int inSampleNum, int *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 16 bit per sample, stereo.
//
//		inSampleNum
//			Number of input  samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be size as inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.


// ===================================================================================================

// convert PCM 16 bit mono to PCM 16 bit stereo
void PCM16MonoToPCM16Stereo(short *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 16 bit per sample, mono.
//
//		inSampleNum
//			Number of input samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be double size as inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.



// ===================================================================================================

// convert PCM 16 bit stereo unsigned to PCM 16 bit stereo
void PCM8StereoUnsignedToPCM16Stereo(unsigned char *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 8 bit per sample, stereo.
//
//		inSampleNum
//			Number of input samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be double size as inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.


// ===================================================================================================

// convert PCM 16 bit stereo signed to PCM 16 bit stereo
void PCM8StereoSignedToPCM16Stereo(char *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 8 bit per sample, stereo.
//
//		inSampleNum
//			Number of input samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be double size as inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.

// ===================================================================================================

// convert PCM 8 bit mono unsigned to PCM 16 bit stereo
void PCM8MonoUnsignedToPCM16Stereo(unsigned char *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 8 bit per sample, mono.
//
//		inSampleNum
//			Number of input  samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be 4 * size as inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.


// ===================================================================================================

// convert PCM 8 bit mono signed to PCM 16 bit stereo
void PCM8MonoSignedToPCM16Stereo(char *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 8 bit per sample, mono.
//
//		inSampleNum
//			Number of input  samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be 4 * size as inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.


// ===================================================================================================

// convert PCM 32 bit stereo to PCM 16 bit stereo
void PCM32StereoToPCM16Stereo(int *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 32 bit per sample, stereo.
//
//		inSampleNum
//			Number of input samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be 1/2 size of inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.

// ===================================================================================================

// convert PCM 32 bit mono to PCM 16 bit stereo
void PCM32MonoToPCM16Stereo(int *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 32 bit per sample, mono.
//
//		inSampleNum
//			Number of input  samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be  size of inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.

// =================================================================================
// convert PCM 16 bit stereo big-endian to PCM 16 bit stereo little-endian
void PCM16StereoBEToPCM16StereoLE(short *inSamples, unsigned int inSampleNum, short *outSamples);
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 16 bit per sample, stereo.
//
//		inSampleNum
//			Number of input samples.
//
//		outSamples
//			Buffer for output samples. This buffer must be  size of inSamples.
//			This buffer can be same buffer as inSamples.
//
//	RETURN VALUES:
//		None.
//
//	REMARKS:
//		Buffer inSamples and outSamples can be the same buffer.



// ===================================================================================================
// ===================================================================================================



class WAudioDecoder {
public:
//==============================================
	// initialize decoder, call before you use decoder
	virtual int  Initialize(int param1, int param2, int param3, int param4) = 0;

	// PARAMETERS:
	//		param1	- decoder specific value, depends on decoder type
	//		param2	- decoder specific value, depends on decoder type
	//		param3	- decoder specific value, depends on decoder type
	//		param4	- decoder specific value, depends on decoder type
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
	//
	//	REMARKS:
	//		Each decoder has it's own parameters.


//==============================================
//	uninitialize decoder, call when you don't need decoder anymore

	virtual int Uninitialize() = 0;

	// PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
// ====================================================================

// open stream
//
//	PARAMETERS:
//		pQueue		- queue with stream data
//		fDynamic	- indicate dynamic or static stream
//		fLite		- indicate lite or full opening
//		param2		- unused, reserved for future use
//
//	RETURN VALUES:
//		1	- all OK
//		0	- error
//
//	REMARKS:
//		User is responsible for pQueue. This function only reads data from queue in
//		dynamic stream.
//		With lite opening function will not chack if stream is valid stream. Uee lite
//		opening for access to ID3 data only.

	virtual int OpenStream(WQueue *pQueue, int fDynamic, int fLite, int param2) = 0;


// ======================================================================================

//	close decoder, stream
	virtual int Close() = 0;

	// PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
//==============================================
//	get stream info, samplerate, channels, ...

	virtual INPUT_STREAM_INFO *GetStreamInfo() = 0;

	// PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		Pointer to INPUT_STREAM_INFO structure.
	//		0	- error
//==============================================

// Get ID3 info array
//
//  PARAMETERS:
//		version - ID3 version
//
//	RETURN VALUES:
//		pointer to ID3 info array
//		NULL - error

//virtual wchar_t **GetID3Info(int version, char *pStream, unsigned int nStreamSize, int param1, int param2) = 0;


//==============================================

//	get data from decoder

	virtual int GetData(DECODER_DATA *pDecoderData) = 0;

	// PARAMETERS:
	//		pDecoderData
	//			Pointer to DECODER_DATA.
	//				pchData	- pointer to allocated memory, decoder will fil this memory with data
	//				nDataSize	- number of bytes we need from decoder
	//
	//	RETURN VALUES:
	//		DECODER_OK - decoder has returned some valid data
	//		DECODER_END_OF_DATA - end of data
	//		DECODER_FATAL_ERROR - fatal error
	//		DECODER_MANAGED_NEED_MORE_DATA - decoder need more data from managed stream
	//
	//	REMARKS:
	//		DECODER MUST ALWAYS RETURN 2 CHANNEL ( STEREO ) 16 BIT PCM DATA.
	//		
	//
//==============================================
//	seek stream position

	virtual int Seek(unsigned int nSamples) = 0;

	// PARAMETERS:
	//		nSamples
	//			Number of samples to seek from beginning of stream.
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error
//==============================================
//	free class instance

	virtual void  Release() = 0;


//==============================================
// get current bitrate

	virtual int GetBitrate(int fAverage) = 0;

	// PARAMETERS:
	//		fAverage
	//			Specifies if we need average bitrate or real bitrate.
	//		
	//
	//	RETURN VALUES:
	//		Bitrate in bits per second.

//==============================================
// set reverse mode
	virtual int SetReverseMode(int fReverse) = 0;

	// PARAMETERS:
	//		fReverse
	//			Srecifying reverse mode.
	//		
	//
	//	RETURN VALUES:
	//		1	- all OK
	//		0	- error.


//==============================================

	// get error messages from decoder
	virtual DECODER_ERROR_MESSAGE * GetError() = 0;

	// PARAMETERS:
	//		None.
	//
	//	RETURN VALUES:
	//		Pointer to DECODER_ERROR_MESSAGE structure.

// ======================================================

};


void RemoveEndingSpaces(char* buff);
void RemoveEndingSpacesEx(char* buff, int size);


WAudioDecoder *  CreateMp3Decoder();

#endif

