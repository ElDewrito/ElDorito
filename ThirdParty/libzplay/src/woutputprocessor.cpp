/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  WAVE output processor
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
#include "debug.h"
#include "woutputprocessor.h"




WOutputProcessor::WOutputProcessor()
{

}


WOutputProcessor::~WOutputProcessor()
{

}



int WOutputProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
// send this data to next processor


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



int WOutputProcessor::Enable(int fBroadcast, int fEnable)
{
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


int WOutputProcessor::Flush(int fBroadcast)
{
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


int WOutputProcessor::Clear(int fBroadcast)
{
	c_queue.Clear();

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

unsigned int WOutputProcessor::PullSamples(PROCESSOR_AUDIO_DATA *data)
{

	return c_queue.PullSamples(data);
}


int WOutputProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
{

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


	if(c_queue.PushSamples(data) == 0)
	{
		strcpy(c_pchErrorMessageStr, "WOutputProcessor::PushData->Can't add data to queue.");
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;

	}

	return 1;
}