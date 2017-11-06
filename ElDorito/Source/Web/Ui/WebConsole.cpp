#include "WebConsole.hpp"
#include "ScreenLayer.hpp"
#include "../../Console.hpp"
#include "../../Patches/Input.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"

using namespace Blam::Input;

namespace
{
	class WebConsoleOutputHandler : public Console::ConsoleOutputHandler
	{
	public:
		void WriteLine(const std::string &line) override;
	};

	void OnGameInputUpdated();
}

namespace Web::Ui::WebConsole
{
	void Init()
	{
		Console::RegisterHandler(std::make_shared<WebConsoleOutputHandler>());
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
	}

	void Show()
	{
		ScreenLayer::Show("console", "{}");
	}

	void Hide()
	{
		ScreenLayer::Hide("console");
	}
}

namespace
{
	void WebConsoleOutputHandler::WriteLine(const std::string &line)
	{
		// Create a JSON object with the line
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("line");
		jsonWriter.String(line.c_str());
		jsonWriter.EndObject();

		// Broadcast a "console" event to all screens
		Web::Ui::ScreenLayer::Notify("console", jsonBuffer.GetString(), true);
	}

	void OnGameInputUpdated()
	{
		if (GetKeyTicks(eKeyCodeTilde, eInputTypeUi) == 1 || GetKeyTicks(eKeyCodeF1, eInputTypeUi) == 1)
			Web::Ui::WebConsole::Show();
	}
}