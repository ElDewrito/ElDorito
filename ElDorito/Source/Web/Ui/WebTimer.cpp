#include "WebTimer.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include <ctime>
#include "ScreenLayer.hpp"
#include "../../Blam/BlamPlayers.hpp"
#include "../../Blam/BlamTime.hpp"
#include "../../Pointer.hpp"

namespace Web
{
	namespace Ui
	{
		void WebTimer::Start(const std::string& name, float durationSeconds)
		{
			rapidjson::StringBuffer jsonBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

			jsonWriter.StartObject();
			jsonWriter.Key("name");
			jsonWriter.String(name.c_str());
			jsonWriter.Key("startTime");
			jsonWriter.Int64(_time64(0));
			jsonWriter.Key("duration");
			jsonWriter.Double(durationSeconds);
			jsonWriter.EndObject();

			Web::Ui::ScreenLayer::Notify("timer", jsonBuffer.GetString(), true);
		}
	}
}
