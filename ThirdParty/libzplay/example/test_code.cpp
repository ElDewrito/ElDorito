/*
 *	This is small example how to use libZPlay library to play files.
 *	This example is using OpenFile functions to open disk files and play.
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



// we will link this with static version of libZPLay library
#define LIB_ZPLAY_STATIC
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



long filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, 2);
   length = ftell(stream);
   fseek(stream, curpos, 0);
   return length;

}

int  __stdcall  CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2);


int main(int argc, char **argv)
{
	// clear screen
	system("CLS");



	// create class instance
	player = CreateZPlay();

	player->SetSettings(sidAccurateLength,1);

	player->SetCallbackFunc(CallbackFunc, MsgNextSong, 0);


	// chek if we have class instance
	if(player == 0)
	{
		printf("Error: Can't create class instance !\nPress key to exit.\n");
		getch();
		return 0;	
	}

	// get library version
	int ver = player->GetVersion();
	// check if we have version 1.00 and above
	if(ver < 100)
	{
		printf("Error: Need library version 1.00 and above !\nPress key to exit.\r\n");
		getch();
		player->Release();
		return 0;
	}


	// display version info
	printf("libZPlay v.%i.%02i\r\n\r\n", ver / 100, ver % 100);


	//printf("BPM: %u\n",player->DetectFileBPM("test2.mp3", sfMp3, dmPeaks));



/*
	char line[MAX_PATH];
	unsigned int n = 0;
	size_t size = 0;

	FILE *in = fopen("lista.m3u", "rt");


	TStreamInfo info1;
	TID3Info id3;
	TID3InfoW id3W;
	TID3InfoEx id3Ex;
	TID3InfoExW id3ExW;
	while(fgets(line, MAX_PATH, in))
	{
		size = strlen(line);
		if(size)
			line[size - 1] = 0;


	//	if(strstr(line, ".wma") == 0)
		//	continue;


		printf("%u: FILE:  %s\n", n, line);

		int found = 0;
		
		if(player->OpenFile(line, sfMp3))
		{
			player->GetStreamInfo(&info1);
			
			found = 0;

			if(player->LoadID3Ex(&id3Ex, 1))
				found = 1;
			
			if(found)
			{
				printf("%u: %s - %s  %u:%u\n", n, id3Ex.Artist, id3Ex.Title, info1.Length.hms.minute, info1.Length.hms.second);
				player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3Ex.Picture.hBitmap);
			}
			else
			{
				printf("%u: NO ID3  %s  %u:%u\n", n, line, info1.Length.hms.minute, info1.Length.hms.second);
			}
			
			found = 0;

			if(player->LoadID3ExW(&id3ExW, 1))
				found = 1;
			
			if(found)
			{
				wprintf(L"%u: %s - %s  %u:%u\n", n, id3ExW.Artist, id3ExW.Title, info1.Length.hms.minute, info1.Length.hms.second);
				player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3ExW.Picture.hBitmap);
			}
			else
			{
				printf("%u: NO ID3  %s  %u:%u\n", n, line, info1.Length.hms.minute, info1.Length.hms.second);
			}

			found = 0;

			if(player->LoadID3(id3Version2, &id3))
				found = 1;
			
			if(found)
			{
				printf("%u: %s - %s  %u:%u\n", n, id3.Artist, id3.Title, info1.Length.hms.minute, info1.Length.hms.second);
			
			}
			else
			{
				printf("%u: NO ID3  %s  %u:%u\n", n, line, info1.Length.hms.minute, info1.Length.hms.second);
			}

			found = 0;

			if(player->LoadID3W(id3Version2, &id3W))
				found = 1;
			
			
			if(found)
			{
				wprintf(L"%u: %s - %s  %u:%u\n", n, id3W.Artist, id3W.Title, info1.Length.hms.minute, info1.Length.hms.second);
			}
			else
			{
				printf("%u: NO ID3  %s  %u:%u\n", n, line, info1.Length.hms.minute, info1.Length.hms.second);
			}
	
		}

		
		else
		{
			printf( "%u: ERROR  %s\n", n, line);
		}


		n++;

	}


	fclose(in);
	getch();
	player->Release();
	return 0;
*/

	if(argc > 1) 
	{
		printf( "x: play    \tj: fade out   \ti: side cut   \td: pitch down\r\n"
				"c: pause   \tk: fade in    \to: center cut \tf: pitch up\r\n"
				"v: stop    \tl: loop       \tn: volume down\tg: tempo down\r\n"
				"y: rew     \tp: equalizer  \tm: volume up  \th: tempo up\r\n"
				"b: fwd     \te: echo       \ta: rate down  \r\n"
				"r: reverse \tt: channel mix\ts: rate up    \tq: quit\r\n\r\n");
		


		// open file
     	if(player->OpenFile(argv[1], sfAutodetect) == 0)
	//	if(player->OpenFileW(L"šðèæž.mp3", sfAutodetect) == 0)
		//if(player->OpenFileW(L"test2.mp3", sfAutodetect) == 0)
		{
        	printf("Error: %s\nPress key to exit.\r\n", player->GetError());
            getch();
            player->Release();
            return 0;
        }
/*
		if(player->AddFile("t2.mp3", sfMp3) == 0)
		{
        	printf("Error: %s\nPress key to exit.\r\n", player->GetError());
            getch();
            player->Release();
            return 0;
        }
*/

/*
		char *tmp = (char*) malloc(10000000);
		FILE *in = fopen(argv[1], "rb");
		long size = filesize(in);
		fread(tmp, size, 1, in);
		fclose(in);


		if(player->OpenStream(1, 1, tmp, size, sfMp3) == 0)
		{
        	printf("Error: %s\nPress key to exit.\r\n", player->GetError());
            getch();
            player->Release();
            return 0;
        }

		free(tmp);
*/

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


/*

		// get ID3 info
		TID3InfoEx id3_info;
		
	
		if(player->LoadID3Ex(&id3_info, 1)) // loading ID3v2
		{
            printf("Title:   %s\r\n", id3_info.Title);
            printf("Artist:  %s\r\n", id3_info.Artist);
            printf("Album:   %s\r\n", id3_info.Album);
            printf("Year:    %s\r\n", id3_info.Year);
            printf("Comment: %s\r\n", id3_info.Comment);
            printf("Genre:   %s\r\n", id3_info.Genre);
			printf("Track:   %s\r\n\r\n", id3_info.TrackNum);

			printf("Artist1 :  %s\r\n", id3_info.AlbumArtist );
			printf("Composer:  %s\r\n", id3_info.Composer );
			printf("Original:  %s\r\n", id3_info.OriginalArtist );
			printf("Copyright: %s\r\n", id3_info.Copyright );
			printf("URL:       %s\r\n", id3_info.URL );
			printf("Encoder:   %s\r\n", id3_info.Encoder );
			printf("Publisher: %s\r\n", id3_info.Publisher );
			printf("BPM:       %u\r\n", id3_info.BPM);

			printf("MIME:      %s\r\n", id3_info.Picture.MIMEType);
			printf("TYPE:      %u\r\n", id3_info.Picture.PictureType);
			printf("Desc:      %s\r\n", id3_info.Picture.Description);

			printf("Size:      %u\r\n", id3_info.Picture.PictureDataSize);

			FILE *out = fopen("out.jpg", "wb");
			fwrite(id3_info.Picture.PictureData, id3_info.Picture.PictureDataSize, 1, out);
			fclose(out);

			player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3_info.Picture.hBitmap);
						
        }
		else
		{
			printf("No ID3 data\r\n\r\n");
		}
*/
		



		TID3InfoExW id3_info;
		for(unsigned int i = 0; i < 5000; i++)
		if(player->LoadID3ExW(&id3_info, 1)) // loading ID3v2
		{
            wprintf(L"Title:   %s\r\n", id3_info.Title);
            wprintf(L"Artist:  %s\r\n", id3_info.Artist);
            wprintf(L"Album:   %s\r\n", id3_info.Album);
            wprintf(L"Year:    %s\r\n", id3_info.Year);
            wprintf(L"Comment: %s\r\n", id3_info.Comment);
            wprintf(L"Genre:   %s\r\n", id3_info.Genre);
			wprintf(L"Track:   %s\r\n\r\n", id3_info.TrackNum);

			wprintf(L"Artist1 :  %s\r\n", id3_info.AlbumArtist );
			wprintf(L"Composer:  %s\r\n", id3_info.Composer );
			wprintf(L"Original:  %s\r\n", id3_info.OriginalArtist );
			wprintf(L"Copyright: %s\r\n", id3_info.Copyright );
			wprintf(L"URL:       %s\r\n", id3_info.URL );
			wprintf(L"Encoder:   %s\r\n", id3_info.Encoder );
			wprintf(L"Publisher: %s\r\n", id3_info.Publisher );
			wprintf(L"BPM:       %u\r\n", id3_info.BPM);

			wprintf(L"MIME:      %s\r\n", id3_info.Picture.MIMEType);
			wprintf(L"TYPE:      %u\r\n", id3_info.Picture.PictureType);
			wprintf(L"Desc:      %s\r\n", id3_info.Picture.Description);

			wprintf(L"Size:      %u\r\n", id3_info.Picture.PictureDataSize);

			FILE *out = fopen("out.jpg", "wb");
			fwrite(id3_info.Picture.PictureData, id3_info.Picture.PictureDataSize, 1, out);
			fclose(out);

			player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3_info.Picture.hBitmap);
						
        }
		else
		{
			printf("No ID3 data\r\n\r\n");
		}

	

/*
		TID3InfoW id3_info;
		if(player->LoadID3W(id3Version2, &id3_info)) // loading ID3v2
		{
            wprintf(L"Title:   %s\r\n", id3_info.Title);
            wprintf(L"Artist:  %s\r\n", id3_info.Artist);
            wprintf(L"Album:   %s\r\n", id3_info.Album);
            wprintf(L"Year:    %s\r\n", id3_info.Year);
            wprintf(L"Comment: %s\r\n", id3_info.Comment);
            wprintf(L"Genre:   %s\r\n", id3_info.Genre);
			wprintf(L"Track:   %s\r\n\r\n", id3_info.TrackNum);
						
        }
		else
		{
			printf("No ID3 data\r\n\r\n");
		}
*/

/*

		TID3Info id3_info;
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
		else
		{
			printf("No ID3 data\r\n\r\n");
		}

*/
/*
		TID3Info id3_info;
		if(player->LoadFileID3("test2.mp3", sfAutodetect, id3Version2, &id3_info)) // loading ID3v2
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

*/
/*
		TID3InfoW id3_info;
		if(player->LoadFileID3W(L"test2.mp3", sfAutodetect, id3Version2, &id3_info)) // loading ID3v2
		{
            wprintf(L"Title:   %s\r\n", id3_info.Title);
            wprintf(L"Artist:  %s\r\n", id3_info.Artist);
            wprintf(L"Album:   %s\r\n", id3_info.Album);
            wprintf(L"Year:    %s\r\n", id3_info.Year);
            wprintf(L"Comment: %s\r\n", id3_info.Comment);
            wprintf(L"Genre:   %s\r\n", id3_info.Genre);
			wprintf(L"Track:   %s\r\n\r\n", id3_info.TrackNum);					
        }
		else
		{
			printf("No ID3 data\r\n\r\n");
		}

	*/	

/*
		TID3InfoEx id3_info;
		for(int i = 0; i < 5000; i++)
		if(player->LoadFileID3Ex("test2.mp3", sfAutodetect, &id3_info, 1)) // loading ID3v2
		{
		
            printf("Title:   %s\r\n", id3_info.Title);
            printf("Artist:  %s\r\n", id3_info.Artist);
            printf("Album:   %s\r\n", id3_info.Album);
            printf("Year:    %s\r\n", id3_info.Year);
            printf("Comment: %s\r\n", id3_info.Comment);
            printf("Genre:   %s\r\n", id3_info.Genre);
			printf("Track:   %s\r\n\r\n", id3_info.TrackNum);

			printf("Artist1 :  %s\r\n", id3_info.AlbumArtist );
			printf("Composer:  %s\r\n", id3_info.Composer );
			printf("Original:  %s\r\n", id3_info.OriginalArtist );
			printf("Copyright: %s\r\n", id3_info.Copyright );
			printf("URL:       %s\r\n", id3_info.URL );
			printf("Encoder:   %s\r\n", id3_info.Encoder );
			printf("Publisher: %s\r\n", id3_info.Publisher );
			printf("BPM:       %u\r\n", id3_info.BPM);

			printf("MIME:      %s\r\n", id3_info.Picture.MIMEType);
			printf("TYPE:      %u\r\n", id3_info.Picture.PictureType);
			printf("Desc:      %s\r\n", id3_info.Picture.Description);

			printf("Size:      %u\r\n", id3_info.Picture.PictureDataSize);
			


			player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3_info.Picture.hBitmap);
								
        }
		else
		{
			printf("No ID3 data\r\n\r\n");
		}

	*/

/*
		char b[100000];
		memset(b, 0, 10000);
		TID3InfoEx id3_info;
		for(int i = 0; i < 5000; i++)
		//if(player->OpenFileW(L"empty.mp3", sfMp3)) // loading ID3v2
		if(player->OpenStream(1,0,  b, 100000, sfMp3)) // loading ID3v2
		{
			if(player->LoadID3Ex(&id3_info, 1))
			{
			
	
				printf("Title:   %s\r\n", id3_info.Title);
				printf("Artist:  %s\r\n", id3_info.Artist);
				printf("Album:   %s\r\n", id3_info.Album);
				printf("Year:    %s\r\n", id3_info.Year);
				printf("Comment: %s\r\n", id3_info.Comment);
				printf("Genre:   %s\r\n", id3_info.Genre);
				printf("Track:   %s\r\n\r\n", id3_info.TrackNum);

				printf("Artist1 :  %s\r\n", id3_info.AlbumArtist );
				printf("Composer:  %s\r\n", id3_info.Composer );
				printf("Original:  %s\r\n", id3_info.OriginalArtist );
				printf("Copyright: %s\r\n", id3_info.Copyright );
				printf("URL:       %s\r\n", id3_info.URL );
				printf("Encoder:   %s\r\n", id3_info.Encoder );
				printf("Publisher: %s\r\n", id3_info.Publisher );
				printf("BPM:       %u\r\n", id3_info.BPM);

				printf("MIME:      %s\r\n", id3_info.Picture.MIMEType);
				printf("TYPE:      %u\r\n", id3_info.Picture.PictureType);
				printf("Desc:      %s\r\n", id3_info.Picture.Description);

				printf("Size:      %u\r\n", id3_info.Picture.PictureDataSize);
				


				player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3_info.Picture.hBitmap);
							
			}
			else
			{
				printf("No ID3 data\r\n\r\n");
			}
		}
		else
		{
			printf("error\n");
		}

*/
/*

		TID3InfoExW id3_info;
		if(player->LoadFileID3ExW(L"test2.mp3", sfAutodetect, &id3_info, 1)) // loading ID3v2
		{
            wprintf(L"Title:   %s\r\n", id3_info.Title);
            wprintf(L"Artist:  %s\r\n", id3_info.Artist);
            wprintf(L"Album:   %s\r\n", id3_info.Album);
            wprintf(L"Year:    %s\r\n", id3_info.Year);
            wprintf(L"Comment: %s\r\n", id3_info.Comment);
            wprintf(L"Genre:   %s\r\n", id3_info.Genre);
			wprintf(L"Track:   %s\r\n\r\n", id3_info.TrackNum);

			wprintf(L"Artist1 :  %s\r\n", id3_info.AlbumArtist );
			wprintf(L"Composer:  %s\r\n", id3_info.Composer );
			wprintf(L"Original:  %s\r\n", id3_info.OriginalArtist );
			wprintf(L"Copyright: %s\r\n", id3_info.Copyright );
			wprintf(L"URL:       %s\r\n", id3_info.URL );
			wprintf(L"Encoder:   %s\r\n", id3_info.Encoder );
			wprintf(L"Publisher: %s\r\n", id3_info.Publisher );
			wprintf(L"BPM:       %u\r\n", id3_info.BPM);

			wprintf(L"MIME:      %s\r\n", id3_info.Picture.MIMEType);
			wprintf(L"TYPE:      %u\r\n", id3_info.Picture.PictureType);
			wprintf(L"Desc:      %s\r\n", id3_info.Picture.Description);

			wprintf(L"Size:      %u\r\n", id3_info.Picture.PictureDataSize);

		

			player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3_info.Picture.hBitmap);				
        }
		else
		{
			printf("No ID3 data\r\n\r\n");
		}

		*/
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
/*

	// start playing
    if(player->Play() == 0)
	{
		printf("Error: %s\nPress key to exit.\r\n", player->GetError());
		player->Release(); // delete ZPlay class
		return 0;
	}

*/
	

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











int  __stdcall  CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
	ZPlay *myinstance = (ZPlay*) instance;

	switch(message)
	{
		case MsgNextSong:
		{
			printf("Index: %u  Left: %u\n", param1, param2);
		}
		break;
	}

	return 0;
}