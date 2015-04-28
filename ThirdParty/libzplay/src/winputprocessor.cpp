/*
 *  libzplay - windows ( WIN32 ) multimedia library for playing mp3, mp2, ogg, wav, flac and raw PCM streams
 *  WAVE input processor
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

#include <string.h>
#include <stdio.h>
#include "debug.h"
#include "winputProcessor.h"




WInputProcessor::WInputProcessor()
{

}


WInputProcessor::~WInputProcessor()
{

}



int WInputProcessor::PushData(char *pchData, unsigned int nDataSize, unsigned int nUserData)
{
	#ifdef _PRINT_DEBUG_INFO
		printf("WInputProcessor::PushData\n");
	#endif


	// send data to next processor
	if(c_next)
	{	
		#ifdef AUDIO_PROCESSOR_RETURN_ERROR_STR
			if(c_pchReturnError == 0)
				c_next->c_pchReturnError = c_pchErrorMessageStr;
			else
				c_next->c_pchReturnError = c_pchReturnError;
		#endif
				
		return c_next->PushData(pchData, nDataSize, nUserData);
	}
/*
	// send data to output function
	if(c_output)
	{			
		if(c_output(pchData, nDataSize, nUserData) == 0)
		{
			strcpy(c_pchErrorMessageStr, "WInputProcessor::PushData->Output function return 0.");
			if(c_pchReturnError)
					strcpy(c_pchReturnError, c_pchErrorMessageStr);
			return 0;
		}

		return 1;
	}


	if(queue.PushLast(pchData, nDataSize) == 0)
	{
		strcpy(c_pchErrorMessageStr, "WInputProcessor::PushData->Can't add data to queue.");
		if(c_pchReturnError)
			strcpy(c_pchReturnError, c_pchErrorMessageStr);
		return 0;
	}
*/
	return 1;
}



int WInputProcessor::Configure(unsigned int fBroadcast, unsigned int nSampleRate, unsigned int nChannel, unsigned int nBitPerSample)
{
// send this data to next processor


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



int WInputProcessor::Enable(int fBroadcast, int fEnable)
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


int WInputProcessor::Flush(int fBroadcast)
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


int WInputProcessor::Clear(int fBroadcast)
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

int WInputProcessor::PushSamples(PROCESSOR_AUDIO_DATA *data)
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


	ASSERT_W(0);
	return 0;
}