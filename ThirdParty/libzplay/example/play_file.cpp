/*
* libZPlay example
*
* Play test.mp3 to sound card output.
*
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "libzplay.h"

using namespace libZPlay;

int main(int argc, char **argv)
{
	printf("Playing test.mp3\n\nPress q to end\n\n");

	// create class instance using class factory.
	ZPlay *player = CreateZPlay();

	// open file
	int result = player->OpenFile("test.mp3", sfAutodetect);
	if(result == 0)
	{
		// display error message
		printf("Error: %s\n", player->GetError());
		player->Release();
		return 0;
	}

	// get song length
	TStreamInfo info;
	player->GetStreamInfo(&info);

	printf("Length: %02u:%02u:%02u:%03u\n\n", info.Length.hms.hour, 
												info.Length.hms.minute,
												info.Length.hms.second,
												info.Length.hms.millisecond);

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

	return 0;
}
