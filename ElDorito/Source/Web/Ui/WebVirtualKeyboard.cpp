#include "WebVirtualKeyboard.hpp"

#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../ThirdParty/rapidjson/writer.h"

#include "ScreenLayer.hpp"
#include "../../Blam/BlamInput.hpp"
#include "../../Patches/VirtualKeyboard.hpp"
#include "../../Utils/String.hpp"

namespace
{
	void WebVirtualKeyboardHandler(Blam::Input::VirtualKeyboard* keyboard);
	Blam::Input::VirtualKeyboard* CurrentKeyboard;
}

namespace Web::Ui::WebVirtualKeyboard
{
	void Init()
	{
		Patches::VirtualKeyboard::SetKeyboardHandler(WebVirtualKeyboardHandler);
	}

	void Submit(std::wstring& value)
	{
		if (!CurrentKeyboard)
			return;

		//Strip illegal characters
		std::string illegal = "\\/:?\"<>|";
		for (auto it = value.begin(); it < value.end(); ++it)
			if (illegal.find(*it) != std::wstring::npos)
				*it = ' ';

		CurrentKeyboard->SetValue(value.c_str());
		CurrentKeyboard->Finish();
		CurrentKeyboard = nullptr;
	}

	void Cancel()
	{
		if (!CurrentKeyboard)
			return;
		CurrentKeyboard->Reset();
		CurrentKeyboard = nullptr;
	}
}

namespace
{
	void WebVirtualKeyboardHandler(Blam::Input::VirtualKeyboard* keyboard)
	{
		CurrentKeyboard = keyboard;
		auto title = Utils::String::ThinString(CurrentKeyboard->Title);
		auto description = Utils::String::ThinString(CurrentKeyboard->Description);
		auto defaultValue = Utils::String::ThinString(CurrentKeyboard->DefaultValue);

		// Screen parameters
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("title");
		jsonWriter.String(title.c_str());
		jsonWriter.Key("description");
		jsonWriter.String(description.c_str());
		jsonWriter.Key("defaultValue");
		jsonWriter.String(defaultValue.c_str());
		jsonWriter.EndObject();

		CurrentKeyboard->Start();
		Web::Ui::ScreenLayer::Show("keyboard", jsonBuffer.GetString());
	}
}