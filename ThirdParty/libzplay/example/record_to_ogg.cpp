/*
 *	Record samples from line-in and save to out.mp3
 *
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <olectl.h>
#include <ole2.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <conio.h>


#include "../include/libzplay.h"


using namespace libZPlay;

ZPlay* player;

 
int nRate = 100;
int nPitch = 100;
int nTempo = 100;
int fCenterCut = 0;
int fSideCut = 0;
int fMixChannels = 0;
int lVolume = 100;
int rVolume = 100;


int fPause = 0;
int fEcho = 0;
int fEq = 0;
int fReverse = 0;
TStreamStatus status;
TStreamTime pos;



int main(int argc, char **argv)
{
	// clear screen
	system("CLS");


	// create class instance
	player = CreateZPlay();

	// chek if we have class instance
	if(player == 0)
	{
		printf("Error: Can't create class instance !\nPress key to exit.\n");
		getch();
		return 0;	
	}

	// get library version
	int ver = player->GetVersion();
	// check if we have version 2.00 and above
	if(ver < 200)
	{
		printf("Error: Need library version 2.00 and above !\nPress key to exit.\r\n");
		getch();
		player->Release();
		return 0;
	}


	// display version info
	printf("libZPlay v.%i.%02i\r\n\r\n", ver / 100, ver % 100);


	printf( "x: play    \tj: fade out   \ti: side cut   \td: pitch down\r\n"
				"c: pause   \tk: fade in    \to: center cut \tf: pitch up\r\n"
				"v: stop    \tl: loop       \tn: volume down\tg: tempo down\r\n"
				"y: rew     \tp: equalizer  \tm: volume up  \th: tempo up\r\n"
				"b: fwd     \te: echo       \ta: rate down  \r\n"
				"r: reverse \tt: channel mix\ts: rate up    \tq: quit\r\n\r\n");
		


		// open wavein using Line In
     	if(player->OpenFile("wavein://src=line;volume=50;", sfAutodetect) == 0)
		{
        	printf("Error: %s\nPress key to exit.\r\n", player->GetError());
            getch();
            player->Release();
            return 0;
        }

		// set wave output to disk file, ogg encoding
		if(player->SetWaveOutFile("out.ogg", sfOgg, 1) == 0)
		{
        	printf("Error: %s\nPress key to exit.\r\n", player->GetError());
            getch();
            player->Release();
            return 0;

		}

	// get stream info
	TStreamInfo pInfo;

	player->GetStreamInfo(&pInfo);


	// display info about stream
	char *sVbr = "CBR";
	if(pInfo.VBR)
		sVbr = "VBR";

     printf("\r\n%s %i Hz %s  Channel: %u  Length: %02i:%02i:%02i:%02i\r\n\r\n",
			pInfo.Description,
			pInfo.SamplingRate,
			sVbr,
			pInfo.ChannelNumber,
			pInfo.Length.hms.hour,
			pInfo.Length.hms.minute,
			pInfo.Length.hms.second,
			pInfo.Length.hms.millisecond);


	printf("Status| kbps |  POS:  |Echo|Equ|Cut|Fade|Rev|Rate|Pitch|Tempo|Vol|Loop|Ch mix|\r\n");


	// start playing
    if(player->Play() == 0)
	{
		printf("Error: %s\nPress key to exit.\r\n", player->GetError());
		player->Release(); // delete ZPlay class
		return 0;
	}

	

	while(1)
	{

		// get current status
     	player->GetStatus(&status); 

		char *st = 0;
		if(status.fPlay)
			st = "Play ";
		else if(status.fPause)
			st = "Pause";
		else
			st = "Stop ";		
	
		// get current position
		player->GetPosition(&pos);


     	printf("%s   %04i  %02i:%02i:%02i   %i   %i   %i   %i    %i   %03i  %03i   %03i  %03i   %i    %i\r", 
				st,
				player->GetBitrate(0),
        		pos.hms.hour,
				pos.hms.minute,
				pos.hms.second,
				status.fEcho,
				status.fEqualizer,
				status.fVocalCut,
				status.fSlideVolume,
				status.fReverse,
				player->GetRate(),
				player->GetPitch(),
				player->GetTempo(),
				lVolume,
				status.nLoop,
				fMixChannels
				);



		if(kbhit())
		{
           	int a = getch();

			
            switch(a)
			{

				case 'i':	// side cut
					fSideCut = !fSideCut;
					player->StereoCut(fSideCut, 1, 0);
					fCenterCut = 0;
				break;

				case 'o':	// center
					fCenterCut = !fCenterCut;
					player->StereoCut(fCenterCut, 0, 0);
					fSideCut = 0;
				break;

				case 's':	// higher rate
					nRate += 5;
					player->SetRate(nRate);
				break;

				case 'a':	// lower rate
					nRate -= 5;
					if(nRate < 0)
						nRate = 0;
					player->SetRate(nRate);
				break;

				case 'd':	// lower pitch
					nPitch -= 5;
					if(nPitch < 0)
						nPitch = 0;
					player->SetPitch(nPitch);
				break;

				case 'f':	// higher pitch
					nPitch += 5;
					player->SetPitch(nPitch);
				break;


				case 'g':	// lower tempo
					nTempo -= 5;
					if(nTempo < 0)
						nTempo = 0;
					player->SetTempo(nTempo);
				break;

				case 'h':	// higher tempo
					nTempo += 5;
					player->SetTempo(nTempo);
				break;

				case 'r':	// reverse mode
					fReverse = !fReverse;

					if(player->ReverseMode(fReverse) == 0)
					{
						printf("Error: %s\n", player->GetError());
						fReverse = 0;
					}
				break;


				case 'p':	// enable or disable equalizer
				{
					fEq = !fEq;

					player->EnableEqualizer(fEq);

					if(fEq)
					{
						player->SetEqualizerPreampGain(-100000);
						player->SetEqualizerBandGain(0, 104000);
						player->SetEqualizerBandGain(1, 104000);
						player->SetEqualizerBandGain(2, 104000);

					}

					
				}
				break;

				case 'l': // play loop
				{
					TStreamTime pCurrentTime;
					player->GetPosition(&pCurrentTime);
					TStreamTime pEndTime;
					pEndTime.ms = pCurrentTime.ms + 5000;
					player->PlayLoop(tfMillisecond, &pCurrentTime, tfMillisecond, &pEndTime ,5, 1);
				}
				break;

				case 'j': // fade out volume
				{
					TStreamTime start;
					TStreamTime end;
			
					player->GetPosition(&start);
					
					end.sec = start.sec + 5;
					player->SlideVolume( tfSecond, &start, 100,100, tfSecond, &end, 0,0);
		

				}
				break;

				case 'k': // fade in volume
				{
					TStreamTime start;
					player->GetPosition(&start);
					TStreamTime end;
					end.sec = start.sec + 5;
					player->SlideVolume( tfSecond, &start, 0,0, tfSecond, &end, 100, 100);
				}
				break;

            	
            	case 'e': // enable echo
				{
					fEcho = !fEcho;
            		player->EnableEcho(fEcho);
				}
            	break;
            	
            	case 'x': // play
                	if(player->Play() == 0)
						printf("Err: %s\n", player->GetError());
					fPause = 0;
                break;

                case 'v': // stop
                	player->Stop();
					fPause = 0;
                break;

                case 'c':	// pause
                	if(fPause)
					{
                    	player->Resume();
                        fPause = 0;
                    }
                    else
					{
                		player->Pause();
                        fPause = 1;
                    }
                break;

                case 'y': // jump back 5 seconds
				{
					TStreamTime pTime;
					pTime.sec = 5;
                	player->Seek(tfSecond, &pTime, smFromCurrentBackward);
				}
                break;

                case 'b': // jump forward 5 seconds
				{
                	TStreamTime pTime;
					pTime.sec = 5;
					player->Seek(tfSecond, &pTime, smFromCurrentForward);
			
				}
                break;

				case 't':
				{
					// mix stereo channels to mono
					fMixChannels = !fMixChannels;
					player->MixChannels(fMixChannels, 50, 50);

				}
				break;

				case 'n':	// volume down
					if(lVolume >= 5)
						lVolume -= 5;

					if(rVolume >= 5)
						rVolume -= 5;

					player->SetMasterVolume(lVolume, rVolume);
				break;

				case 'm':	// volume up
					if(lVolume <= 95)
						lVolume += 5;

					if(rVolume <= 95)
						rVolume += 5;

					player->SetMasterVolume(lVolume, rVolume);
				break;

                case 'q':
				case 'Q':
					player->Release();	
                return 0; 
            }
        }

        Sleep(50);
	}
}

