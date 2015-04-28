/*
* libZPlay example
*
* Reverse playing
*
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "libzplay.h"

using namespace libZPlay;

int main(int argc, char **argv)
{
	printf("Reverse playing test.mp3\n\nPress r to toggle normal/reverse playing\n\nPress q to end\n\n");

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

	// set reverse mode
	player->ReverseMode(1);

	// seek to end of song
	TStreamTime time;
	time.samples = 0;
	player->Seek(tfSamples, &time, smFromEnd);

	// start playing
	player->Play();

	// display position and wait for song end
	while(1)
	{
		// get stream status to check if song is still playing
		TStreamStatus status;
		player->GetStatus(&status);	
		if(status.fPlay == 0)
			break; // exit checking loop

		// check key press
		if(kbhit())
		{
           	int a = getch();

			if(a == 'r' || a == 'R') // enter reverse mode
			{
				if(status.fReverse) // we are in reverse mode
					player->ReverseMode(0); // exit reverse mode
				else
					player->ReverseMode(1); // enter reverse mode		

			}


			if(a == 'q' || a == 'Q')
				break; // end program if Q key is pressed
		}



		// get current position
		TStreamTime pos;
		player->GetPosition(&pos);
		// display position
		printf("Pos: %02u:%02u:%02u:%03u   Reverse: %u\r",
			pos.hms.hour, pos.hms.minute, pos.hms.second, pos.hms.millisecond,
			status.fReverse);

		Sleep(300); // wait 300 ms
	}

	// destroy class instance
	player->Release();

	return 0;
}
