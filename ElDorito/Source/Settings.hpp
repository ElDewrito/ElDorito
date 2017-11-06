#include <stdint.h>

class Settings // always at 0x23019B8 (static address)
{
public:
	char pad_0x0000[0x33C]; //0x0000
	uint8_t FULL_SCREEN; //0x033C 
	char pad_0x033D[0x7]; //0x033D
	__int16 BRIGHTNESS; //0x0344 
	char pad_0x0348[0x8]; //0x0348
	uint32_t HORIZONTAL_RESOLUTION; //0x0350 
	uint32_t VERTICAL_RESOLUTION; //0x0354 
	char pad_0x0358[0x8]; //0x0358
	uint32_t TEXTURE_RESOLUTION; //0x0360  0=low, 1=medium, 2=high
	uint32_t SHADOW_QUALITY; //0x0364 
	uint32_t TEXTURE_FILTERING; //0x0368 
	uint32_t LIGHTNING_QUALITY; //0x036C 
	uint32_t EFFECTS_QUALITY; //0x0370 
	uint32_t DETAILS_LEVEL; //0x0374 
	uint32_t POSTPROCESSING; //0x0378 
	uint8_t MOTION_BLUR; //0x037C 
	uint8_t VSYNC; //0x037D 
	uint8_t ANTI_ALIASING; //0x037E 
	char pad_0x037F[0x41]; //0x037F
	uint32_t MASTER_VOLUME; //0x03C0 
	uint32_t SFX_VOLUME; //0x03C4 
	uint32_t MUSIC_VOLUME; //0x03C8 
	char pad_0x03CC[0xC]; //0x03CC
	uint8_t TOGGLE_CROUCH; //0x03D8 
	uint8_t HUD_SHAKE; //0x03D9 
	char pad_0x03DA[0x2]; //0x03DA
	uint8_t PLAYER_MARKER_COLORS; //0x03DC  0=default, 1=ally blue, 2=armor colors
	char pad_0x03DD[0x3]; //0x03DD
	uint16_t PADDING; //0x03E0 
	uint8_t FOV_DOUBLED; //0x03E2 Divide this value by 2 to get actual FOV
	char pad_0x03E3[0x1]; //0x03E3
	uint8_t CONTROLS_METHOD; //0x03E4  0=keyboard, 1=gamepad
	char pad_0x03E5[0x3]; //0x03E5
	uint32_t INFANTRY_VERTICAL_MOUSE_SENS; //0x03E8 
	uint32_t INFANTRY_HORIZONTAL_MOUSE_SENS; //0x03EC 
	uint32_t VEHICLE_VERTICAL_MOUSE_SENS; //0x03F0 
	uint32_t VEHICLE_HORIZONTAL_MOUSE_SENS; //0x03F4 
	uint8_t MOUSE_ACCELERATION; //0x03F8 
	char pad_0x03F9[0x3]; //0x03F9
	uint8_t MOUSE_FILTER; //0x03FC 
	uint8_t INVERT_MOUSE; //0x03FD 
};//Size=0x03FE