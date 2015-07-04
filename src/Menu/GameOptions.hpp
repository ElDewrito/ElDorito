/*
To get base address, do this:
1. find TEAM_MODE address
2. Find out what writes to this address
3. Copy easy guess

TODO:
1. need to get a way to open x menu
2. need way to change map & game mode & game mode #2
3. Skipped infection
*/


class GameOptions;

class GameOptions
{
public:
	char pad_0x0000[0x120]; //0x0000
	unsigned char TEAM_MODE; //0x0120 6=disabled, 7=enabled
	char pad_0x0121[0xAF]; //0x0121
	__int16 SLAYER_JUGGERNAUT_VIP_SCORE_TO_WIN_ONE-SIDED_TERRITORIES; //0x01D0   -1=unlimited for slayer, 0=unlimited for juggernaut; 2=disabled, 3=enabled
	unsigned char CTF_NUM_OF_FLAGS; //0x01D2 0=multiple flags, 1=one flag
	char pad_0x01D3[0x1]; //0x01D3
	__int16 ODDBALL_KOTH_SCORE_TO_WIN_ASSAULT_BOMB_MODE_TERRITORY_CAPTURE_TIME_SINGLE_VIP; //0x01D4    -1=unlimited for oddball, 0=unlimited for KOTH; 0=multi-bomb, 1=one-sided, 2=neutral-bomb; in seconds, 0=instant; 0=disabled, 1=enabled
	char pad_0x01D6[0x2]; //0x01D6
	unsigned char CTF_CAPTURES_TO_WIN_ASSAULT_DETONATIONS_TO_WIN; //0x01D8 0=unlimited
char pad_0x01D9[0x267]; //0x01D9

};//Size=0x0440