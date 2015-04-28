#include "RawInput.h"

#include "../ElDorito.h"
#include "../Patch.h"
#include "../BlamTypes.h"

namespace
{
	bool g_enabled = false;
	void RawInputHook();
}

namespace Patches
{
	namespace RawInput
	{
		void ApplyAll()
		{
			// Hook the input handling routine to fix mouse acceleration
			Hook(0x112395, RawInputHook).Apply();
		}

		void Enable(bool enabled)
		{
			g_enabled = enabled;
		}

		bool IsEnabled()
		{
			return g_enabled;
		}
	}
}

namespace
{
	bool RawInputHookImpl(RAWINPUT *rwInput)
	{
		if (!g_enabled)
			return false;
		if (rwInput->header.dwType != RIM_TYPEMOUSE)
			return true;

		Pointer InputPtr = ElDorito::GetMainTls(GameGlobals::Input::TLSOffset)[0];
		Pointer &horizPtr = InputPtr(GameGlobals::Input::HorizontalViewAngle);
		Pointer &vertPtr = InputPtr(GameGlobals::Input::VerticalViewAngle);
		float currentHoriz = horizPtr.Read<float>();
		float currentVert = vertPtr.Read<float>();
		float weaponSensitivity = Pointer::Base(0x4CDEF14).Read<float>();
		float maxVertAngle = Pointer::Base(0x14B49E4).Read<float>();

		Pointer SettingsPtr = ElDorito::GetMainTls(GameGlobals::GameSettings::TLSOffset)[0];
		if (SettingsPtr == 0) // game itself does this the same way, not sure why it'd be 0 in TLS data though since the game is also meant to set it in TLS if its 0
			SettingsPtr = Pointer(0x22C0128);

		Pointer &invertedPtr = SettingsPtr(GameGlobals::GameSettings::YAxisInverted);
		Pointer &yaxisPtr = SettingsPtr(GameGlobals::GameSettings::YAxisSensitivity);
		Pointer &xaxisPtr = SettingsPtr(GameGlobals::GameSettings::XAxisSensitivity);
		Pointer &yaxisPtrVehicle = SettingsPtr(GameGlobals::GameSettings::VehicleYAxisSensitivity);
		Pointer &xaxisPtrVehicle = SettingsPtr(GameGlobals::GameSettings::VehicleXAxisSensitivity);
		uint32_t isInverted = invertedPtr.Read<uint32_t>();
		float yaxisSens = (float)yaxisPtr.Read<uint32_t>() / 25.f;
		float xaxisSens = (float)xaxisPtr.Read<uint32_t>() / 25.f;

		typedef int(__cdecl *Game_GetLocalPlayerIdxFunc)(int playerNum);
		Game_GetLocalPlayerIdxFunc Game_GetLocalPlayerIdx = (Game_GetLocalPlayerIdxFunc)0x589C30;
		uint16_t playerIdx = (uint16_t)Game_GetLocalPlayerIdx(0); // we only need the first 16 bits of this

		Pointer PlayerData = ElDorito::GetMainTls(GameGlobals::PlayerAlt::TLSOffset)[0];
		Pointer vehicleData = Pointer(PlayerData(GameGlobals::PlayerAlt::VehicleData + (playerIdx * GameGlobals::PlayerAlt::PlayerObjectSize)).Read<uint32_t>());
		char isInVehicle = vehicleData(GameGlobals::PlayerAlt::VehicleDataIsInVehicle).Read<char>();

		if (isInVehicle != 0)
		{
			yaxisSens = (float)yaxisPtrVehicle.Read<uint32_t>() / 25.f;
			xaxisSens = (float)xaxisPtrVehicle.Read<uint32_t>() / 25.f;
		}

		if (isInverted != 0)
			currentVert += (float)rwInput->data.mouse.lLastY * yaxisSens / 1000.0f / weaponSensitivity;
		else
			currentVert -= (float)rwInput->data.mouse.lLastY * yaxisSens / 1000.0f / weaponSensitivity;

		if (currentVert > maxVertAngle)
			currentVert = maxVertAngle;
		else if (currentVert < (0.f - maxVertAngle))
			currentVert = (0.f - maxVertAngle);

		currentHoriz -= (float)rwInput->data.mouse.lLastX * xaxisSens / 1000.0f / weaponSensitivity;

		horizPtr.Write(currentHoriz);
		vertPtr.Write(currentVert);
		return true;
	}

	__declspec(naked) void RawInputHook()
	{
		__asm
		{
			lea eax, [ebp - 0x2C] // Address of RAWINPUT structure
			push eax
			call RawInputHookImpl
			add esp, 4
			test al, al
			jz disabled

			// Skip past the game's mouse handling code
			mov ecx, [ebp - 0x18]
			push 0x5123AA
			ret

		disabled:
			// Run the game's mouse handling code
			mov eax, [ebp - 0x10]
			add ds:[0x238E6C0], eax
			push 0x51239E
			ret
		}
	}
}