/*
* libZPlay example
*
* Use dynamic stream.
*/


#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "libzplay.h"

using namespace libZPlay;

// callback function prototype, need this to get info when dynamic stream needs more data
int  __stdcall  myCallbackFunc(void* instance,
							void *user_data,
							TCallbackMessage message,
							unsigned int param1,
							unsigned int param2);


unsigned int nOutputDataSize = 0;

int main(int argc, char **argv)
{
	printf("Play dynamic stream.\n\nPress q to end\n\n");

	// create class instance using class factory.
	ZPlay *player = CreateZPlay();

	// detect stream format
	TStreamFormat format = player->GetFileFormat("test.mp3");
	if(format == sfUnknown)
	{
		printf("Unsupported format\n");
		player->Release();
		return 0;
	}

	FILE *in = fopen("test.mp3", "rb"); // open input file
	if(in == NULL)
	{
		printf("Can't open test.mp3.");
		player->Release();
		return 0;
	}

	// set callback mechanism to intercept MsgStreamNeedMoreData message
	// send file handle as user parameter to callback function
	player->SetCallbackFunc(myCallbackFunc, (TCallbackMessage) MsgStreamNeedMoreData, in);


	char buffer[10000]; // buffer for stream data
	// load some starting data into memory buffer,
	// this data must contain at least, one valid mp3 frame
	unsigned int read = fread(buffer, 1, 10000, in);

	// open memory stream - buffered, dynamic stream.
	int result = player->OpenStream(1, 1, buffer, read, format);
	if(result == 0)
	{
		// display error message
		printf("Error: %s\n", player->GetError());
		player->Release();
		fclose(in);
		return 0;
	}

	// start playing
	player->Play();

	// display position and wait for song end
	while(1)
	{
		// check key press
		if(kbhit())
		{
           	int a = getch();
			if(a == 'q' || a == 'Q')
				break; // end program if Q key is pressed
		}

		// get stream status to check if song is still playing
		TStreamStatus status;
		player->GetStatus(&status);	
		if(status.fPlay == 0)
			break; // exit checking loop

		// get current position
		TStreamTime pos;
		player->GetPosition(&pos);
		// display position
		printf("Pos: %02u:%02u:%02u:%03u\r", pos.hms.hour, pos.hms.minute, pos.hms.second, pos.hms.millisecond);

		Sleep(300); // wait 300 ms
	}

	// destroy class instance
	player->Release();

	fclose(in);	// close wave file
	return 0;
}


int  __stdcall  myCallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
	ZPlay *myplayer = (ZPlay*) instance;

	switch(message)
	{
		case MsgStreamNeedMoreData: // stream needs more data
		{
			FILE *in = (FILE*) user_data; // this parameter is set by SetCallbackFunc

			// read next chunk of data from file into memory buffer
			char buffer[10000];
			unsigned int read = fread(buffer, 1, 10000, in);

			// push this memory buffer into stream
			myplayer->PushDataToStream(buffer, read);
		}
		return 0;	
	}

	return 0;
}