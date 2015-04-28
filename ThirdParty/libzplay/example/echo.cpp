/*
* libZPlay example
*
* Play test.mp3 to sound card output with multiple echo.
* There are different settings for left and right channel
* to produce more interesting effect.
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

		TEchoEffect effect[2];
	// 2 second delay
	effect[0].nLeftDelay = 1800; // 1800 ms delay
	effect[0].nLeftEchoVolume = 20; // use 20 % volume of delayed samples
	effect[0].nLeftSrcVolume = 80; // use 80 % volume of original samples
	effect[0].nRightDelay = 2000;
	effect[0].nRightEchoVolume = 20;
	effect[0].nRightSrcVolume = 80;
	// 300 ms delay
	effect[1].nLeftDelay = 180; // 180 ms delay
	effect[1].nLeftEchoVolume = 20; // use 20 % volume of delayed samples
	effect[1].nLeftSrcVolume = 0; // don't need original samples because we already involve this in previous effect
	effect[1].nRightDelay = 200;
	effect[1].nRightEchoVolume = 20;
	effect[1].nRightSrcVolume = 0;
	player->SetEchoParam(effect, 2); // set new echo effect
	player->EnableEcho(1); // enable echo effect

		int nNumberOfEffects = player->GetEchoParam(NULL, 0);
		if(nNumberOfEffects)
		{
			TEchoEffect *effects = new TEchoEffect[nNumberOfEffects];	
			nNumberOfEffects = player->GetEchoParam(effects, nNumberOfEffects);
			for(int i = 0; i < nNumberOfEffects; i++)
			{
				printf("Effect: %i  Delay: %i  OriginalVol: %i   EchoVol %i\n",
						i,
						effects[i].nLeftDelay, 
						effects[i].nLeftSrcVolume,
						effects[i].nLeftEchoVolume);
			}
			delete [] effects;
		}



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
