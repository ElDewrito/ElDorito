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


#include "../include/libzplay.h"

using namespace libZPlay;

ZPlay* player;

 

int  __stdcall  CallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2);


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



	if(argc > 1) 
	{


		TID3InfoEx id3_info;
		if(player->LoadFileID3Ex(argv[1], sfAutodetect, &id3_info, 1)) // loading ID3v2
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
			
			// draw picture on desktop window
			player->DrawBitmapToHWND(NULL, 0, 0, 0, 0, id3_info.Picture.hBitmap);
								
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

	getch();
}
