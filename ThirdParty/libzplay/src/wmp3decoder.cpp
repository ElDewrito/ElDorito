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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "debug.h"
#include "wmp3decoder.h"
#include "wtags.h"


// delay of LIBMAD decoder
# define LIBMAD_DELAY  (528 + 1) // from LAME encoder.h (= DECDELAY + 1)


#define FRAME_INDEX_ERR 0xFFFFFFFF




// minimal mp3 frame size
#define MIN_FRAME_SIZE 24
// max frame size
#define MAX_FRAME_SIZE 5761
// check next 200 mp3 frames to determine constant or variable bitrate if no XING header found 
#define SEARCH_DEEP_VBR 200
// frame overlay number, this frames will be processed but PCM data will not be send to output
// need this fo fix problems with accurate seek on MPEG2 files
#define FRAME_OVERLAY_NUM 2
 
enum {
	TAG_XING = 0x0001,
	TAG_LAME = 0x0002,
	TAG_VBRI = 0x0004,
	TAG_VBR  = 0x0100
};

enum {
	TAG_XING_FRAMES = 0x00000001L,
	TAG_XING_BYTES  = 0x00000002L,
	TAG_XING_TOC    = 0x00000004L,
	TAG_XING_SCALE  = 0x00000008L
};


enum {
	TAG_LAME_NSPSYTUNE    = 0x01,
	TAG_LAME_NSSAFEJOINT  = 0x02,
	TAG_LAME_NOGAP_NEXT   = 0x04,
	TAG_LAME_NOGAP_PREV   = 0x08,

	TAG_LAME_UNWISE       = 0x10
};

# define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
# define INFO_MAGIC	(('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')
# define LAME_MAGIC	(('L' << 24) | ('A' << 16) | ('M' << 8) | 'E')
# define VBRI_MAGIC	(('V' << 24) | ('B' << 16) | ('R' << 8) | 'I')


// skip ID3v2 tag at the beginning of file
#define SKIP_ID3TAG_AT_BEGINNING 1

// number of mp3 frames decoded to output
#define INPUT_MP3_FRAME_NUMBER 4



enum rgain_name {
  RGAIN_NAME_NOT_SET    = 0x0,
  RGAIN_NAME_RADIO      = 0x1,
  RGAIN_NAME_AUDIOPHILE = 0x2
};

enum rgain_originator {
  RGAIN_ORIGINATOR_UNSPECIFIED = 0x0,
  RGAIN_ORIGINATOR_PRESET      = 0x1,
  RGAIN_ORIGINATOR_USER        = 0x2,
  RGAIN_ORIGINATOR_AUTOMATIC   = 0x3
};



enum tag_lame_vbr {
	TAG_LAME_VBR_CONSTANT      = 1,
	TAG_LAME_VBR_ABR           = 2,
	TAG_LAME_VBR_METHOD1       = 3,
	TAG_LAME_VBR_METHOD2       = 4,
	TAG_LAME_VBR_METHOD3       = 5,
	TAG_LAME_VBR_METHOD4       = 6,
	TAG_LAME_VBR_CONSTANT2PASS = 8,
	TAG_LAME_VBR_ABR2PASS      = 9
};

enum tag_lame_source {
	TAG_LAME_SOURCE_32LOWER  = 0x00,
	TAG_LAME_SOURCE_44_1     = 0x01,
	TAG_LAME_SOURCE_48       = 0x02,
	TAG_LAME_SOURCE_HIGHER48 = 0x03
};

enum tag_lame_mode {
	TAG_LAME_MODE_MONO      = 0x00,
	TAG_LAME_MODE_STEREO    = 0x01,
	TAG_LAME_MODE_DUAL      = 0x02,
	TAG_LAME_MODE_JOINT     = 0x03,
	TAG_LAME_MODE_FORCE     = 0x04,
	TAG_LAME_MODE_AUTO      = 0x05,
	TAG_LAME_MODE_INTENSITY = 0x06,
	TAG_LAME_MODE_UNDEFINED = 0x07
};

enum tag_lame_surround {
	TAG_LAME_SURROUND_NONE      = 0,
	TAG_LAME_SURROUND_DPL       = 1,
	TAG_LAME_SURROUND_DPL2      = 2,
	TAG_LAME_SURROUND_AMBISONIC = 3
};

enum tag_lame_preset {
	TAG_LAME_PRESET_NONE          =    0,
	TAG_LAME_PRESET_V9            =  410,
	TAG_LAME_PRESET_V8            =  420,
	TAG_LAME_PRESET_V7            =  430,
	TAG_LAME_PRESET_V6            =  440,
	TAG_LAME_PRESET_V5            =  450,
	TAG_LAME_PRESET_V4            =  460,
	TAG_LAME_PRESET_V3            =  470,
	TAG_LAME_PRESET_V2            =  480,
	TAG_LAME_PRESET_V1            =  490,
	TAG_LAME_PRESET_V0            =  500,
	TAG_LAME_PRESET_R3MIX         = 1000,
	TAG_LAME_PRESET_STANDARD      = 1001,
	TAG_LAME_PRESET_EXTREME       = 1002,
	TAG_LAME_PRESET_INSANE        = 1003,
	TAG_LAME_PRESET_STANDARD_FAST = 1004,
	TAG_LAME_PRESET_EXTREME_FAST  = 1005,
	TAG_LAME_PRESET_MEDIUM        = 1006,
	TAG_LAME_PRESET_MEDIUM_FAST   = 1007,
	TAG_LAME_PRESET_PORTABLE      = 1010,
	TAG_LAME_PRESET_RADIO         = 1015
};




# define RGAIN_REFERENCE  83		/* reference level (dB SPL) */

# define RGAIN_SET(rgain)	((rgain)->name != RGAIN_NAME_NOT_SET)

# define RGAIN_VALID(rgain)  \
  (((rgain)->name == RGAIN_NAME_RADIO ||  \
    (rgain)->name == RGAIN_NAME_AUDIOPHILE) &&  \
   (rgain)->originator != RGAIN_ORIGINATOR_UNSPECIFIED)

# define RGAIN_DB(rgain)  ((rgain)->adjustment / 10.0)


void tag_init(struct tag_xl *);
#define tag_finish(tag)	/* nothing */
int tag_parse(struct tag_xl *, struct mad_stream const *, struct mad_frame const *);
static int parse_xing(struct tag_xing *xing, struct mad_bitptr *ptr, unsigned int *bitlen);
unsigned short crc_compute(char const *data, unsigned int length, unsigned short init);
static int parse_lame(struct tag_lame *lame, struct mad_bitptr *ptr, unsigned int *bitlen, unsigned short crc);
void rgain_parse(struct rgain *, struct mad_bitptr *);
char const *rgain_originator(struct rgain const *);







// Summary:
//    Reallocate array of pointers to unsigned char.
// Parameters:
//    src		- Pointer to array you need to reallocate. If this parameter is NULL, new array will be allocated
//    old_size	- Old size of array, these elements will be moved to reallocated memory
//    new_size	- New size of array. If this value is 0, src array will be deallocated and function will return NULL.  
// Returns:
//    Pointer to reallocated array. If new_size is 0, function will free src array and return NULL. 
//    If new_size is not 0 and function fails, return value will be NULL.
// Description:
//    This function will reallocate array of pointers and return reallocated array. 
//    Function will always allocate new memory for array, even if you decrease array from previous size
//    new memory will be allocated.
unsigned char ** reallocate_ptr(unsigned char **src, unsigned int old_size, unsigned int new_size);


static
unsigned short const crc16_table[256] = {
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,

	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,

	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,

	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};





enum {
	DECODER_NO_ERROR = 0,
	DECODER_FILE_OPEN_ERROR,
	DECODER_FILE_SEEK_POSITION_OUT_OF_BOUNDS,
	DECODER_FILESIZE_OUT_OF_BOUNDS,
	DECODER_NOT_VALID_MP3_STREAM,
	DECODER_MEMORY_ALLOCATION_FAIL,
	DECODER_NO_ID3_DATA,
	DECODER_SEEK_NOT_SUPPORTED_MANAGED,
	DECODER_SEEK_POSITION_OUT_OF_BOUNDS,
	DECODER_REVERSE_NOT_SUPPORTED_MANAGED,
	DECODER_INVALID_ID3_PARAMETER,


	DECODER_UNKNOWN_ERROR
};





wchar_t *g_mp3_error_strW[DECODER_UNKNOWN_ERROR + 1] = {
	L"No error.",
	L"Mp3Decoder: File open error.",
	L"Mp3Decoder: File sek position is out of bounds.",
	L"Mp3Decoder: File size is out of bounds.",
	L"Mp3Decoder: This is not valid mp3 stream.",
	L"Mp3Decoder: Memory allocation fail."
	L"Mp3Decoder: No ID3 data.",
	L"Mp3Decoder: Seek is not supported on managed stream.",
	L"Mp3Decoder: Seek position is out of bounds.",
	L"Mp3Decoder: Reverse mod is not supported on managed stream.",
	L"Mp3Decoder: Invalid ID3 version parameter.",

	L"Mp3Decoder: Unknown error."

};


WMp3Decoder::WMp3Decoder()
{
	err(DECODER_NO_ERROR);

// =============================================================
// ENCODER DELAY, PADDING, BLANK SAMPLES, GAPLESS PLAYING
// =============================================================

	// Blank samples at file beginning (XING/LAME/VBRI frame)
	c_nBlankSamplesAtBeginning = 0;

	// Encoder delay in samples (read from LAME/VBRI frame)
	c_nEncoderDelayInSamples = 0;

	// Encoder padding at end of file (read from LAME/VBRI frame)
	c_nEncoderEndPaddingInSamples = 0;


// =============================
//	INITIALIZE STREAM VALUES
	c_pchStreamStart = 0;
	c_pchStreamEnd = 0;
	c_nStreamSize = 0;
	c_pchStart = 0;
	c_pchEnd = 0;
	c_nSize = 0;
	c_pchPosition = 0;
	c_nBitPerSample = 0;
	c_fReverse = 0;
	c_pchBuffer = 0;
	c_fManagedStream = 0;
	c_fEndOfStream = 0;
	c_nBufferSize = 0;
	c_nCurrentPosition = 0;
	c_fPreciseSongLength = 0;
	c_fPreciseSeek = 0;

	c_Queue = NULL;


	memset(&c_xing, 0, sizeof(struct tag_xl));

	c_fReady = 0;


	memset(&c_DecoderData, 0, sizeof(DECODER_DATA));
// =================================================
//	INITIALIZE INPUT_STREAM_INFO
	memset(&c_isInfo, 0, sizeof(INPUT_STREAM_INFO));

// ==================================================

	c_nFreeBitrate = 0;

	c_pFramePtr = 0;
	c_nLastAvailableFrameIndex = 0;
	c_nAllocatedFrameNumber = 0;
	c_nCurrentFrameIndex = 0;

	c_fValidXing = 0;
	c_nXingFrameSize = 0;
	c_pchMadBuffGuard = 0;
	c_pchManagedBuffer = 0;
	c_nManagedBufferMaxSize = 0;
	c_nManagedBufferLoad = 0;
	c_nSkipSamlesNum = 0;
	c_nFrameOverlay = 0;
	c_nCurrentSeekIndex = 0;
	c_nCurrentBitrate = 0;
	
	memset(&c_left_dither, 0, sizeof(AUTODITHER_STRUCT));
	memset(&c_right_dither, 0, sizeof(AUTODITHER_STRUCT));

	c_pchBufferAllocSize = 0;

}


WMp3Decoder::~WMp3Decoder()
{
	Uninitialize();
}



int WMp3Decoder::Initialize(int param1, int param2, int param3, int param4)
{
	// INPUT_MP3_FRAME_NUMBER * 1152 samples
	c_pchBuffer = (int*) malloc(INPUT_MP3_FRAME_NUMBER * 1152 * 4);	// always use 16 bit stereo
	if(c_pchBuffer == 0)
	{
		err(DECODER_MEMORY_ALLOCATION_FAIL);
		return 0;
	}

	c_pchBufferAllocSize = INPUT_MP3_FRAME_NUMBER * 1152;

// =============================================================
// ENCODER DELAY, PADDING, BLANK SAMPLES, GAPLESS PLAYING
// =============================================================

	// Blank samples at file beginning (XING/LAME/VBRI frame)
	c_nBlankSamplesAtBeginning = 0;

	// Encoder delay in samples (read from LAME/VBRI frame)
	c_nEncoderDelayInSamples = 0;

	// Encoder padding at end of file (read from LAME/VBRI frame)
	c_nEncoderEndPaddingInSamples = 0;
// ==============================================================



	c_fPreciseSongLength = param1;
	c_fPreciseSeek = param2;
	return 1;
}


int WMp3Decoder::Uninitialize()
{
	if(c_pchBuffer)
		free(c_pchBuffer);

	c_pchBuffer = 0;
	c_pchBufferAllocSize = 0;


	return 1;
}


void WMp3Decoder::Release()
{
	delete this;
}

DECODER_ERROR_MESSAGE *WMp3Decoder::GetError()
{
	return &c_err_msg;
}






int WMp3Decoder::_OpenFile(unsigned int fSkipExtraChecking)
{

	unsigned char *start = c_pchStreamStart;
	unsigned char *end = c_pchStreamEnd;
	unsigned int size = c_nStreamSize;

	unsigned char* tmp = 0;

	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_header header;


	// check for ID3v1 tag
	if(size > 128 && memcmp(end - 127, "TAG",3) == 0)
	{
		// ID3v1 tag found, adjust stream pointers to exclude tag from stream
		end -= 128;
		size -= 128;	
	}

	// check for ID3v2 tag
	if(( size  > 10 ) && (memcmp(start,"ID3",3) == 0) && start[6] < 0x80 && start[7] < 0x80 && start[8] < 0x80 && start[9] < 0x80)
	{ 
		// ID3v2 tag found
		// calculate ID3v2 frame size
		unsigned int id3v2_size	= DecodeSyncSafeInteger4( start[6], start[7], start[8], start[9]);
		
		// add ID3v2 header size
		id3v2_size += 10; // size of ID3V2 frame
		// check if this is valid ID3v2 tag
		if(size >  ( id3v2_size + MIN_FRAME_SIZE))
		{

			if(SKIP_ID3TAG_AT_BEGINNING && ( *(start + id3v2_size) == 0xFF) &&  (( *(start + id3v2_size + 1) & 0xE0) == 0xE0))
			{
				// adjust stream pointers to exclude ID3v2 tag from stream
				start += id3v2_size;
				size -= id3v2_size;	
			}
		}
	}

	
	// now we have clear mp3 stream without ID3 tags

	// check if stream is large enough to contain one mp3 frame
	if(size < MAD_BUFFER_GUARD)
	{
		err(DECODER_NOT_VALID_MP3_STREAM);
		return 0;
	}

	// initialize LIBMAD structures
	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_header_init(&header);
	// set pointer to first byte after ID3v2 tag
	mad_stream_buffer(&stream, start, size - MAD_BUFFER_GUARD);

	// initialize some values 
	unsigned char* first_frame = start;
	unsigned int sampling_rate = 0;
	unsigned int channel = 0;
	int layer = 0;
	int channel_mode = 0;
	int flags = 0;
	int emphasis = 0;
	int mode_extension = 0;
	unsigned int bitrate = 0;
	int sample_per_frame = 0;
	int mode;

	// scan stream until one valid mp3 frame is found
	while(1)
	{
		while(mad_frame_decode(&frame, &stream))
		{
			if(MAD_RECOVERABLE(stream.error))
				continue;

			// fatal error
			mad_stream_finish(&stream);
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			err(DECODER_NOT_VALID_MP3_STREAM);
			return 0;
		}

		// possible first valid mp3 frame
		first_frame =  (unsigned char*) stream.this_frame;

		// we have possible first frame, but we need more checking
		
		// remember some infos about this possible first frame
		sampling_rate = frame.header.samplerate;
		layer = frame.header.layer;
		channel_mode = frame.header.mode;
		channel = ( frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
		emphasis = frame.header.emphasis;
		mode_extension = frame.header.mode_extension;
		bitrate = frame.header.bitrate;
		flags = frame.header.flags;
		sample_per_frame = (32 * MAD_NSBSAMPLES(&frame.header)); // (frame.header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152;
		mode = frame.header.mode;


		// decode next frame
		if(mad_frame_decode(&frame, &stream))
		{
			if(MAD_RECOVERABLE(stream.error))
				continue;

			// fatal error
			mad_stream_finish(&stream);
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			err(DECODER_NOT_VALID_MP3_STREAM);
			return 0;
		}
		else
		{
			// we have next valid mp3 frame, check frame data that must be constant for all mp3 frames
			int spf = (32 * MAD_NSBSAMPLES(&frame.header)); // (frame.header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152;
		
			if(sampling_rate != frame.header.samplerate || layer != frame.header.layer || spf != sample_per_frame ||
							mode_extension != frame.header.mode_extension || mode != frame.header.mode || frame.header.bitrate > 600000)
			{
					continue; // this wasn't valid mp3 frame, search for next first frame
			}
					
			break;	
		}		
	}




	// now we have valid first frame

	// free bitrate
	int freebitrate = stream.freerate;
	
	// calculate new size, skip first frame offset
	size -= (first_frame - start);
	start = first_frame;


	if(size < MAD_BUFFER_GUARD)
	{
		mad_stream_finish(&stream);
		mad_frame_finish(&frame);
		mad_header_finish(&header);
		err(DECODER_NOT_VALID_MP3_STREAM);
		return 0;
	}


// =============================================================
// ENCODER DELAY, PADDING, BLANK SAMPLES, GAPLESS PLAYING
// =============================================================

	// Blank samples at file beginning (XING/LAME/VBRI frame)
	c_nBlankSamplesAtBeginning = 0;

	// Encoder delay in samples (read from LAME/VBRI frame)
	c_nEncoderDelayInSamples = 0;

	// Encoder padding at end of file (read from LAME/VBRI frame)
	c_nEncoderEndPaddingInSamples = 0;
// ==============================================================


	unsigned int song_length_frames = 0;
	unsigned int xing_frame_size = 0;
	unsigned int valid_xing = 0;
	unsigned int vbr = 0;


	// now, check if we have valid XING frame as first frame in stream
	mad_stream_buffer(&stream, start, size - MAD_BUFFER_GUARD);




	if(mad_frame_decode(&frame, &stream) == 0)
	{

		// check if first frame is XING frame
		
		if(tag_parse(&c_xing, &stream, &frame) == 0)
		{

			if(c_xing.flags != 0) // we have empty blank frame with XING/LAME/VBRI, skip this frame
			{
				c_nBlankSamplesAtBeginning = (32 * MAD_NSBSAMPLES(&frame.header));
			}

			if(c_xing.flags & TAG_LAME) // LAME tag, get encoder delay and padding
			{
				c_nEncoderDelayInSamples = c_xing.lame.start_delay;
				c_nEncoderEndPaddingInSamples = c_xing.lame.end_padding;
			}
			else if(c_xing.flags & TAG_VBRI)
			{
				c_nEncoderDelayInSamples = c_xing.lame.start_delay - (32 * MAD_NSBSAMPLES(&frame.header));
			}
			else if(c_xing.flags & TAG_XING)
			{ // we have XING frame
				// calculate song length
				if((c_xing.xing.flags & TAG_XING_FRAMES) && c_xing.xing.flags & TAG_XING_TOC)
				{
					xing_frame_size = stream.next_frame - stream.this_frame;
					// we have valid xing frame
					valid_xing = 1;			
				}
			}	
		}
	}


	if(size < MAD_BUFFER_GUARD)
	{
		mad_stream_finish(&stream);
		mad_frame_finish(&frame);
		mad_header_finish(&header);
		err(DECODER_NOT_VALID_MP3_STREAM);
		return 0;
	}




	// ===============================================
	// GET SONG LENGTH in song_length_frames
	// ===============================================

	if(valid_xing)
	{
		// we have xing frame, song length is written in XING frame, estimate average stream bitrate
		// get precise song length in frames from XING frame data
		song_length_frames = (unsigned int) c_xing.xing.frames;
		bitrate = MulDiv(size, 8 * sampling_rate, sample_per_frame * song_length_frames);
		vbr = 1;

	}
	else
	{
		if(c_fPreciseSongLength)
		{

			// get number of frames to preallocate array for all frame pointers, prevent multiple array reallocations
			unsigned int prealloc = 0;
			if(valid_xing)
			{
				// we have xing frame, use info about number of frames
				prealloc = c_xing.xing.frames;
			}
			else
			{
				// calculate number of frames from stream size and bitrate
				if(bitrate < 112000)
					bitrate = 112000;	// prevent allocation of unneeded memory on VBR streams with low bitrate first frame
				// calculate number of frames from stream size and average bitrate
				prealloc = MulDiv(size, sampling_rate * 8, sample_per_frame * bitrate) + 10; 
			}

			song_length_frames = get_frame_pointers(start, size, &vbr, &bitrate, &c_pFramePtr, prealloc);
		
			if(song_length_frames == 0)
			{
				mad_frame_finish(&frame);
				mad_header_finish(&header);
				mad_stream_finish(&stream);
				err(DECODER_NOT_VALID_MP3_STREAM);
				return 0;
			}

			c_nLastAvailableFrameIndex = song_length_frames - 1;
		}
		else
		{
			// we don't have xing frame, scan some frames and check if stream  is VBR or CBR
			mad_stream_buffer(&stream, start, size);
			unsigned int frame_num = 0;
			unsigned int sum_size = 0;
			
			tmp = 0;
			while(frame_num < SEARCH_DEEP_VBR)
			{
				if(mad_header_decode(&header,&stream)) { // if some error
					if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
						continue;
			
					if(stream.error == MAD_ERROR_BUFLEN)
					{ // if buffer end
						if(tmp)
						{
							free(tmp);
							tmp = 0;
							break;
						}
						// fix MAD_BUFFER_GUARD problem
						unsigned int len = end + 1 - stream.this_frame;	
						tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
						if(!tmp)
							break;
						// copy last frame into buffer and add MAD_BUFFER_GUARD zero bytes to end
						memcpy(tmp, stream.this_frame, len);
						memset(tmp + len, 0, MAD_BUFFER_GUARD);
						mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
						// try again 
						continue;
					}
				}

				if(header.bitrate != bitrate)  // bitrate changed
					vbr = 1;	
		
				sum_size += (stream.next_frame - stream.this_frame);
				frame_num++;	
			}
 
			if(tmp)
				free(tmp);

			tmp = 0;

			if(vbr)
			{
				// VBR without XING header, scan whole file
	
				// calculate number of frames from stream size and bitrate
				unsigned int prealloc = 0;
				if(bitrate < 112000)
					bitrate = 112000;	// prevent allocation of unneeded memory on VBR streams with low bitrate first frame
				// calculate number of frames from stream size and average bitrate
				prealloc = MulDiv(size, sampling_rate * 8, sample_per_frame * bitrate) + 10; 
			

				song_length_frames = get_frame_pointers(start, size, &vbr, &bitrate, &c_pFramePtr, prealloc);
				if(song_length_frames == 0)
				{
					mad_frame_finish(&frame);
					mad_header_finish(&header);
					mad_stream_finish(&stream);
					err(DECODER_NOT_VALID_MP3_STREAM);
					return 0;
				}

				c_nLastAvailableFrameIndex = song_length_frames - 1;
				c_nAllocatedFrameNumber = song_length_frames;
		
			}
			else
			{ 
				// CBR, estimate song length from stream size.
				song_length_frames = (unsigned int) MulDiv(size ,  sampling_rate * 8, sample_per_frame * bitrate);	
			}
		}
	}

// ==========================================================
// ENABLE PRECISE SEEK FUNCTION
// ==========================================================

	if(c_fPreciseSeek && c_pFramePtr == NULL) // if we already have c_pFramePtr, accurate seek is already enabled by accurate song length
	{
		// Allocate array and this will enable accurate seek function, we will get poiners
		// to frames when seek occurs by scaning from first frame to specified frame.
		unsigned char **ptr = reallocate_ptr(0, 0, song_length_frames);
		if(ptr == 0)
		{
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			mad_stream_finish(&stream);
			err(DECODER_MEMORY_ALLOCATION_FAIL);
			return 0;
		}

		c_pFramePtr = ptr;
		c_pFramePtr[0] = start;
		c_nLastAvailableFrameIndex = 0;
		c_nAllocatedFrameNumber = song_length_frames;
	}



	mad_frame_finish(&frame);
	mad_header_finish(&header);
	mad_stream_finish(&stream);


	if(c_fManagedStream)
	{
	// add data into queue
		if(c_Queue->CutDataFifo(start - c_pchStreamStart) == 0)
		{
			err(DECODER_MEMORY_ALLOCATION_FAIL);
			return 0;
		}
	}

	// set stream info
	c_pchStart = start;
	c_nSize = size;
	c_pchEnd = c_pchStart + c_nSize - 1;
	c_pchPosition = c_pchStart;

	// get informations of input stream
	// WE ALWAYS USE 16 bit per sample

	// save original bit per sample
	c_nBitPerSample = 16;


	c_isInfo.nSampleRate = sampling_rate;
	c_isInfo.nChannel = channel;
	c_isInfo.nBitPerSample = 16;
	c_isInfo.nBlockAlign = sample_per_frame;
	c_isInfo.nFileBitrate = bitrate;
	c_isInfo.fVbr = vbr;
	c_isInfo.nLength = song_length_frames * sample_per_frame - c_nBlankSamplesAtBeginning - c_nEncoderDelayInSamples - c_nEncoderEndPaddingInSamples;

	c_nFreeBitrate = freebitrate;

	c_fValidXing = valid_xing;
	c_nXingFrameSize = xing_frame_size;


	char *mpeg_ver;
	if( (flags & MAD_FLAG_MPEG_2_5_EXT))
		mpeg_ver = "MPEG 2.5";
	else if((flags & MAD_FLAG_LSF_EXT))
		mpeg_ver = "MPEG 2";
	else
		mpeg_ver = "MPEG 1";

	char *layer_ver;
	switch(layer)
	{
		case MAD_LAYER_I:
			layer_ver = "LAYER I";
		break;

		case MAD_LAYER_II:
			layer_ver =	"LAYER II";
		break;

		case MAD_LAYER_III:
			layer_ver =	"LAYER III"; 
		break;
			
		default:
			layer_ver =	"Unknown"; 
		break;		
	}

	char *ch_mode;
	switch(channel_mode)
	{
		case MAD_MODE_SINGLE_CHANNEL:
			ch_mode = "MONO";
		break;

		case MAD_MODE_DUAL_CHANNEL:
			ch_mode = "DUAL CHANNEL";
		break;

		case MAD_MODE_JOINT_STEREO:
			ch_mode = "JOINT STEREO";
		break;

		case MAD_MODE_STEREO:
			ch_mode = "STEREO";
		break;
	}

	sprintf(c_pchDescription, "%s %s %s", mpeg_ver, layer_ver, ch_mode);
	c_isInfo.pchStreamDescription = c_pchDescription;


	// initialize decoder
	mad_stream_init(&c_stream);
	mad_frame_init(&c_frame);
	mad_synth_init(&c_synth);


	if(c_fManagedStream)
	{
		// set decoder to 0, we need to use data from queue
		mad_stream_buffer(&c_stream, c_pchManagedBuffer, 0);
	}
	else
	{

		// set decoder to first valid frame
		mad_stream_buffer(&c_stream, c_pchStart, c_nSize);
	}


	c_nCurrentPosition = 0;

	// skip decoder and encoder delay samples at start
	c_nSkipSamlesNum = LIBMAD_DELAY + c_nBlankSamplesAtBeginning + c_nEncoderDelayInSamples;

	// check if working buffer can take padding samples

	unsigned int nNeedAllocatedSamples = INPUT_MP3_FRAME_NUMBER * 1152 + c_nEncoderEndPaddingInSamples;
	if(c_pchBufferAllocSize < nNeedAllocatedSamples)
	{
		int *tmp = (int*) malloc(nNeedAllocatedSamples * 4);	// always use 16 bit stereo
		if(tmp == 0)
		{
			err(DECODER_MEMORY_ALLOCATION_FAIL);
			return 0;
		}

		if(c_pchBuffer)
			free(c_pchBuffer);

		c_pchBuffer = tmp;
		c_pchBufferAllocSize = nNeedAllocatedSamples;

	}

	// fix pading samples because we have decoder delay
	if(c_nEncoderEndPaddingInSamples >= LIBMAD_DELAY)
		c_nEncoderEndPaddingInSamples -= LIBMAD_DELAY;	

	c_fReady = 1;

	return 1;


}

int WMp3Decoder::Close()
{
// =============================================================
// free accurate seek structures and initialize values
	if(c_pFramePtr)
		free(c_pFramePtr);

	c_pFramePtr = 0;
	c_nLastAvailableFrameIndex = 0;
	c_nAllocatedFrameNumber = 0;
// =============================================================
// free mad buffer guard
	if(c_pchMadBuffGuard)
	{
		free(c_pchMadBuffGuard);
		c_pchMadBuffGuard = NULL;
	}
// ==============================================================
// free managed buffer
	if(c_pchManagedBuffer)
		free(c_pchManagedBuffer);

	c_pchManagedBuffer = 0;
	c_nManagedBufferMaxSize = 0;
// ===============================================================


	if(c_fReady == 0)
		return 1;

//=================================================================
// initialize stream data
	c_pchStreamStart = 0;
	c_pchStreamEnd = 0;
	c_nStreamSize = 0;
	c_pchStart = 0;
	c_pchEnd = 0;
	c_nSize = 0;
	c_pchPosition = 0;
	c_fManagedStream = 0;
	c_fEndOfStream = 0;
	c_nCurrentPosition = 0;

// ================================

//	INITIALIZE INPUT_STREAM_INFO
	memset(&c_isInfo, 0, sizeof(INPUT_STREAM_INFO));

// ====================================================
// initialize XING frame
	c_fValidXing = 0;
	c_nXingFrameSize = 0;
	memset(&c_xing, 0, sizeof(struct tag_xl));
// ====================================================



// clear working buffer
	c_nBufferSize = 0;

// clear decoder data
	memset(&c_DecoderData, 0, sizeof(DECODER_DATA));
	memset(&c_left_dither, 0, sizeof(AUTODITHER_STRUCT));
	memset(&c_right_dither, 0, sizeof(AUTODITHER_STRUCT));

// clear other data
	c_nManagedBufferLoad = 0;
	c_nSkipSamlesNum = 0;
	c_nFrameOverlay = 0;
	c_nCurrentFrameIndex = 0;
	c_nCurrentSeekIndex = 0;
	c_fEndOfStream = 0;


// =============================================================
// ENCODER DELAY, PADDING, BLANK SAMPLES, GAPLESS PLAYING
// =============================================================

	// Blank samples at file beginning (XING/LAME/VBRI frame)
	c_nBlankSamplesAtBeginning = 0;

	// Encoder delay in samples (read from LAME/VBRI frame)
	c_nEncoderDelayInSamples = 0;

	// Encoder padding at end of file (read from LAME/VBRI frame)
	c_nEncoderEndPaddingInSamples = 0;
// ==============================================================

	c_fReady = 0;
	return 1;
}




INPUT_STREAM_INFO *WMp3Decoder::GetStreamInfo()
{
	ASSERT_W(c_fReady);
	return &c_isInfo;
}


wchar_t **WMp3Decoder::GetID3Info(int version, char *pStream, unsigned int nStreamSize, int param1, int param2)
{
	ASSERT_W(c_pchStreamStart);
	err(DECODER_NO_ERROR);
	return c_tag.LoadID3Info(c_pchStreamStart, c_nStreamSize, version);
}






int WMp3Decoder::Seek(unsigned int nSamples)
{
	ASSERT_W(c_fReady);

	err(DECODER_NO_ERROR);

	if(c_fManagedStream) // can't seek on managed stream
	{
		err(DECODER_SEEK_NOT_SUPPORTED_MANAGED);
		return 0;
	}

	
	// no frame overlay
	c_nFrameOverlay = 0;	

	if(nSamples >= c_isInfo.nLength)
	{
		err(DECODER_SEEK_POSITION_OUT_OF_BOUNDS);
		return 0;

	}

	// clear all position data and decoder
	_ClearForSeek();

	unsigned char *nNewPosition = c_pchStart;

	if(c_pFramePtr)	// we can use precise seek
	{
		// get frame index 
		unsigned int nFrameIndex = getFrameIndex(nSamples);
		if(nFrameIndex == FRAME_INDEX_ERR)
		{
			err(DECODER_MEMORY_ALLOCATION_FAIL);
			return 0;
		}

		// skip XING/LAME/VBRI blank frame
		c_nSkipSamlesNum = c_nBlankSamplesAtBeginning;

				
		nNewPosition = c_pFramePtr[nFrameIndex];
		// calculate number of samples to skip within frame
		c_nSkipSamlesNum += (nSamples - nFrameIndex * c_isInfo.nBlockAlign);
			

		c_nCurrentFrameIndex = nFrameIndex;
		c_nCurrentSeekIndex = nFrameIndex;

		// fix encoder and decoder delay
		c_nSkipSamlesNum += LIBMAD_DELAY + c_nEncoderDelayInSamples;
		
	
		if(c_fReverse)
			c_nSkipSamlesNum = 0;
	}
	else // no precide seek
	{
		// use standard seek
		if(c_fValidXing)
		{
			// check if we have valid TOC table
			if ((c_xing.flags & TAG_XING) && (c_xing.xing.flags & TAG_XING_TOC))
			{
			
				double pa, pb, px;
				double percentage = 100.0 * (double) nSamples / (double) c_isInfo.nLength;
			
				int perc = (int) percentage;
				if (perc > 99)
					perc = 99;

				pa = c_xing.xing.toc[perc];
				if (perc < 99)
					pb = c_xing.xing.toc[perc+1];
				else 
					pb = 256;
         
				px = pa + (pb - pa) * (percentage - perc);
				nNewPosition = c_pchStart  + (unsigned int) (((double) ( c_nSize + c_nXingFrameSize )   / 256.0) * px);
			
			}
			else
			{
				// TOC table is innvalid, calculate seek position from stream size 
				nNewPosition = c_pchStart + (unsigned int) ((double) nSamples / (double) c_isInfo.nLength * (double) c_nSize);  
			}
		}
		else
		{	
			// we don't have XING header, calculate new position
			nNewPosition = c_pchStart + (unsigned int) ((double) nSamples / (double) c_isInfo.nLength * (double) c_nSize); 
		}

		
		// start delay
		if(nNewPosition == c_pchStart)
			c_nSkipSamlesNum = LIBMAD_DELAY + c_nBlankSamplesAtBeginning + c_nEncoderDelayInSamples;
			
	}


	if(nNewPosition >= c_pchEnd)
	{
		err(DECODER_SEEK_POSITION_OUT_OF_BOUNDS);
		c_nSkipSamlesNum = 0;
		return 0;
	}


	c_pchPosition = nNewPosition;

	// clear all buffers
	mad_frame_finish(&c_frame);
	mad_synth_finish(&c_synth);
	mad_stream_finish(&c_stream);


	// initialize decoder
	mad_stream_init(&c_stream);
	mad_frame_init(&c_frame);
	mad_synth_init(&c_synth);
	// set free bitrate value
	c_stream.freerate = c_nFreeBitrate;
	// set decoder to first valid frame
	mad_stream_buffer(&c_stream, c_pchStart, c_nSize);

	// skip bytes, this is seek function
	mad_stream_skip(&c_stream,  c_pchPosition - c_pchStart);

	c_nCurrentPosition = nSamples;

	return 1;

}


int WMp3Decoder::GetData(DECODER_DATA *pDecoderData)
{
	ASSERT_W(c_fReady);
	ASSERT_W(pDecoderData);
	ASSERT_W(pDecoderData->pSamples);

	unsigned int nSamplesNeed = pDecoderData->nSamplesNum;	// number of samples requested by user
	unsigned int nSamplesHave = 0;

	int *pchOutBuffer = (int*) pDecoderData->pSamples;	// use int because here we are working always with 16 bit stereo samples
	
	// initialize this to 0													
	pDecoderData->nBufferDone = 0;
	pDecoderData->nBufferInQueue = 0;
	pDecoderData->nBytesInQueue = 0;
	pDecoderData->nSamplesNum = 0;
	pDecoderData->nStartPosition = 0;
	pDecoderData->nEndPosition = 0;

	c_fEndOfStream = pDecoderData->fEndOfStream;

	c_DecoderData.nBufferDone = 0;

	unsigned int nPaddingSamples = c_nEncoderEndPaddingInSamples;

	 
	if(c_fReverse == 1 || c_fManagedStream != 0)
		nPaddingSamples = 0;	



	while(nSamplesNeed)	// try to get all data that user need
	{
		if(c_nBufferSize <= nPaddingSamples)	// we don't have any data in buffer, so get new data from decoder
		{
			// try to fill working buffer
			int ret = _FillBuffer(this);

			// data for managed stream
			pDecoderData->nBufferDone += c_DecoderData.nBufferDone;
			pDecoderData->nBufferInQueue = c_DecoderData.nBufferInQueue;
			pDecoderData->nBytesInQueue = c_DecoderData.nBytesInQueue;

			switch(ret)
			{
				case DECODER_OK: // buffer is loaded, all is OK by decoder, so try again with buffer
				continue;

				case DECODER_END_OF_DATA:	// decoder has no data anymore, end of stream or fatal error
				case DECODER_FATAL_ERROR:
				{
					if(nSamplesHave)	// if we have some data already in user buffer, give this to user
					{
						pDecoderData->nSamplesNum = nSamplesHave;
						return DECODER_OK;
					}
				}
				return ret;

				case DECODER_MANAGED_NEED_MORE_DATA:
				{
					// we can't get more data from decoder and we don't have enough data in user buffer
					// to fulfill user request

					if(c_fEndOfStream)
					{
						// user specifies that there will be no data, so give user samples we have 
						if(nSamplesHave)
						{
							pDecoderData->nSamplesNum = nSamplesHave;
							return DECODER_OK;
						}	

						// we don't have any data, user specifies that there will be no more data, return end of data.
						return DECODER_END_OF_DATA;
					}

					// we can expect more data from user so we will not return partial data to user,
					// instead we will save this partial data into internal buffer and indicate user that we need
					// more input data for decoder

					if(nSamplesHave)	// we need to save samples from user buffer
					{
						// check if we need to reallocate working buffer
						if(c_pchBufferAllocSize < nSamplesHave + c_nBufferSize)	// reallocate buffer
						{
							int *tmp = (int*) malloc((nSamplesHave + c_nBufferSize) * 4);	// always use 16 bit stereo
							if(tmp == 0)
								return DECODER_FATAL_ERROR;

							free(c_pchBuffer);
							c_pchBuffer = tmp;
							// new allocated size
							c_pchBufferAllocSize = nSamplesHave + c_nBufferSize;		
						}

						if(c_nBufferSize)
						{
							// move padding samples to beginning of buffer
							MoveMemory(c_pchBuffer + nSamplesHave, c_pchBufferPos, c_nBufferSize * 4);

						}
					
						
						// copy back data from user buffer to our internal buffer
						unsigned int i;
						pchOutBuffer = (int*) pDecoderData->pSamples;
						for(i = 0; i < nSamplesHave; i++)
							c_pchBuffer[i] = pchOutBuffer[i];
								
						c_nBufferSize = nSamplesHave + c_nBufferSize;
						c_pchBufferPos = c_pchBuffer;	
						
					}
					
				}
				return DECODER_MANAGED_NEED_MORE_DATA;
			}
		}
		else // we have some data in buffer
		{
			// we have enough data for user
			if(c_nBufferSize >= nSamplesNeed + nPaddingSamples)
			{
				// we have enough data in  buffer, so we have all data that user needs
				unsigned int i;
				for(i = 0; i < nSamplesNeed; i++)
					pchOutBuffer[i] = c_pchBufferPos[i];

				nSamplesHave += nSamplesNeed;
					
				// move pointer to next sample in buffer, need this for next call to GetData
				c_pchBufferPos = &c_pchBufferPos[nSamplesNeed];
				// calculate number of remaining samples in buffer
				c_nBufferSize -= nSamplesNeed;
				// we have all samples we need, don't loop anymore
				nSamplesNeed = 0;
			}
			else
			{

				unsigned int nRealHave;
				// we can give some data to user
				if(c_nBufferSize > nPaddingSamples)
				{
					// we have some data in buffer, but we need to decrease size by padding samples
					// because we need to have padding samples in buffer
					nRealHave = c_nBufferSize - nPaddingSamples;
					unsigned int i;
					for(i = 0; i < nRealHave; i++)
						pchOutBuffer[i] = c_pchBufferPos[i];

					nSamplesHave += nRealHave;
					// calculate number of samples we need
					nSamplesNeed -= nRealHave;
					// move pointer of user buffer
					pchOutBuffer = &pchOutBuffer[nRealHave];

					// move padding samples to beginning of buffer
					memmove(c_pchBuffer, &c_pchBufferPos[nRealHave], nPaddingSamples * 4);
						
					c_nBufferSize = nPaddingSamples;
								
				}
				else
				{
					return DECODER_FATAL_ERROR;
					
				}		
			}
		}
	}


	pDecoderData->nSamplesNum = nSamplesHave;
	pDecoderData->nStartPosition = c_nCurrentPosition;
	if(c_fReverse)
		c_nCurrentPosition -= nSamplesHave;
	else
		c_nCurrentPosition += nSamplesHave;
	pDecoderData->nEndPosition = c_nCurrentPosition;


	return DECODER_OK;

}



int WMp3Decoder::GetBitrate(int fAverage)
{
	if(fAverage)
		return c_isInfo.nFileBitrate;
	
	return c_nCurrentBitrate;
	
}


int WMp3Decoder::SetReverseMode(int fReverse)
{
	ASSERT_W(c_fReady);

	err(DECODER_NO_ERROR);
	
	if(fReverse)
	{
		if(c_fManagedStream)
		{
			err(DECODER_REVERSE_NOT_SUPPORTED_MANAGED);
			return 0;
		}

		if(c_pFramePtr == NULL || c_fPreciseSeek == 0 )
		{	
			// dynamic scan
			unsigned char **ptr = reallocate_ptr(0, 0, 1);
			if(ptr == 0)
			{
				err(DECODER_MEMORY_ALLOCATION_FAIL);
				return 0;
			}

			c_pFramePtr = ptr;
			c_pFramePtr[0] = c_pchStart;
			c_nLastAvailableFrameIndex = 0;
			c_nAllocatedFrameNumber = 1;
			c_fPreciseSeek = 1;
		}
		

		c_fReverse = 1;
	}
	else
		c_fReverse = 0;


	return 1;
}




void WMp3Decoder::_ClearForSeek()
{

// free mad buffer guard
	if(c_pchMadBuffGuard)
	{
		free(c_pchMadBuffGuard);
		c_pchMadBuffGuard = 0;
	}

// clear working buffer
	c_nBufferSize = 0;

// clear decoder data
	memset(&c_DecoderData, 0, sizeof(DECODER_DATA));
	memset(&c_left_dither, 0, sizeof(AUTODITHER_STRUCT));
	memset(&c_right_dither, 0, sizeof(AUTODITHER_STRUCT));

// clear other data
	c_nManagedBufferLoad = 0;
	c_nSkipSamlesNum = 0;
	c_nFrameOverlay = 0;
	c_nCurrentFrameIndex = 0;
	c_nCurrentSeekIndex = 0;
	c_fEndOfStream = 0;

}



/*
* NAME:	tag->init()
* DESCRIPTION:	initialize tag structure
*/

void tag_init(struct tag_xl *tag)
{
	tag->flags      = 0;
	tag->encoder[0] = 0;
	memset(&tag->xing, 0, sizeof(struct tag_xing));
	memset(&tag->lame, 0, sizeof(struct tag_lame));
}


/*
* NAME:	tag->parse()
* DESCRIPTION:	parse Xing/LAME tag(s)
*/
int tag_parse(struct tag_xl *tag, struct mad_stream const *stream, struct mad_frame const *frame)
{
	struct mad_bitptr ptr = stream->anc_ptr;
	struct mad_bitptr start = ptr;
	unsigned int bitlen = stream->anc_bitlen;
	unsigned long magic;
	unsigned long magic2;
	int i;

	tag_init(tag); //tag->flags = 0;

	if (bitlen < 32)
		return -1;

	magic = mad_bit_read(&ptr, 32);
    
	bitlen -= 32;

	if (magic != XING_MAGIC && magic != INFO_MAGIC && magic != LAME_MAGIC)
	{
			/*
			* Due to an unfortunate historical accident, a Xing VBR tag may be
			* misplaced in a stream with CRC protection. We check for this by
			* assuming the tag began two octets prior and the high bits of the
			* following flags field are always zero.
			*/

			if (magic != ((XING_MAGIC << 16) & 0xffffffffL) &&
				magic != ((INFO_MAGIC << 16) & 0xffffffffL))
			{
				//check for VBRI tag
				if (bitlen >= 400)
				{
					mad_bit_skip(&ptr, 256);
					magic2 = mad_bit_read(&ptr, 32);
					if (magic2 == VBRI_MAGIC)
					{
						mad_bit_skip(&ptr, 16); //16 bits - version
						tag->lame.start_delay = mad_bit_read(&ptr, 16); //16 bits - delay
						mad_bit_skip(&ptr, 16); //16 bits - quality
						tag->xing.bytes = mad_bit_read(&ptr, 32); //32 bits - bytes
						tag->xing.frames = mad_bit_read(&ptr, 32); //32 bits - frames
						unsigned int table_size = mad_bit_read(&ptr, 16);
						unsigned int table_scale = mad_bit_read(&ptr, 16);
						unsigned int entry_bytes = mad_bit_read(&ptr, 16);
						unsigned int entry_frames = mad_bit_read(&ptr, 16);
						{
						    unsigned int Entry = 0, PrevEntry = 0, Percent, SeekPoint = 0, i = 0;
						    float AccumulatedTime = 0;
                            float TotalDuration = (float) (1000.0 * tag->xing.frames * ((frame->header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152) / frame->header.samplerate);
						    float DurationPerVbriFrames = TotalDuration / ((float)table_size + 1);
						    for (Percent=0;Percent<100;Percent++)
						    {
                                float EntryTimeInMilliSeconds = ((float)Percent/100) * TotalDuration;
                                while (AccumulatedTime <= EntryTimeInMilliSeconds)
                                {
                                    PrevEntry = Entry;
                                    Entry = mad_bit_read(&ptr, entry_bytes * 8) * table_scale;
                                    i++;
                                    SeekPoint += Entry;
                                    AccumulatedTime += DurationPerVbriFrames;
                                    if (i >= table_size) break;
                                }
                                unsigned int fraction = ( (int)(((( AccumulatedTime - EntryTimeInMilliSeconds ) / DurationPerVbriFrames ) 
			                                             + (1.0f/(2.0f*(float)entry_frames))) * (float)entry_frames));
			                    unsigned int SeekPoint2 = SeekPoint - (int)((float)PrevEntry * (float)(fraction) 
				                                         / (float)entry_frames);
				                unsigned int XingPoint = (256 * SeekPoint2) / tag->xing.bytes;
							

				                if (XingPoint > 255) XingPoint = 255;
				                tag->xing.toc[Percent] = (unsigned char)(XingPoint & 0xFF);
                            }
                        }
						tag->flags |= (TAG_XING | TAG_VBRI);
						tag->xing.flags = (TAG_XING_FRAMES | TAG_XING_BYTES | TAG_XING_TOC);
					
						return 0;
					}
				}
				return -1;
			}

			magic >>= 16;

			/* backtrack the bit pointer */

			ptr = start;
			mad_bit_skip(&ptr, 16);
			bitlen += 16;
	}

	if ((magic & 0x0000ffffL) == (XING_MAGIC & 0x0000ffffL))
		tag->flags |= TAG_VBR;

	/* Xing tag */

	if (magic == LAME_MAGIC)
	{
		ptr = start;
		bitlen += 32;
	}
	else if (parse_xing(&tag->xing, &ptr, &bitlen) == 0)
		tag->flags |= TAG_XING;

	/* encoder string */

	if (bitlen >= 20 * 8)
	{
		start = ptr;

		for (i = 0; i < 20; ++i) {
			tag->encoder[i] = (char)  mad_bit_read(&ptr, 8);

			if (tag->encoder[i] == 0)
				break;

			/* keep only printable ASCII chars */

			if (tag->encoder[i] < 0x20 || tag->encoder[i] >= 0x7f) {
				tag->encoder[i] = 0;
				break;
			}
		}

		tag->encoder[20] = 0;
		ptr = start;
	}

	/* LAME tag */

	if (memcmp(tag->encoder, "LAME", 4) == 0 && stream->next_frame - stream->this_frame >= 192)
	{

		unsigned int crc_size = 190; // MPEG1 STEREO (SIDE INFO is 32 bytes )
		if(frame->header.flags & MAD_FLAG_LSF_EXT)
		{	
			if(frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
				crc_size = 175; // MPEG 2/2.5 (LSF) STEREO (SIDE INFO is 17)
			else
				crc_size = 167; // MPEG 2/2.5 (LSF) MONO (SIDE INFO is 9)
		}
		else
		{
			if(frame->header.mode == MAD_MODE_SINGLE_CHANNEL)	
				crc_size = 175; // MPEG1 MONO (SIDE INFO is 17)
			
		}


        unsigned short crc = crc_compute((const char *) stream->this_frame, crc_size, 0x0000);
		if (parse_lame(&tag->lame, &ptr, &bitlen, crc) == 0)
		{
             tag->flags |= TAG_LAME;
			 tag->encoder[9] = 0;
        }
	}
	else {
		for (i = 0; i < 20; ++i) {
			if (tag->encoder[i] == 0)
				break;

			/* stop at padding chars */

			if (tag->encoder[i] == 0x55) {
				tag->encoder[i] = 0;
				break;
			}
		}
	}

	return 0;
}

unsigned short crc_compute(char const *data, unsigned int length,
						   unsigned short init)
{
	register unsigned int crc;

	for (crc = init; length >= 8; length -= 8)
	{
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	}

	switch (length)
	{
	  case 7: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 6: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 5: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 4: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 3: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 2: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 1: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
	  case 0: break;
	}

	return (unsigned short) crc;
}



/*
* NAME:	parse_xing()
* DESCRIPTION:	parse a Xing VBR tag
*/


static
int parse_xing(struct tag_xing *xing,
struct mad_bitptr *ptr, unsigned int *bitlen)
{
	if (*bitlen < 32)
		goto fail;

    xing->flags = mad_bit_read(ptr, 32);
	*bitlen -= 32;

	if (xing->flags & TAG_XING_FRAMES) {
		if (*bitlen < 32)
			goto fail;

		xing->frames = mad_bit_read(ptr, 32);
		*bitlen -= 32;
	}

	if (xing->flags & TAG_XING_BYTES) {
		if (*bitlen < 32)
			goto fail;

		xing->bytes = mad_bit_read(ptr, 32);
		*bitlen -= 32;
	}

	if (xing->flags & TAG_XING_TOC) {
		int i;

		if (*bitlen < 800)
			goto fail;

		for (i = 0; i < 100; ++i)
			xing->toc[i] = (unsigned char) mad_bit_read(ptr, 8);

		*bitlen -= 800;
	}

	if (xing->flags & TAG_XING_SCALE) {
		if (*bitlen < 32)
			goto fail;

		xing->scale = mad_bit_read(ptr, 32);
		*bitlen -= 32;
	}

	return 0;

fail:
	xing->flags = 0;
	return -1;
}


/*
* NAME:	parse_lame()
* DESCRIPTION:	parse a LAME tag
*/


static
int parse_lame(struct tag_lame *lame,
struct mad_bitptr *ptr, unsigned int *bitlen,
	unsigned short crc)
{
	struct mad_bitptr save = *ptr;
	unsigned long magic;
	char const *version;

	if (*bitlen < 36 * 8)
		goto fail;

	/* bytes $9A-$A4: Encoder short VersionString */

	magic   = mad_bit_read(ptr, 4 * 8);
	version = (const char *) mad_bit_nextbyte(ptr);

	mad_bit_skip(ptr, 5 * 8);

	/* byte $A5: Info Tag revision + VBR method */

	lame->revision = (unsigned char) mad_bit_read(ptr, 4);
	if (lame->revision == 15)
		goto fail;

	lame->vbr_method = (enum tag_lame_vbr) mad_bit_read(ptr, 4);

	/* byte $A6: Lowpass filter value (Hz) */

	lame->lowpass_filter = (unsigned short) ( mad_bit_read(ptr, 8) * 100);

	/* bytes $A7-$AA: 32 bit "Peak signal amplitude" */

	lame->peak = mad_bit_read(ptr, 32) << 5;

	/* bytes $AB-$AC: 16 bit "Radio Replay Gain" */

	rgain_parse(&lame->replay_gain[0], ptr);

	/* bytes $AD-$AE: 16 bit "Audiophile Replay Gain" */

	rgain_parse(&lame->replay_gain[1], ptr);

	//LAME 3.95.1 reference level changed from 83dB to 89dB and foobar seems to use 89dB
	if (magic == LAME_MAGIC) {
		char str[6];
		unsigned major = 0, minor = 0, patch = 0;
		int i;

		memcpy(str, version, 5);
		str[5] = 0;

		sscanf(str, "%u.%u.%u", &major, &minor, &patch);

		if (major == 3 && minor < 95) {
			for (i = 0; i < 2; ++i) {
				if (RGAIN_SET(&lame->replay_gain[i]))
					lame->replay_gain[i].adjustment += 6;  // 6.0 dB 
			}
		}
	}

	/* byte $AF: Encoding flags + ATH Type */

	lame->flags    = (unsigned char) mad_bit_read(ptr, 4);
	lame->ath_type = (unsigned char) mad_bit_read(ptr, 4);

	/* byte $B0: if ABR {specified bitrate} else {minimal bitrate} */

	lame->bitrate = (unsigned char) mad_bit_read(ptr, 8);

	/* bytes $B1-$B3: Encoder delays */

	lame->start_delay = mad_bit_read(ptr, 12);
	lame->end_padding = mad_bit_read(ptr, 12);

	/* byte $B4: Misc */

	lame->source_samplerate = (enum tag_lame_source) mad_bit_read(ptr, 2);

	if (mad_bit_read(ptr, 1))
		lame->flags |= TAG_LAME_UNWISE;

	lame->stereo_mode   = (enum tag_lame_mode) mad_bit_read(ptr, 3);
	lame->noise_shaping = (unsigned char) mad_bit_read(ptr, 2);

	/* byte $B5: MP3 Gain */

	lame->gain = (signed char) mad_bit_read(ptr, 8);

	/* bytes $B6-B7: Preset and surround info */

	mad_bit_skip(ptr, 2);

	lame->surround = (enum tag_lame_surround) mad_bit_read(ptr,  3);
	lame->preset   = (enum tag_lame_preset) mad_bit_read(ptr, 11);

	/* bytes $B8-$BB: MusicLength */

	lame->music_length = mad_bit_read(ptr, 32);

	/* bytes $BC-$BD: MusicCRC */

	lame->music_crc = (unsigned short) mad_bit_read(ptr, 16);

	/* bytes $BE-$BF: CRC-16 of Info Tag */

	if (mad_bit_read(ptr, 16) != crc)
		goto fail;

	*bitlen -= 36 * 8;

	return 0;

fail:
	*ptr = save;
	return -1;
}


/*
 * NAME:	rgain->parse()
 * DESCRIPTION:	parse a 16-bit Replay Gain field
 */
void rgain_parse(struct rgain *rgain, struct mad_bitptr *ptr)
{
  int negative;

  rgain->name       = (enum rgain_name) mad_bit_read(ptr, 3);
  rgain->originator = (enum rgain_originator) mad_bit_read(ptr, 3);

  negative          = mad_bit_read(ptr, 1);
  rgain->adjustment = (short) mad_bit_read(ptr, 9);

  if (negative)
    rgain->adjustment = (short) -rgain->adjustment;
}





unsigned int WMp3Decoder::count_mp3_frames(unsigned char *buff, unsigned int size, unsigned int *vbr, unsigned int *bitrate)
{
	struct mad_stream stream;
	struct mad_header header;

	mad_stream_init(&stream);
	mad_header_init(&header);

	mad_stream_buffer(&stream, buff, size);

	unsigned char *tmp = 0;
	unsigned int count = 0;
	unsigned int br;
	unsigned int br_sum = 0;
	
	*vbr = 0;
	*bitrate = 0;
	while(1)
	{
		if(mad_header_decode(&header,&stream))
		{
			if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
				continue;
			
			if(stream.error == MAD_ERROR_BUFLEN)
			{
				// fix MAD_BUFFER_GUARD problem 
				if(tmp)
				{
					// we already use MAD_BUFFER_GUARD fix, this is end of stream
					free(tmp);
					tmp = 0;
					break;
				}

				// fix MAD_BUFFER_GUARD problem
				unsigned int len = 0;
				
				len = buff + size - stream.this_frame;
				if(len == 0)
					break;
					
				// allocate memory
				tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
				if(tmp == 0)
					break;
				

				// copy last frame into buffer
				memcpy(tmp, stream.this_frame, len);
				//add MAD_BUFFER_GUARD zero bytes to end
				memset(tmp + len, 0, MAD_BUFFER_GUARD);
				// add SYNC bits
				tmp[len] = 0xFF;
				tmp[len + 1] = 0xE0;
				// set stream pointer to tmp buffer
				mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
				// try to decode this last frame
				continue;
			}
		}

		if(count == 0)
			br = header.bitrate;

		if(br != header.bitrate)
			*vbr = 1;

		br_sum += header.bitrate / 1000;

		count++;
	}

	if(tmp)
		free(tmp);

	mad_stream_finish(&stream);
	mad_header_finish(&header);

	if(count)
		*bitrate = br_sum / count * 1000;

	return count;
}



unsigned int WMp3Decoder::get_frame_pointers(unsigned char *buff, unsigned int size, unsigned int *vbr, unsigned int *bitrate,
		unsigned char*** frame_ptr, unsigned int initial_num)
{
	struct mad_stream stream;
	struct mad_header header;

	mad_stream_init(&stream);
	mad_header_init(&header);

	mad_stream_buffer(&stream, buff, size);

	unsigned char *tmp = 0;
	unsigned int count = 0;
	unsigned int br;
	unsigned int br_sum = 0;

	unsigned char **ptr = 0;
	unsigned int alloc_num = 0;

	// preallocate
	if(initial_num)
	{
		ptr = reallocate_ptr(0, 0, initial_num);
		if(ptr == 0)
			return 0;
	}

	alloc_num = initial_num;

	
	*vbr = 0;
	*bitrate = 0;
	while(1)
	{
		if(mad_header_decode(&header,&stream))
		{
			if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
				continue;
			
			if(stream.error == MAD_ERROR_BUFLEN)
			{
				// fix MAD_BUFFER_GUARD problem 
				if(tmp)
				{
					// we already use MAD_BUFFER_GUARD fix, this is end of stream
					free(tmp);
					tmp = 0;
					break;
				}

				// fix MAD_BUFFER_GUARD problem
				unsigned int len = 0;
				
				len = buff + size - stream.this_frame;
				if(len == 0)
					break;
					
				// allocate memory
				tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
				if(tmp == 0)
					break;
				

				// copy last frame into buffer
				memcpy(tmp, stream.this_frame, len);
				//add MAD_BUFFER_GUARD zero bytes to end
				memset(tmp + len, 0, MAD_BUFFER_GUARD);
				// add SYNC bits
				tmp[len] = 0xFF;
				tmp[len + 1] = 0xE0;
				// set stream pointer to tmp buffer
				mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
				// try to decode this last frame
				continue;
			}
		}

		if(count == 0)
			br = header.bitrate;

		if(br != header.bitrate)
			*vbr = 1;

		br_sum += header.bitrate / 1000;

		// check if we need to reallocate
		if(count + 1 > alloc_num)
		{
			alloc_num += 1024; 	
			ptr = reallocate_ptr(ptr, count, alloc_num);
			if(ptr == 0)
				return 0;	
		}

		ptr[count] = (unsigned char*) stream.this_frame;
		count++;
	}

	if(tmp)
		free(tmp);

	mad_stream_finish(&stream);
	mad_header_finish(&header);

	if(count)
		*bitrate = br_sum / count * 1000;

	*frame_ptr = ptr;

	return count;
}


unsigned char ** reallocate_ptr(unsigned char **src, unsigned int old_size, unsigned int new_size)
{
	if(new_size == 0)	// request to free src
	{
		if(src != NULL)
			free(src);

		return NULL;
	}


	// allocate new memory
	unsigned char **ptr = (unsigned char**) malloc(new_size * sizeof(unsigned char*));
	if(ptr == NULL)
	{
		if(src)
			free(src);

		return NULL;
	}

	// copy old data into new memory
	unsigned int size = old_size < new_size ? old_size : new_size;
	if(src)
	{
		unsigned int i;
		for(i = 0; i < size; i++)
			ptr[i] = src[i];

		free(src);
	}

	return ptr;
}


unsigned int WMp3Decoder::getFrameIndex(unsigned int nSamples)
{
	ASSERT_W(c_pFramePtr);
	// calculate number of frames 
	unsigned int nNeededFrameIndex = nSamples / c_isInfo.nBlockAlign;
	if(nNeededFrameIndex > c_nLastAvailableFrameIndex)
	{
		// we don't have this index in table, scan stream for this index

		struct mad_stream stream;
		struct mad_header header;

		mad_stream_init(&stream);
		mad_header_init(&header);

		// put stream to last known index
		mad_stream_buffer(&stream, c_pFramePtr[c_nLastAvailableFrameIndex], c_pchEnd - c_pFramePtr[c_nLastAvailableFrameIndex]);
		// check this pointer
		if(mad_header_decode(&header,&stream))
		{
			mad_stream_finish(&stream);
			mad_header_finish(&header);
			return FRAME_INDEX_ERR;
		}

		unsigned char *tmp = 0;

		// search until you find index you need
		while(c_nLastAvailableFrameIndex < nNeededFrameIndex)
		{
			// check if we have allocated memory for more frames
			if(c_nLastAvailableFrameIndex + 1 >= c_nAllocatedFrameNumber)
			{
				// we don't have allocated space for more frames, need to reallocate
				// we will increase allocated space for 1024 frames
				unsigned int nAlloc = c_nAllocatedFrameNumber + 1024;
				// try to reallocate 
				unsigned char **ptr = reallocate_ptr(c_pFramePtr, c_nLastAvailableFrameIndex + 1, nAlloc);
				if(ptr == 0)
				{
					mad_stream_finish(&stream);
					mad_header_finish(&header);
					return FRAME_INDEX_ERR;
				}

				c_pFramePtr = ptr;
				c_nAllocatedFrameNumber = nAlloc;
			}
			// reallocation OK, decode header
			if(mad_header_decode(&header,&stream))
			{
				if(MAD_RECOVERABLE(stream.error))  // if recoverable error continue
					continue;

				if(stream.error == MAD_ERROR_BUFLEN)
				{
					// fix MAD_BUFFER_GUARD problem 
					if(tmp)
					{
						// we already use MAD_BUFFER_GUARD fix, this is end of stream
						free(tmp);
						mad_stream_finish(&stream);
						mad_header_finish(&header);
						return FRAME_INDEX_ERR;	
					}

					// fix MAD_BUFFER_GUARD problem
					unsigned int len = 0;
					
					len = c_pchEnd + 1 - stream.this_frame;
					if(len == 0)
					{
						mad_stream_finish(&stream);
						mad_header_finish(&header);
						return FRAME_INDEX_ERR;
					}
						
					// allocate memory
					tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
					if(tmp == 0)
					{
						mad_stream_finish(&stream);
						mad_header_finish(&header);
						return FRAME_INDEX_ERR;
					}
					

					// copy last frame into buffer
					memcpy(tmp, stream.this_frame, len);
					//add MAD_BUFFER_GUARD zero bytes to end
					memset(tmp + len, 0, MAD_BUFFER_GUARD);
					// add SYNC bits
					tmp[len] = 0xFF;
					tmp[len + 1] = 0xE0;
					// set stream pointer to tmp buffer
					mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
					// try to decode this last frame
					continue;
				}
			}

			c_nLastAvailableFrameIndex++;
			c_pFramePtr[c_nLastAvailableFrameIndex] = (unsigned char*) stream.this_frame;
		}
	}

	return nNeededFrameIndex;
}




int WMp3Decoder::_FillBuffer(WMp3Decoder *instance)
{


	struct mad_stream *stream = &instance->c_stream;
	struct mad_frame *frame = &instance->c_frame; 
	struct mad_synth *synth = &instance->c_synth; 

	register mad_fixed_t *left_ch;
	register mad_fixed_t *right_ch;
	register mad_fixed_t sample;
	register mad_fixed_t output;

	AUTODITHER_STRUCT *left_dither = &instance->c_left_dither;
	AUTODITHER_STRUCT *right_dither = &instance->c_right_dither;

	mad_fixed_t random_number;

	unsigned int nchannels;
	unsigned int nsamples;
	unsigned int nsamples_counter;
	unsigned int nFrameCount = 0;
	unsigned int output_size = 0;

	char* output_buf = (char*) instance->c_pchBuffer + instance->c_nBufferSize * 4;
	instance->c_pchBufferPos = instance->c_pchBuffer;

	// reverse mode
	if(instance->c_fReverse)
	{
		ASSERT_W(instance->c_pFramePtr); // we need pointer to each frame

		if(instance->c_nCurrentSeekIndex == 0) // if we are at song start, end of data
			return DECODER_END_OF_DATA;

		// seek backward specified number of frames
		unsigned int nIndex;
		if(instance->c_nCurrentSeekIndex < INPUT_MP3_FRAME_NUMBER)
		{
			// we are near the song beginning, seek to song beginning
			nIndex = 0;
			instance->c_nFrameOverlay = 0;
			if(instance->c_xing.flags != 0)
					instance->c_nFrameOverlay = 1;
					
			instance->c_nCurrentSeekIndex = 0;
		}
		else
		{
			// try frame overlay
			if(instance->c_nCurrentSeekIndex < ( INPUT_MP3_FRAME_NUMBER + FRAME_OVERLAY_NUM))
			{
				// no frame overlay because we are near stream beginning
				nIndex =  instance->c_nCurrentSeekIndex - INPUT_MP3_FRAME_NUMBER;
				instance->c_nCurrentSeekIndex -= INPUT_MP3_FRAME_NUMBER;
				instance->c_nFrameOverlay = 0;
				if(instance->c_nCurrentSeekIndex == 0 && instance->c_xing.flags != 0)
					instance->c_nFrameOverlay = 1;

			}
			else
			{
				nIndex =  instance->c_nCurrentSeekIndex - INPUT_MP3_FRAME_NUMBER - FRAME_OVERLAY_NUM;
				instance->c_nCurrentSeekIndex -= INPUT_MP3_FRAME_NUMBER;
				instance->c_nFrameOverlay = FRAME_OVERLAY_NUM;
			}
		}


		ASSERT_W(nIndex <= instance->c_nLastAvailableFrameIndex);

		// clear all buffers
		mad_frame_finish(frame);
		mad_synth_finish(synth);
		mad_stream_finish(stream);

		memset(&instance->c_left_dither, 0, sizeof(AUTODITHER_STRUCT));
		memset(&instance->c_right_dither, 0, sizeof(AUTODITHER_STRUCT));

		if(instance->c_pchMadBuffGuard)
		{
			free(instance->c_pchMadBuffGuard);
			instance->c_pchMadBuffGuard = 0;
		}


		// initialize decoder
		mad_stream_init(stream);
		mad_frame_init(frame);
		mad_synth_init(synth);
		// set free bitrate value
		instance->c_stream.freerate = instance->c_nFreeBitrate;
		// set decoder to first valid frame
		mad_stream_buffer(stream, instance->c_pchStart, instance->c_nSize);

		// skip bytes, this is seek function

		mad_stream_skip(stream,  instance->c_pFramePtr[nIndex] - instance->c_pchStart);

	}

	// get specified umber of mp3 frames into buffer
	while(nFrameCount < INPUT_MP3_FRAME_NUMBER)
	{
		if(mad_frame_decode(frame, stream) != 0) // if decoding error
		{
	
			if(MAD_RECOVERABLE(stream->error)) // if recoverable error try to decode next frame
					continue;

			// not recoverable error, possible end of data 

			if(instance->c_fManagedStream) // managed stream
			{
				// load data from queue
				unsigned int have = instance->c_Queue->GetSizeSum();

				if(have == 0)
				{
					if(instance->c_fEndOfStream)
					{
						// fix MAD_BUFFER_GUARD problem
						if(stream->error == MAD_ERROR_BUFLEN)
						{
							if(instance->c_pchMadBuffGuard)
							{
								if(output_size == 0)
									return DECODER_END_OF_DATA;

								break;
							}
							
							unsigned int len = instance->c_pchManagedBuffer + instance->c_nManagedBufferLoad - stream->this_frame;
							if(len == 0)
							{
								if(output_size == 0)
									return DECODER_END_OF_DATA;

								break;
							}
					
							// allocate memory
							instance->c_pchMadBuffGuard = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
							if(instance->c_pchMadBuffGuard == 0)
							{
								if(output_size == 0)
									return DECODER_FATAL_ERROR;

								break;
							}

							// copy last frame into buffer
							memcpy(instance->c_pchMadBuffGuard, stream->this_frame, len);
							//add MAD_BUFFER_GUARD zero bytes to end
							memset(instance->c_pchMadBuffGuard + len, 0, MAD_BUFFER_GUARD);
							// add SYNC bits
							instance->c_pchMadBuffGuard[len] = 0xFF;
							instance->c_pchMadBuffGuard[len + 1] = 0xE0;
							// set stream pointer to tmp buffer
							mad_stream_buffer(stream, instance->c_pchMadBuffGuard, len + MAD_BUFFER_GUARD);
							// try to decode this last frame

							continue;
						}

						if(output_size == 0)
							return DECODER_END_OF_DATA;

						break;
					}
					
					if(output_size == 0)
						return DECODER_MANAGED_NEED_MORE_DATA;

					break;
				}	
				
				// first move rest of data from buffer at the beginning of buffer
				
				unsigned int used = stream->this_frame - instance->c_pchManagedBuffer;
				if(used > instance->c_nManagedBufferLoad)
				{
					if(output_size == 0)
						return DECODER_FATAL_ERROR;

					break;
				}

				// calculate size of unused data in buffer
				unsigned int rest = instance->c_nManagedBufferLoad - used;

				if(used)
				{
					// move unused data to beginning of buffer
					MoveMemory(instance->c_pchManagedBuffer, stream->this_frame, rest);
					// set buffer
					mad_stream_buffer(stream, instance->c_pchManagedBuffer, rest);
					instance->c_nManagedBufferLoad = rest;
				}
				
				
				// fill buffer to the end

				// calculate rest of space in buffer to the end
				unsigned int need = instance->c_nManagedBufferMaxSize - rest;
				// get number of bytes we have in queue
				
				
				// check if we have enough data
				if(need > have)
					need = have;
				
				// get data from queue to fill buffer to maximun
				unsigned int nDone = 0;
				unsigned int ret = instance->c_Queue->PullDataFifo(instance->c_pchManagedBuffer + rest, need, (int*) &nDone);
				instance->c_DecoderData.nBufferDone += nDone;
				instance->c_DecoderData.nBufferInQueue = instance->c_Queue->GetCount();
				instance->c_DecoderData.nBytesInQueue = instance->c_Queue->GetSizeSum();
				// set buffer load value
				instance->c_nManagedBufferLoad += ret;
				// set MAD buffer
				mad_stream_buffer(stream, instance->c_pchManagedBuffer, instance->c_nManagedBufferLoad);
				// try again to decode
				continue;

			}
			else // not managed stream
			{
			
				// fix MAD_BUFFER_GUARD problem
				if(stream->error == MAD_ERROR_BUFLEN)
				{
					if(instance->c_pchMadBuffGuard)
					{
						if(output_size == 0)
							return DECODER_END_OF_DATA;

						break;
					}
					
					unsigned int len = instance->c_pchStart + instance->c_nSize - stream->this_frame;
					if(len == 0)
					{
						if(output_size == 0)
							return DECODER_END_OF_DATA;

						break;
					}
					
					// allocate memory
					instance->c_pchMadBuffGuard = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
					if(instance->c_pchMadBuffGuard == 0)
					{
						if(output_size == 0)
							return DECODER_FATAL_ERROR;

						break;
					}
				

					// copy last frame into buffer
					memcpy(instance->c_pchMadBuffGuard, stream->this_frame, len);
					//add MAD_BUFFER_GUARD zero bytes to end
					memset(instance->c_pchMadBuffGuard + len, 0, MAD_BUFFER_GUARD);
					// add SYNC bits
					instance->c_pchMadBuffGuard[len] = 0xFF;
					instance->c_pchMadBuffGuard[len + 1] = 0xE0;
					// set stream pointer to tmp buffer
					mad_stream_buffer(stream, instance->c_pchMadBuffGuard, len + MAD_BUFFER_GUARD);
					// try to decode this last frame
					continue;
				}
				
				if(output_size == 0)
					return DECODER_FATAL_ERROR;

				break;
			}
		} 

		// we have valid mp3 frame


		// synth frame
		mad_synth_frame(synth, frame);

		instance->c_nCurrentBitrate = frame->header.bitrate;

		// check if we need to overlay this frame
		if(instance->c_nFrameOverlay != 0)
		{
			instance->c_nFrameOverlay--;
			continue;
		}

		// dither, resample ...
		nchannels = synth->pcm.channels;
		nsamples_counter  = synth->pcm.length;
		left_ch   = synth->pcm.samples[0];
		right_ch  = synth->pcm.samples[1];

		// skip some samples if we need ( accurate seek )
		if(instance->c_nSkipSamlesNum != 0)
		{
			if(nsamples_counter > instance->c_nSkipSamlesNum) // skip sample inside this frame
			{
				nsamples_counter -= instance->c_nSkipSamlesNum;

				left_ch += instance->c_nSkipSamlesNum;
				right_ch += instance->c_nSkipSamlesNum;
				instance->c_nSkipSamlesNum = 0;
			}
			else
			{
				// skip whole frame
				instance->c_nSkipSamlesNum -= nsamples_counter;
				continue;
			}

		}

		nsamples = nsamples_counter;

		if(nchannels == 2)
		{ // stereo
			while (nsamples_counter--)
			{
				// left channel
				sample = *left_ch++;
				// noise shape 	
				sample += left_dither->error[0] - left_dither->error[1] + left_dither->error[2];
				left_dither->error[2] = left_dither->error[1];
				left_dither->error[1] = left_dither->error[0] / 2;
				// bias 
				output = sample + 4096;	
				// dither 
				random_number  = ((unsigned long) left_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
				output += (random_number & 8191) - (left_dither->random & 8191);

				// clip
				if (output >= MAD_F_ONE)
					output = MAD_F_ONE - 1;
				else if (output < -MAD_F_ONE)
					output = -MAD_F_ONE;

				// quantize 
				output &= ~8191;
				// error feedback 
				left_dither->error[0] = sample - output;	
				output_buf[0] = (char) (output >> 13);
				output_buf[1] = (char) (output >> 21);
				// right channel
				sample = *right_ch++;	
				// noise shape 
				sample += right_dither->error[0] - right_dither->error[1] + right_dither->error[2];
				right_dither->error[2] = right_dither->error[1];
				right_dither->error[1] = right_dither->error[0] / 2;
				 // bias 
				output = sample + 4096;
				// dither 
				random_number  = ((unsigned long) right_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
				output += (random_number & 8191) - (right_dither->random & 8191);

				if (output >= MAD_F_ONE)
					output = MAD_F_ONE - 1;
				else if (output < -MAD_F_ONE)
					output = -MAD_F_ONE;

				// quantize 
				output &= ~8191;
				// error feedback 
				right_dither->error[0] = sample - output;
				output_buf[2] = (char) (output >> 13);
				output_buf[3] = (char) (output >> 21);
				output_buf += 4;
					
					
			} // while (nsamples--)

			output_size += nsamples * 4;
		}	
		else
		{ // mono
			while (nsamples_counter--)
			{
			// left channel
				sample = *left_ch++;
				// noise shape 	
				sample += left_dither->error[0] - left_dither->error[1] + left_dither->error[2];
				left_dither->error[2] = left_dither->error[1];
				left_dither->error[1] = left_dither->error[0] / 2;
				// bias 
				output = sample + 4096;
				// dither 
				random_number  = ((unsigned long) left_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
				output += (random_number & 8191) - (left_dither->random & 8191);

				// clip
				if (output >= MAD_F_ONE)
					output = MAD_F_ONE - 1;
				else if (output < -MAD_F_ONE)
					output = -MAD_F_ONE;
	
				// quantize 
				output &= ~8191;
				// error feedback 
				left_dither->error[0] = sample - output;
				output_buf[0] = (char) (output >> 13);
				output_buf[1] = (char) (output >> 21);
				output_buf[2] = (char) (output >> 13);
				output_buf[3] = (char) (output >> 21);
				output_buf += 4;
			}
			output_size += nsamples * 4;
		}
		nFrameCount++;
	}

	instance->c_nBufferSize += (output_size / 4);

	if(instance->c_fReverse)
		PCM16StereoReverse(instance->c_pchBuffer, output_size / 4 , instance->c_pchBuffer);


	return DECODER_OK;
}




void WMp3Decoder::err(unsigned int error_code)
{
	if(error_code > DECODER_UNKNOWN_ERROR)
		error_code = DECODER_UNKNOWN_ERROR;
			
	c_err_msg.errorW = g_mp3_error_strW[error_code];
}



int WMp3Decoder::OpenStream(WQueue *pQueue, int fDynamic, int param1, int param2)
{
	err(DECODER_NO_ERROR);

	c_Queue = pQueue;

	unsigned int nSize = pQueue->GetSizeSum();
	if(nSize == 0)
	{
		err(DECODER_NOT_VALID_MP3_STREAM);
		return 0;
	}


	unsigned char *ptr;
	unsigned int size; 
	if(pQueue->QueryFirstPointer((void**) &ptr, &size) == 0)
	{
		err(DECODER_NOT_VALID_MP3_STREAM);
		return 0;
	}

	c_fManagedStream = 0;

	if(param1 == 0)
	{

		c_pchStreamStart = ptr;
		c_nStreamSize = size;
		c_pchStreamEnd = c_pchStreamStart + c_nStreamSize - 1;


		if(fDynamic)
		{
			c_fManagedStream = 1;


			c_nManagedBufferMaxSize = MAX_FRAME_SIZE * INPUT_MP3_FRAME_NUMBER;
			c_pchManagedBuffer = (unsigned char*) malloc(c_nManagedBufferMaxSize);
			if(c_pchManagedBuffer == NULL)
			{
				c_nManagedBufferMaxSize = 0;
				err(DECODER_MEMORY_ALLOCATION_FAIL);
				return 0;

			}

		}


		
		if(_OpenFile(0) == 0)
		{
			Close();
			return 0;
		}

		return 1;
	}
	else
	{
		c_pchStreamStart = ptr;
		c_nStreamSize = size;
		c_pchStreamEnd = c_pchStreamStart + c_nStreamSize - 1;
		return 1;

	}

	return 0;

}