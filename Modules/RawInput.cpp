#include "RawInput.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"

#include <Windows.h>
#include <iostream>

bool g_enabled;
WNDPROC realProc;

LRESULT __stdcall ourWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_enabled)
		switch (message)
		{
		case WM_INPUT:
		{
			UINT uiSize = 40; // sizeof(RAWINPUT) ?
			static unsigned char lpb[40];
			RAWINPUT* rwInput;

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &uiSize, sizeof(RAWINPUTHEADER)) != -1)
			{
				rwInput = (RAWINPUT*)lpb;

				if (!rwInput->header.dwType)
				{
					Pointer InputPtr = ElDorito::GetMainTls(GameGlobals::Input::TLSOffset)[0];
					Pointer &horizPtr = InputPtr(GameGlobals::Input::HorizontalViewAngle);
					Pointer &vertPtr = InputPtr(GameGlobals::Input::VerticalViewAngle);
					float currentHoriz = horizPtr.Read<float>();
					float currentVert = vertPtr.Read<float>();
					float weaponSensitivity = Pointer::Base(0x4CDEF14).Read<float>();
					float maxVertAngle = Pointer::Base(0x14B49E4).Read<float>();

					Pointer SettingsPtr = ElDorito::GetMainTls(GameGlobals::GameSettings::TLSOffset)[0];
					if (SettingsPtr == 0)
						SettingsPtr = Pointer(0x22C0128);

					Pointer &invertedPtr = SettingsPtr(GameGlobals::GameSettings::YAxisInverted);
					Pointer &yaxisPtr = SettingsPtr(GameGlobals::GameSettings::YAxisSensitivity);
					Pointer &xaxisPtr = SettingsPtr(GameGlobals::GameSettings::XAxisSensitivity);
					uint32_t isInverted = invertedPtr.Read<uint32_t>();
					float yaxisSens = (float)yaxisPtr.Read<uint32_t>() / 25.f;
					float xaxisSens = (float)xaxisPtr.Read<uint32_t>() / 25.f;
					TODO("find a function/variable that tells us if player is in vehicle, so we can use the vehicle sensitivity params, also find out what mouse filter is meant to do");

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
				}
			}
			break;
		}
		}

	return CallWindowProc(realProc, hWnd, message, wParam, lParam);
}


RawInput::RawInput()
	:
	enabled(false),
	hookedWndProc(false),
	DisableInputPatch1(0x112398, 0x90, 6),
	DisableInputPatch2(0x1123A4, 0x90, 6)
{
	g_enabled = false;
}

RawInput::~RawInput()
{
}

std::string RawInput::Info(const std::string& Topic) const
{
	std::string Info = "Raw Input: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "\nUsage: rawinput (on|off)\n"
		"Bypasses the games mouse processing with our own RawInput implementation.";

	return Info;
}

std::string RawInput::Suggest(const std::vector<std::string>& Arguments) const
{
	if (Arguments.size() == 2)
	{
		if (Arguments[1].empty())
		{
			return (enabled ? "off" : "on");
		}
	}
	return "";
}

void RawInput::Tick(const std::chrono::duration<double>& Delta)
{
}

bool RawInput::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1].compare("off") == 0)
		{
			DisableInputPatch1.Reset();
			DisableInputPatch2.Reset();

			enabled = false;
			g_enabled = false;
			std::cout << "Raw input disabled.\n" << std::endl;

			return true;
		}
		else if (Args[1].compare("on") == 0)
		{
			if (!this->hookedWndProc)
			{
				HWND hwnd = Pointer::Base(0x159C014).Read<HWND>();
				if (hwnd != 0)
				{
					realProc = (WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)ourWndProc);
					this->hookedWndProc = true;
				}
			}

			if (this->hookedWndProc)
			{
				DisableInputPatch1.Apply();
				DisableInputPatch2.Apply();

				enabled = true;
				g_enabled = true;
				std::cout << "Raw input enabled.\n" << std::endl;
			}
			else
				std::cout << "Failed to hook wndproc - has the game loaded yet?\n" << std::endl;

			return true;
		}
	}

	return false;
}