#include "ShowGameUI.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"
#include "../Utils/String.h"

#include <Windows.h>
#include <iostream>
#include <iostream>

ShowGameUI::ShowGameUI() : DialogShow(false), DialogStringId(0), DialogFlags(0), DialogParentStringId(0)
{
}

ShowGameUI::~ShowGameUI()
{
}

std::string ShowGameUI::Info(const std::string& Topic) const
{
	std::string Info = "Show Game UI dialogs/widgets [ Experimental ]"
		"\nUsage: show_ui (stringID)\n"
		"stringID should be the stringID of the UI element you want to show, in hexadecimal.\n";
	Info += "Some sample StringIDs:\n";
	Info += "10083 main_menu\n";
	Info += "10084 start_menu\n";
	return Info;
}

std::string ShowGameUI::Suggest(const std::vector<std::string>& Arguments) const
{
	if( Arguments.size() == 2 )
	{
		if( Arguments[1].empty() )
		{
			return "10083";
		}
	}
	return "";
}

void ShowGameUI::Tick(const std::chrono::duration<double>& Delta)
{
	static uint8_t UIData[0x40];
	if (DialogShow)
	{
		typedef void*(__thiscall * OpenUIDialogByIdFunc)(void* a1, unsigned int dialogStringId, int a3, int dialogFlags, int parentDialogStringId);

		// fill UIData with proper data
		OpenUIDialogByIdFunc openui = (OpenUIDialogByIdFunc)0xA92780;
		openui(&UIData, DialogStringId, 0, DialogFlags, DialogParentStringId);

		// send UI notification
		uint32_t eax = (uint32_t)&UIData;
		uint32_t ecx = *(uint32_t*)0x5260254;
		*(DWORD*)(ecx + 8) = eax;

		eax = *(uint32_t*)0x5260254;
		eax = *(uint32_t*)eax;
		*(uint32_t*)0x5260254 = eax;

		DialogShow = false;
	}
}

bool ShowGameUI::Run(const std::vector<std::string>& Args)
{
	if( Args.size() < 2 )
		return false;

	int parentDialog = 0x1000C;
	if( Args.size() >= 3 )
		parentDialog = std::stoul(Args[2], nullptr, 16);

	int openType = 0;
	if( Args.size() >= 4 )
		openType = std::stoul(Args[3], nullptr, 16);

	unsigned int stringID = std::stoul(Args[1], nullptr, 16);

	DialogStringId = stringID;
	DialogFlags = openType;
	DialogParentStringId = parentDialog;
	DialogShow = true;

	return true;
}