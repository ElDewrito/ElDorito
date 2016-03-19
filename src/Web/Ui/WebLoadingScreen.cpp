#include "WebLoadingScreen.hpp"
#include "../../Patches/LoadingScreen.hpp"
#include "ScreenLayer.hpp"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../ThirdParty/rapidjson/writer.h"

using namespace Web::Ui;

namespace
{
	class WebLoadingScreenUi : public Patches::LoadingScreen::LoadingScreenUi
	{
		uint32_t currentBytes;
		uint32_t totalBytes;

	public:
		WebLoadingScreenUi() : currentBytes(0), totalBytes(0) { }

		void Show(const std::string &mapPath) override;
		void Begin(uint32_t totalBytes) override;
		void UpdateProgress(uint32_t bytes) override;
		void Hide() override;
	};
}

namespace Web
{
	namespace Ui
	{
		namespace WebLoadingScreen
		{
			void Init()
			{
				auto ui = std::make_shared<WebLoadingScreenUi>();
				Patches::LoadingScreen::SetUi(ui);
			}
		}
	}
}

namespace
{
	void WebLoadingScreenUi::Show(const std::string &mapPath)
	{
		// The map path is in the form "maps\\<name>", so strip the folder from the beginning to get just the name
		auto separatorIndex = mapPath.find_first_of("\\/");
		auto mapName = mapPath.substr(separatorIndex + 1);

		// Build a JSON object with the map name
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("map");
		jsonWriter.String(mapName.c_str());
		jsonWriter.EndObject();

		// Show the "loading" screen
		ScreenLayer::Show("loading", jsonBuffer.GetString());
	}

	void WebLoadingScreenUi::Begin(uint32_t totalBytes)
	{
		currentBytes = 0;
		this->totalBytes = totalBytes;
	}

	void WebLoadingScreenUi::UpdateProgress(uint32_t bytes)
	{
		currentBytes += bytes;

		// Build a JSON object with the progress data
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("currentBytes");
		jsonWriter.Int64(currentBytes);
		jsonWriter.Key("totalBytes");
		jsonWriter.Int64(totalBytes);
		jsonWriter.EndObject();

		// Send a loadprogress event to visible screens only
		ScreenLayer::Notify("loadprogress", jsonBuffer.GetString(), false);
	}

	void WebLoadingScreenUi::Hide()
	{
		ScreenLayer::Hide("loading");
	}
}