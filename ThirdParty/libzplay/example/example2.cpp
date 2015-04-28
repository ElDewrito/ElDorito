/*
 *	This is small example how to use libZPlay library to play streams.
 *	This example is using OpenStream functions to open memory and play.
 *	It includes example of using callback function and callback messages
 *	to push more data into stream.
 *
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <conio.h>



// include ZPlay class header 
#include "../include/libzplay.h"

// must use namespace
using namespace libZPlay;

// callback function prototype, need this to use callback message to push more data into managed stream
int  __stdcall  CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2);




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

#define BUFFER_SIZE 50000

char buf[BUFFER_SIZE];



int main(int argc, char **argv)
{
	

	// clear screen
	system("CLS");

	// create class instance
	ZPlay *player = CreateZPlay();

 

	// chek if we have class instance
	if(player == 0)
	{
		printf("Error: Can't create class instance !\nPress key to exit.\n");
		getch();
		return 0;	
	}


	// get library version
	int ver = player->GetVersion();
	// check if we have version 1.90 and above
	if(ver < 190)
	{
		printf("Error: Need library version 1.00 and above !\nPress key to exit.\r\n");
		getch();
		player->Release();
		return 0;
	}


	// display version info
	printf("libZPlay v.%i.%02i\r\n\r\n", ver / 100, ver % 100);


	FILE *in = NULL;
	
	if(argc > 1) 
	{
		printf( "x: play    \tj: fade out   \ti: side cut   \td: pitch down\r\n"
				"c: pause   \tk: fade in    \to: center cut \tf: pitch up\r\n"
				"v: stop    \tl: loop       \tn: volume down\tg: tempo down\r\n"
				"y: rew     \tp: equalizer  \tm: volume up  \th: tempo up\r\n"
				"b: fwd     \te: echo       \ta: rate down  \r\n"
				"r: reverse \tt: channel mix\ts: rate up    \tq: quit\r\n\r\n");
		

		TStreamFormat nFormat = player->GetFileFormat(argv[1]);

		
		if(nFormat == sfUnknown)
		{
			printf("Error: Unknown file format.\nPress key to exit.\r\n");
			getch();
            player->Release();
            return 0;
		}
	

		// open disk file, I will use this to load file into memory and then use OpenStream to play this memory data
		in = fopen(argv[1], "rb");
		if(in == 0)
		{
			printf("Error: Can't open file.\r\nPress key to exit.\r\n");
			getch();
            player->Release();
            return 0;	
		}

		// read starting chunk of data into memory buffer. This data chunk must contain
		// enough data for decoder to detect stream type. For example, in this starting chunk must be 
		// at least one valid mp3 frame, or ogg page, or FLAC frame, ...
		int nRead = fread(buf, 1, BUFFER_SIZE, in);

		if(nRead == 0)
		{
			printf("Error: Can't read file.\r\nPress key to exit.\r\n");
			getch();
            player->Release();
			fclose(in);
            return 0;	
		}


 
		// set callback function. Send me message when dynamic stream needs more data.
		player->SetCallbackFunc(CallbackFunc, MsgStreamNeedMoreDataAsync, in);


		// Now I have data in memory buffer.
		// Open stream as dynamic stream ( I will push more data into this stream later ), buffered stream ( class will
		// save data from buffer into internal memory so I can destroy data in buffer ).
     	if(player->OpenStream(1, 1, buf, nRead, nFormat) == 0)
		{
        	printf("Error: %s\nPress key to exit.\r\n", player->GetError());
            getch();
            player->Release();
			fclose(in);
            return 0;
        }

	

		// set some echo efffect
		TEchoEffect effect[2];
		effect[0].nLeftDelay = 2000;
		effect[0].nLeftEchoVolume = 20;
		effect[0].nLeftSrcVolume = 80;
		effect[0].nRightDelay = 500;
		effect[0].nRightEchoVolume = 20;
		effect[0].nRightSrcVolume = 80;

		effect[1].nLeftDelay = 300;
		effect[1].nLeftEchoVolume = 20;
		effect[1].nLeftSrcVolume = 0;
		effect[1].nRightDelay = 300;
		effect[1].nRightEchoVolume = 20;
		effect[1].nRightSrcVolume = 0;
		player->SetEchoParam(effect, 2);


		// get ID3 info
		TID3Info id3_info;
		
		// try to load some ID3 data from starting memory chunk
		if(player->LoadID3(id3Version2, &id3_info)) // loading ID3v2
		{
            printf("Title:   %s\r\n", id3_info.Title);
            printf("Artist:  %s\r\n", id3_info.Artist);
            printf("Album:   %s\r\n", id3_info.Album);
            printf("Year:    %s\r\n", id3_info.Year);
            printf("Comment: %s\r\n", id3_info.Comment);
            printf("Genre:   %s\r\n", id3_info.Genre);
			printf("Track:   %s\r\n\r\n", id3_info.TrackNum);
			
        }
		else if(player->LoadID3(id3Version1, &id3_info))
		{ 
			printf("Title:   %s\r\n", id3_info.Title);
			printf("Artist:  %s\r\n", id3_info.Artist);
			printf("Album:   %s\r\n", id3_info.Album);
			printf("Year:    %s\r\n", id3_info.Year);
			printf("Comment: %s\r\n", id3_info.Comment);
			printf("Genre:   %s\r\n", id3_info.Genre);
			printf("Track:   %s\r\n\r\n", id3_info.TrackNum);	
		}
		else
		{
			printf("No ID3 data\r\n\r\n");
		}

     }
     else
	 {
     // no filename in argument

		player->Release(); // delete ZPlay class
		char *end = strrchr(argv[0], '\\');
		if(end && *(end + 1) != 0)
			end++;
		else
			end =  argv[0];

     	printf("Usage: %s filename\r\n\r\nPress key to exit\r\n", end);
        getch();
     	return 0;
     }



	// get stream info
	TStreamInfo pInfo;

	player->GetStreamInfo(&pInfo);

	

	// display info about stream
	char *sVbr = "CBR";
	if(pInfo.VBR)
		sVbr = "CBR";

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
		printf("Error: %s\nPress key to exit.\r\n", player->GetError());

	

	int running = 1;
	while(running)
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

				case '1':
				player->SetFFTGraphParam(gpGraphType, gtAreaLeftOnTop);
			break;

			case '2':
				player->SetFFTGraphParam(gpFFTPoints, 2048);
			break;

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
					DWORD t1 = GetTickCount();
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
					running = 0;
                break;
            }
        }

        Sleep(50);
	}

	if(in)
		fclose(in);

	player->Close();
	player->Release();
	return 1;

}

int  __stdcall  CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
	ZPlay *myinstance = (ZPlay*) instance;

	switch(message)
	{
		case MsgStreamNeedMoreDataAsync:
		{
			FILE *in = (FILE*) user_data;

			// read data into memory buffer
			int nRead = fread(buf, 1, BUFFER_SIZE, in);

			if(nRead == 0)
			{
				// there is no data in file, end of file
				// push null buffer to indicate that there will be no more data
				myinstance->PushDataToStream(0, 0);	
			}
			else
			{
				// add new data to stream
				myinstance->PushDataToStream(buf, nRead);	
			}
		}
		return 0;
	}

	return 0;
}