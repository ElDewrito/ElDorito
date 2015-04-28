/*
* libZPlay example
*
* Gapless playing of 3 files.
*
* Use callback mechanism to get information on song change
*/


#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define LIB_ZPLAY_STATIC
#include "libzplay.h"

using namespace libZPlay;

// callback function prototype
int  __stdcall  myCallbackFunc(void* instance,
                            void *user_data,
                            TCallbackMessage message,
                            unsigned int param1,
                            unsigned int param2);



int main(int argc, char **argv)
{
    printf("Gapless playing of 3 files.nnPress q to endnn");

    // create class instance using class factory.
    ZPlay *player = CreateZPlay();

    // set callback mechanism to intercept MsgNextSongAsync message
    // this message is sent when one  next fong from gapless queue starts playing
    player->SetCallbackFunc(myCallbackFunc, (TCallbackMessage) (MsgNextSongAsync), 0);

    // add first file to gapless queue
    int result = player->AddFile("test.mp3", sfAutodetect);
    if(result == 0)
    {
        // display error message
        printf("Error: %sn", player->GetError());
        player->Release();
        return 0;
    }

    // add second file to gapless queue
    result = player->AddFile("test1.mp3", sfAutodetect);
    if(result == 0)
    {
        // display error message
        printf("Error: %sn", player->GetError());
    }

    // add third fiel to gapless queue
    result = player->AddFile("test2.mp3", sfAutodetect);
    if(result == 0)
    {
        // display error message
        printf("Error: %sn", player->GetError());
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
        printf("Pos: %02u:%02u:%02u:%03ur", pos.hms.hour, pos.hms.minute, pos.hms.second, pos.hms.millisecond);

        Sleep(300); // wait 300 ms
    }

    // destroy class instance
    player->Release();

    return 0;
}


int  __stdcall  myCallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
    ZPlay *myinstance = (ZPlay*) instance;

    switch(message)
    {
        case MsgNextSongAsync: //  next song from gapless queue starts playing
        {
            printf("Playing song: %u  - Left in queue: %un", param1, param2);
        }
        return 0;
    }

    return 0;
}