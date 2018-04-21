#include "WebLoadingScreen.hpp"
#include "../../Patches/LoadingScreen.hpp"
#include "ScreenLayer.hpp"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../ThirdParty/rapidjson/writer.h"
#include <chrono>

using namespace Web::Ui;

namespace
{
	const auto kUpdateRatMs = 75;

	class WebLoadingScreenUi : public Patches::LoadingScreen::LoadingScreenUi
	{
		uint32_t currentBytes;
		uint32_t totalBytes;
		std::chrono::time_point<std::chrono::steady_clock> lastUpdateTime;

	public:
		WebLoadingScreenUi() : currentBytes(0), totalBytes(0) { }

		void Show(const std::string &mapPath) override;
		void Begin(uint32_t totalBytes) override;
		void UpdateProgress(uint32_t bytes) override;
		void Hide() override;
	};
}

namespace Web::Ui::WebLoadingScreen
{
	void Init()
	{
		auto ui = std::make_shared<WebLoadingScreenUi>();
		Patches::LoadingScreen::SetUi(ui);
	}
}

namespace
{
	void WebLoadingScreenUi::Show(const std::string &mapPath)
	{
		// The map path is in the form "maps\\<name>", so strip the folder from the beginning to get just the name
		auto separatorIndex = mapPath.find_last_of("\\/");
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
		lastUpdateTime = std::chrono::steady_clock::now();
	}

	void WebLoadingScreenUi::UpdateProgress(uint32_t bytes)
	{
		currentBytes += bytes;

		auto currentTime = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count();
		if (elapsed < kUpdateRatMs)
			return;

		lastUpdateTime = currentTime;

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