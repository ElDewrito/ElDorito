/*
* libZPlay example
*
* Play test.mp3 to sound card output,display FFT graph,
* cut all from 5000 Hz to 10000 Hz using equalizer
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


	// we will cut all from 5000 Hz to 10000 Hz,
	// so we need to create custom equalizer bands because
	// we need exact frequency bounds.
	
	// band0: 0 - 5000 Hz
	// band1: 5000 - 10000 Hz
	// band2: 10000 - samplerate/2
	
	// define 2 points to devide frequancy range into 3 bands

	// band:    0      1       2
	// range: 0 - 5000 - 10000 - samplerate/2

	int nFreqPoint[2] = {5000, 10000};
	player->SetEqualizerPoints(nFreqPoint, 2);
	
	// now we need to lower gain of band1 for 100 decibels
	// band gain is expressed in millidecibels, thousandth of decibel
	player->SetEqualizerBandGain(1, -100000); 

	// enable equalizer
	player->EnableEqualizer(1);



	// set graph type to AREA, left channel on top
	player->SetFFTGraphParam(gpGraphType, gtAreaLeftOnTop);
	// set linear scale
	player->SetFFTGraphParam(gpHorizontalScale, gsLinear);

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

		// draw FFT graph on desktop window
		player->DrawFFTGraphOnHWND(0, 0, 0, 300, 200);

		Sleep(50); // wait 300 ms
	}

	// destroy class instance
	player->Release();

	// we are drawing directly on desktop window, so we need
	// to remove our graph by redrawind this area on screen
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = 300;
	rc.bottom = 200;
	InvalidateRect(0, &rc, TRUE);

	return 0;
}
