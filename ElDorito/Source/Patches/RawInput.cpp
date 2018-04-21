#include "RawInput.hpp"

#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Modules/ModuleInput.hpp"
#include "../Modules/ModuleCamera.hpp"
#include "../Forge/ForgeUtil.hpp"

namespace
{
	void RawInputHook();
}

namespace Patches::RawInput
{
	void ApplyAll()
	{
		// Hook the input handling routine to fix mouse acceleration
		Hook(0x112395, RawInputHook).Apply();
	}
}

namespace
{
	bool ShouldDisableRawInput()
	{
		using namespace Blam;

		const auto game_engine_round_in_progress = (bool(*)())(0x00550F90);
		if (game_engine_round_in_progress())
		{
			auto playerIndex = Blam::Players::GetLocalPlayer(0);
			Blam::Players::PlayerDatum *player{ nullptr };
			if (playerIndex == DatumHandle::Null || !(player = Blam::Players::GetPlayers().Get(playerIndex)))
				return false;

			// if we're in a h3 ui screen
			auto activeScreenCount = Pointer(0x05260F34)[0](0x3c).Read<int16_t>();
			if (activeScreenCount > 0)
				return true;

			// if we're rotating an object
			uint32_t heldObjectIndex;
			if (Forge::GetEditorModeState(playerIndex, &heldObjectIndex, nullptr) && heldObjectIndex != -1 && *(uint8_t*)0x238E6AC > 0)
				return true;

			// if we're spectating
			return player && player->SlaveUnit == Blam::DatumHandle::Null;
		}

		return false;
	}

	bool RawInputHookImpl(RAWINPUT *rwInput)
	{
		auto isFlying = Modules::ModuleCamera::Instance().IsFlying();

		// Disable mouse input if a controller is plugged in (this needs to be done
		// even if raw input is off). Leave it enabled while superman.
		bool controllerEnabled = Pointer::Base(0x204DE98).Read<bool>();
		if (controllerEnabled && !isFlying)
			return true;

		if (!Modules::ModuleInput::Instance().VarInputRawInput->ValueInt)
			return false;
		if (rwInput->header.dwType != RIM_TYPEMOUSE)
			return true;

		if (ShouldDisableRawInput())
			return false;

		Pointer InputPtr = ElDorito::GetMainTls(GameGlobals::Input::TLSOffset)[0];
		if (!InputPtr)
			return true;
		Pointer &horizPtr = InputPtr(GameGlobals::Input::HorizontalViewAngle);
		Pointer &vertPtr = InputPtr(GameGlobals::Input::VerticalViewAngle);
		float currentHoriz = horizPtr.Read<float>();
		float currentVert = vertPtr.Read<float>();
		float weaponSensitivity = Pointer::Base(0x4CDEF14).Read<float>();
		float maxVertAngle = Pointer::Base(0x14B49E4).Read<float>();

		if (weaponSensitivity <= 0.000099999997f)
			return true;

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

		Pointer PlayerData = ElDorito::GetMainTls(GameGlobals::PlayerAlt::TLSOffset)[0];
		if (!PlayerData)
			return true;
		Pointer vehicleData = Pointer(PlayerData(GameGlobals::PlayerAlt::VehicleData).Read<uint32_t>()); // Note: this has data for each local player, but since there's no splitscreen support yet, player index is always 0
		char isInVehicle = 0;
		if (vehicleData)
			isInVehicle = vehicleData(GameGlobals::PlayerAlt::VehicleDataIsInVehicle).Read<char>();

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

		horizPtr.WriteFast(currentHoriz);
		vertPtr.WriteFast(currentVert);
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