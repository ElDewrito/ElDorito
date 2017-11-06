#include "WebTimer.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include <ctime>
#include "ScreenLayer.hpp"
#include "../../Blam/BlamPlayers.hpp"
#include "../../Blam/BlamTime.hpp"
#include "../../Pointer.hpp"

namespace Web::Ui
{
	void WebTimer::Start(const std::string& type, int initialValue)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		jsonWriter.StartObject();
		jsonWriter.Key("type");
		jsonWriter.String(type.c_str());
		jsonWriter.Key("startTime");
		jsonWriter.Int64(_time64(0));
		jsonWriter.Key("value");
		jsonWriter.Int(initialValue);
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("timerStart", jsonBuffer.GetString(), true);
	}

	void WebTimer::Update(int value)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		jsonWriter.StartObject();
		jsonWriter.Key("value");
		jsonWriter.Int(value);
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("timerUpdate", jsonBuffer.GetString(), true);
	}

	void WebTimer::End()
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		Web::Ui::ScreenLayer::Notify("timerEnd", "{}", true);
	}
}
