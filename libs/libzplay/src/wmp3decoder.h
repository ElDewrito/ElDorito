/*
 *  libzplay - windows ( WIN32 ) multimedia library
 *  MP3 decoder
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
 * SUPPORTED BY:
 * ============================================================================
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc. <support@underbit.com>
 * http://www.underbit.com/products/mad/
 * GNU General Public License ( GPL.TXT )
 * ============================================================================
*/


#ifndef _W_MP3DECODER_H_
#define _W_MP3DECODER_H_



#include "wdecoder.h"
#include "../decoders/libmad/mad.h"
#include "wtags.h"



struct rgain {
  enum rgain_name name;			/* profile (see above) */
  enum rgain_originator originator;	/* source (see above) */
  signed short adjustment;		/* in units of 0.1 dB */
};

struct tag_xing {
	long flags;		   /* valid fields (see above) */
	unsigned long frames;	   /* total number of frames */
	unsigned long bytes;	   /* total number of bytes */
	unsigned char toc[100];  /* 100-point seek table */
	long scale;		   /* VBR quality indicator (0 best - 100 worst) */
};



struct tag_lame {
	unsigned char revision;
	unsigned char flags;

	enum tag_lame_vbr vbr_method;
	unsigned short lowpass_filter;

	mad_fixed_t peak;
	struct rgain replay_gain[2];

	unsigned char ath_type;
	unsigned char bitrate;

	int start_delay;
	int end_padding;

	enum tag_lame_source source_samplerate;
	enum tag_lame_mode stereo_mode;
	unsigned char noise_shaping;

	signed char gain;
	enum tag_lame_surround surround;
	enum tag_lame_preset preset;

	unsigned long music_length;
	unsigned short music_crc;
};


struct tag_xl {
	int flags;
	struct tag_xing xing;
	struct tag_lame lame;
	char encoder[21];
};



typedef struct {
  mad_fixed_t error[3];
  mad_fixed_t random;
} AUTODITHER_STRUCT;



class WMp3Decoder : public WAudioDecoder {
public:

	WMp3Decoder();
	~WMp3Decoder();

	int OpenStream(WQueue *pQueue, int fDynamic, int param1, int param2);


	int  Initialize(int param1, int param2, int param3, int param4);
	int Uninitialize();


	INPUT_STREAM_INFO *GetStreamInfo();

	wchar_t **GetID3Info(int version, char *pStream, unsigned int nStreamSize, int param1, int param2);
	int Close();
	void  Release();
	int GetData(DECODER_DATA *pDecoderData);
	// seek current position
	int Seek(unsigned int nSamples);

	int GetBitrate(int fAverage);

	DECODER_ERROR_MESSAGE *GetError();

	int SetReverseMode(int fReverse);

private:
	// Blank samples at file beginning (XING/LAME/VBRI frame)
	unsigned int c_nBlankSamplesAtBeginning;

	// Encoder delay in samples (read from LAME/VBRI frame)
	unsigned int c_nEncoderDelayInSamples;

	// Encoder padding at end of file (read from LAME/VBRI frame)
	unsigned int c_nEncoderEndPaddingInSamples;



	// pointer to first byte of input  data stream
	unsigned char *c_pchStreamStart;
	// pointer to last byte of input data stream
	unsigned char *c_pchStreamEnd;
	// size of input data stream
	unsigned int c_nStreamSize;

	// pointer to first byte of input data
	unsigned char *c_pchStart;
	// pointer to last byte of input data
	unsigned char *c_pchEnd;
	// input data size in bytes
	unsigned int c_nSize;
	// current position within data stream, decoder
	unsigned char *c_pchPosition;


	// indicates that stream is ready
	unsigned int c_fReady;

	// info about input stream
	INPUT_STREAM_INFO c_isInfo;


	// bit per sample, need this to convert 8 bit to 16 bit
	unsigned int c_nBitPerSample;
	// specify reverse mode
	unsigned int c_fReverse;


	// managed stream
	unsigned int c_fManagedStream;
	// end of stream indicator
	unsigned int c_fEndOfStream;

	// working buffer
	int *c_pchBuffer;
	// allocated size of buffer in stereo samples
	unsigned int c_pchBufferAllocSize;
	int *c_pchBufferPos;
	unsigned int c_nBufferSize;


	int _OpenFile(unsigned int fSkipExtraChecking);

	// clear all buffers and initialize all to 0
	void _ClearForSeek();

	WQueue *c_Queue;

	DECODER_DATA c_DecoderData;

	// fill buffer with data
	static int _FillBuffer(WMp3Decoder *instance);



// ===========================================================================

	struct mad_stream c_stream; // main mad stream structure ( initialized and free in thread ) 
	struct mad_frame c_frame;	// main mad frame structure ( initialized and free in thread ) 
	struct mad_synth c_synth;	// main mad synth structure ( initialized and free in thread ) 


	// dithering
	AUTODITHER_STRUCT c_left_dither;
	AUTODITHER_STRUCT c_right_dither;


	int c_fPreciseSongLength;
	int c_fPreciseSeek;


	// indicator that we have valid XING frame
	int c_fValidXing;
	// XING frame size in bytes
	unsigned int c_nXingFrameSize;


	// array of pointers to mp3 frames, need this for accurate seek function
	unsigned char **c_pFramePtr;
	// index of last available frame in c_pFramePtr array
	unsigned int c_nLastAvailableFrameIndex;
	// allocated size of c_pFramePtr array, nzmber of allocated elements
	unsigned int c_nAllocatedFrameNumber;
	// current frame index, used only with accurate seek with dynamic scann
	unsigned int c_nCurrentFrameIndex;

	// current seek index, need this for reverse mode
	unsigned int c_nCurrentSeekIndex;

	unsigned int c_nCurrentPosition;	// current position

	// current frame bitrate
	unsigned int c_nCurrentBitrate;

	unsigned int c_nSkipSamlesNum;	// number of samples to skip within frame to get accurate seek position
	unsigned int c_nFrameOverlay;	// frame overlay, number of frames decoded, but skipped to output after seek function

	// get frame index for accurate seek
	unsigned int getFrameIndex(unsigned int nSamples);

	// use this buffer to fix MAD_BUFFER_GUARD problem
	unsigned char *c_pchMadBuffGuard;

	// buffer for data from managed streams
	unsigned char *c_pchManagedBuffer;
	// size of managed buffer in bytes
	unsigned int c_nManagedBufferMaxSize;
	// current load
	unsigned int c_nManagedBufferLoad;

	unsigned int c_nFreeBitrate; // freebitrate
	struct tag_xl c_xing;	// xing structure

	char c_pchDescription[64]; // stream description


	// scan whole stream and count number of mp3 frames
	static unsigned int count_mp3_frames(unsigned char *buff, unsigned int size, unsigned int *vbr, unsigned int *bitrate);
	//
	//	PARAMETERS:
	//		buff
	//			Pointer to stream
	//
	//		size
	//			Size of stream in bytes
	//
	//		vbr
	//			Pointer to variable receiving indicator if this is VBR stream
	//
	//		bitrate
	//			Pointer to variable receiving average stream bitrate.
	//
	//	RETURN VALUE:
	//		Number of mp3 frames in stream.	

// ===============================================================================
	// scan whole stream, count number of mp3 frames and get frame pointers
	static unsigned int get_frame_pointers(unsigned char *buff, unsigned int size, unsigned int *vbr, unsigned int *bitrate,
						unsigned char*** frame_ptr, unsigned int initial_num);
	//
	//	PARAMETERS:
	//		buff
	//			Pointer to stream
	//
	//		size
	//			Size of stream in bytes
	//
	//		vbr
	//			Pointer to variable receiving indicator if this is VBR stream
	//
	//		bitrate
	//			Pointer to variable receiving average stream bitrate.
	//
	//		frame_ptr
	//			Pointer to address of  array.
	//
	//		initial_num
	//			Initial number of elements in  array. Use this value to preallocate memory
	//			if you know number of frames from XING header.
	//
	//	RETURN VALUE:
	//		Number of mp3 frames in stream.	

// ============================================

	void err(unsigned int error_code);
	DECODER_ERROR_MESSAGE c_err_msg;


	ID3Tag c_tag;


};




#endif
