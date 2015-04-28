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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

#include "wdecoder.h"
#include "wmp3decoder.h"

#include "wwavein.h"


WAVEIN_ID g_wavein_id[WAVE_IN_NUM] = {
	{"line", MIXERLINE_COMPONENTTYPE_SRC_LINE, "Line In", "Line", "Line Input"},
	{"line-in", MIXERLINE_COMPONENTTYPE_SRC_LINE, "Line In", "Line", "Line Input"},
	{"mic", MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, "Microphone", "Mic", "Ext Mic"},
	{"microphone", MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, "Microphone", "Mic", "Ext Mic"},
	{"analog", MIXERLINE_COMPONENTTYPE_SRC_ANALOG, "Analog", "Analog Src", "Analog In"},
	{"aux", MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY, "Auxiliary", "Aux", "Aux In"},
	{"cd", MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, "CD Player", "CD Audio", "CD"},
	{"digital", MIXERLINE_COMPONENTTYPE_SRC_DIGITAL, "Digital", "Digital In", "DigitalIn"},
	{"pcspeaker", MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER, "Speaker", "PC Speaker", "PC Speaker"},
	{"synthesizer", MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, "Midi", "Midi Out", "SW Synth"},
	{"midi", MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, "Midi", "Midi Out", "SW Synth"},
	{"phone", MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE, "Telephony", "Telephone", "Phone"},
	{"waveout", MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, "Stereo Out", "Wave Out Mix", "Wave Out"},
	{"undefined", MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED, "", "", ""},
	{"user", 0xFFFFFFFF, "", "", ""},
};


WAudioDecoder *  CreateMp3Decoder()
{
	return (WAudioDecoder*)  new WMp3Decoder();
}






// ==========================================================================================


// convert PCM 16 bit mono to PCM 16 bit stereo
void PCM16MonoToPCM16Stereo(short *inSamples, unsigned int inSampleNum, short *outSamples)
//
//	PARAMETERS:
//		inSamples
//			Buffer vith input samples. PCM data, 16 bit per sample, mono.
//
//		inSampleNum
//			Number of input mono samples.
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
{

	unsigned int i;
	unsigned int index;
	for(i = 1; i <= inSampleNum; i++)
	{
		index = inSampleNum - i;
		outSamples[2 * index] = inSamples[index];	
		outSamples[2 * index + 1] = inSamples[index];
	}

}


// convert PCM 16 bit stereo to PCM 16 bit stereo
void PCM8StereoUnsignedToPCM16Stereo(unsigned char *inSamples, unsigned int inSampleNum, short *outSamples)
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 8 bit per sample, stereo.
//
//		inSampleNum
//			Number of input stereo samples.
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
{
	inSampleNum *= 2;

	unsigned int i;
	unsigned int index;

	for(i = 1; i <= inSampleNum; i++)
	{
		index = inSampleNum - i;
		outSamples[index] = ((short)inSamples[index] - 128) << 8;
	}

}



// convert PCM 16 bit stereo signed to PCM 16 bit stereo
void PCM8StereoSignedToPCM16Stereo(char *inSamples, unsigned int inSampleNum, short *outSamples)
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
{

	unsigned int i;
	unsigned int index;

	inSampleNum *= 2;
	for(i = 1; i <= inSampleNum ; i++)
	{
		index = inSampleNum - i; 
		outSamples[index] = (short) (((short) inSamples[index]) << 8) ;
	}

}



// convert PCM 8 bit mono unsigned to PCM 16 bit stereo
void PCM8MonoUnsignedToPCM16Stereo(unsigned char *inSamples, unsigned int inSampleNum, short *outSamples)
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
{
	unsigned int i;
	unsigned int index;
	short sample;

	for(i = 1; i <= inSampleNum ; i++)
	{
		index = inSampleNum - i; 
		sample = ((short)inSamples[index] - 128) << 8;
		outSamples[2 * index    ] = sample;
		outSamples[2 * index + 1] = sample;
	}

}


// convert PCM 8 bit mono signed to PCM 16 bit stereo
void PCM8MonoSignedToPCM16Stereo(char *inSamples, unsigned int inSampleNum, short *outSamples)
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
{
	unsigned int i;
	unsigned int index;


	for(i = 1; i <= inSampleNum ; i++)
	{
		index = inSampleNum - i; 
		outSamples[2 * index    ] = (short) (((short) inSamples[index]) << 8) ;
		outSamples[2 * index + 1] = (short) (((short) inSamples[index]) << 8) ;
	}

}


// convert PCM 32 bit stereo to PCM 16 bit stereo
void PCM32StereoToPCM16Stereo(int *inSamples, unsigned int inSampleNum, short *outSamples)
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 32 bit per sample, stereo.
//
//		inSampleNum
//			Number of input  samples.
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
{

	unsigned int i;
	for(i = 0; i < inSampleNum * 2; i++)
		outSamples[i] = inSamples[i] >> 16;

}



// convert PCM 32 bit mono to PCM 16 bit stereo
void PCM32MonoToPCM16Stereo(int *inSamples, unsigned int inSampleNum, short *outSamples)
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
{
	unsigned int i;
	short sample;
	for(i = 0; i < inSampleNum; i++)
	{
		sample = inSamples[i] >> 16;
		outSamples[i * 2] = sample;
		outSamples[i * 2 + 1] = sample;
	}

}


// ===================================================================================================

// reverse PCM 16 bit stereo
void PCM16StereoReverse(int *inSamples, unsigned int inSampleNum, int *outSamples)
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 16 bit per sample, stereo.
//
//		inSampleNum
//			Number of input stereo samples.
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
{

	if(inSampleNum < 2)
		return;

	int sample;
	unsigned int heigh = inSampleNum - 1;
	unsigned int low = 0;
	while(low < heigh)
	{
		sample = inSamples[low];
		outSamples[low] = inSamples[heigh];
		outSamples[heigh] = sample;	
		low++;
		heigh--;		
	}
}


// =================================================================================
// convert PCM 16 bit stereo big-endian to PCM 16 bit stereo little-endian
void PCM16StereoBEToPCM16StereoLE(short *inSamples, unsigned int inSampleNum, short *outSamples)
//
//	PARAMETERS:
//		inSamples
//			Buffer with input samples. PCM data, 16 bit per sample, stereo.
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
{
	unsigned int i;
	short sample;
	for(i = 0; i < inSampleNum * 2; i++)
	{
		sample = inSamples[i];
		outSamples[i] = (short) ( ((sample >> 8) & 0xFF) + ((sample & 0xFF) << 8) );
	}

}


int UTF16ToANSI(wchar_t *src, int nCharacterNumber, char *dest, int nDestSizeInBytes)
{

	if((char*) src != dest)
	{
		int ret = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, src, nCharacterNumber, dest, nDestSizeInBytes, NULL, NULL);
		if(nDestSizeInBytes && ret)
		{
			// null terminate string
			if(dest[ret - 1] != 0) // string isn't null terminated
			{
				if(nDestSizeInBytes > ret)
					dest[ret] = 0;
				else
					dest[ret - 1] = 0;

			}
		}

		return ret;

	}

	// src and dest are the same, use internal buffer

	if(nDestSizeInBytes == 0)
		return WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, src, nCharacterNumber, dest, nDestSizeInBytes, NULL, NULL);
	

	char *buff = (char*) malloc(nDestSizeInBytes);
	if(buff == 0)
		return 0;

	int ret = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, src, nCharacterNumber, buff, nDestSizeInBytes, NULL, NULL);
	if(ret == 0)
	{
		free(buff);
		return 0;
	}

	// copy data from internal buffer into dest buffer
	memcpy(dest, buff, ret);
	// null terminate dest buffer
	// null terminate string
	if(dest[ret - 1] != 0) // string isn't null terminated
	{
		if(nDestSizeInBytes > ret)
			dest[ret] = 0;
		else
			dest[ret - 1] = 0;

	}

	free(buff);
	
	return ret;
}


int UTF8ToANSI(char *src, int nBytes, char *dest, int nDestSizeInBytes)
{

	// convert UTF-8 to UTF-16
	int size = MultiByteToWideChar(CP_UTF8, 0, src, nBytes, NULL, 0);
	if(size == 0)
		return 0;

	wchar_t *wide = (wchar_t*) malloc(size * 2);
	if(wide == 0)
		return 0;

	if(MultiByteToWideChar(CP_UTF8, 0, src, nBytes, (LPWSTR) wide, size) == 0)
	{
		free(wide);
		return 0;
	}

	if((char*) src != dest)
	{
		// convert UTF-16 to ANSI
				
		int ret = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) wide, size, dest, nDestSizeInBytes, NULL, NULL);				
		if(nDestSizeInBytes && ret)
		{
			// null terminate string
			if(dest[ret - 1] != 0) // string isn't null terminated
			{
				if(nDestSizeInBytes > ret)
					dest[ret] = 0;
				else
					dest[ret - 1] = 0;

			}
		}

		free(wide);
		return ret;

	}

	// src and dest are the same, use internal buffer

	if(nDestSizeInBytes == 0)
	{
		int ret = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, wide, size, dest, nDestSizeInBytes, NULL, NULL);
		free(wide);
		return ret;
	}
	

	char *buff = (char*) malloc(nDestSizeInBytes);
	if(buff == 0)
	{
		free(wide);
		return 0;
	}

	int ret = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, wide, size, buff, nDestSizeInBytes, NULL, NULL);
	free(wide);
	if(ret == 0)
	{
		free(buff);
		return 0;
	}

	// copy data from internal buffer into dest buffer
	memcpy(dest, buff, ret);
	// null terminate string
	if(dest[ret - 1] != 0) // string isn't null terminated
	{
		if(nDestSizeInBytes > ret)
			dest[ret] = 0;
		else
			dest[ret - 1] = 0;

	}

	free(buff);
	
	return ret;
}



int ANSIToUTF16(char *src, int nCharacterNumber, wchar_t *dest, int nDestSizeInBytes)
{
	if(src != (char*) dest)
	{
		int ret = MultiByteToWideChar(CP_ACP, 0, src, nCharacterNumber, dest, nDestSizeInBytes);
		if(nDestSizeInBytes && ret)
		{
		// null terminate string
			if(dest[ret - 1] != 0) // string isn't null terminated
			{
				if(nDestSizeInBytes > ret)
					dest[ret] = 0;
				else
					dest[ret - 1] = 0;

			}
		}
			
		return ret;

	}

	if(nDestSizeInBytes == 0)
		return MultiByteToWideChar(CP_ACP, 0, src, nCharacterNumber, dest, nDestSizeInBytes);
		
	// allocate internal memory
	wchar_t *mem = (wchar_t*) malloc(nDestSizeInBytes);
	if(mem == 0)
		return 0;

	// convert ANSI to UNICODE using internal buffer
	int ret = MultiByteToWideChar(CP_ACP, 0, src, nCharacterNumber, mem, nDestSizeInBytes);
	if(ret)
	{
		// copy data from internal buffer to dest buffer
		memcpy(dest, mem, ret * 2);
		// null terminate dest buffer
		if(dest[ret - 1] != 0) // string isn't null terminated
		{
			if(nDestSizeInBytes > ret)
				dest[ret] = 0;
			else
				dest[ret - 1] = 0;

		}
	}

	return ret;
}


int UTF8ToUTF16(char *src, int nCharacterNumber, wchar_t *dest, int nDestSizeInBytes)
{
	if(src != (char*) dest)
	{
		int ret = MultiByteToWideChar(CP_UTF8, 0, src, nCharacterNumber, dest, nDestSizeInBytes);
		if(nDestSizeInBytes && ret)
		{
			// null terminate string
			if(dest[ret - 1] != 0) // string isn't null terminated
			{
				if(nDestSizeInBytes > ret)
					dest[ret] = 0;
				else
					dest[ret - 1] = 0;

			}
		}

		return ret;

	}

	if(nDestSizeInBytes == 0)
		return MultiByteToWideChar(CP_UTF8, 0, src, nCharacterNumber, dest, nDestSizeInBytes);
		
	// allocate internal memory
	wchar_t *mem = (wchar_t*) malloc(nDestSizeInBytes);
	if(mem == 0)
		return 0;

	// convert ANSI to UNICODE using internal buffer
	int ret = MultiByteToWideChar(CP_UTF8, 0, src, nCharacterNumber, mem, nDestSizeInBytes);
	if(ret)
	{
		// copy data from internal buffer to dest buffer
		memcpy(dest, mem, ret * 2);
		// null terminate string
		if(dest[ret - 1] != 0) // string isn't null terminated
		{
			if(nDestSizeInBytes > ret)
				dest[ret] = 0;
			else
				dest[ret - 1] = 0;

		}
	}

	return ret;
}


void RemoveEndingSpaces(char* buff)
{
	int size = strlen(buff);
	for(int i = size - 1; i >= 0; i--)
	{
		if(buff[i] == ' ')
			buff[i] = 0;
		else
			return;
	}
}

void RemoveEndingSpacesEx(char* buff, int size)
{
	for(int i = size - 1; i >= 0; i--)
	{
		if(buff[i] == ' ')
			buff[i] = 0;
		else
			return;
	}
}

