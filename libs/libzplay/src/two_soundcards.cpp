/*
* libZPlay example
*
* Play 2 songs, each song on different soundcard.
*/


#include <windows.h>
#include <stdio.h>
#include <conio.h>


#include "libzplay.h"

using namespace libZPlay;

int main(int argc, char **argv)
{
	printf("Play 2 songs on different soundcards.\n\nPress q to end\n\n");

	// create class instance using class factory.
	ZPlay *player1 = CreateZPlay();
	ZPlay *player2 = CreateZPlay();


	// get wave out info
	int num = player1->EnumerateWaveOut();
	printf("Number of wave out devices: %i\n", num);
	unsigned int i;
	TWaveOutInfo waveOutInfo;
	for(i = 0; i < num; i++)
	{
	  if(player1->GetWaveOutInfo(i, &waveOutInfo))
		printf("Device index: %u  - ProductName:%s\n", i, waveOutInfo.ProductName);
	}
	
	// configuration on my computer
	// Number of wave out devices: 5
	// Device index: 0  - ProductName:SoundMAX Digital Audio
	// Device index: 1  - ProductName:Modem #0 Line Playback
	// Device index: 2  - ProductName:Modem #0 Handset Playback
	// Device index: 3  - ProductName:WinGroove WaveOut (NT)
	// Device index: 4  - ProductName:YAMAHA DS-XG Audio (WDM)

	// set wave-out device for player1, in my case: SoundMAX Digital Audio
	player1->SetWaveOutDevice(0);

	// set wave-out device for player2, in my case: YAMAHA DS-XG Audio (WDM)
	player2->SetWaveOutDevice(4);

	// open first file
	int result = player1->OpenFile("test.mp3", sfAutodetect);
	if(result == 0)
	{
		// display error message
		printf("Error: %s\n", player1->GetError());
		player1->Release();
		return 0;
	}

	// open second file file
	result = player2->OpenFile("test1.mp3", sfAutodetect);
	if(result == 0)
	{
		// display error message
		printf("Error: %s\n", player2->GetError());
		player2->Release();
		player1->Release();
		return 0;
	}

	// start playing
	player1->Play();
	player2->Play();

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
		TStreamStatus status1;
		TStreamStatus status2;
		player1->GetStatus(&status1);
		player2->GetStatus(&status2);	
		if(status1.fPlay == 0 && status2.fPlay == 0)
			break; // exit checking loop

		// get current position
		TStreamTime pos1;
		TStreamTime pos2;
		player1->GetPosition(&pos1);
		player2->GetPosition(&pos2);
		// display position
		printf("Pos1: %02u:%02u:%02u:%03u  Pos2: %02u:%02u:%02u:%03u\r", 
			pos1.hms.hour, pos1.hms.minute, pos1.hms.second, pos1.hms.millisecond,
			pos2.hms.hour, pos2.hms.minute, pos2.hms.second, pos2.hms.millisecond);

		Sleep(300); // wait 300 ms
	}

	// destroy class instance
	player1->Release();
	player2->Release();

	return 0;
}
