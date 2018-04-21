#pragma once
#include <cstdint>
#include <string>

namespace Web::Ui::ScreenLayer
{
	void Init();
	void Tick();
	void Resize();
	void Show(bool show);
	void Show(const std::string &screenId, const std::string &data);
	void Hide(const std::string &screenId);
	void CaptureInput(bool capture, bool pointerOnly);
	void Notify(const std::string &event, const std::string &data, bool broadcast);
	void NotifyScreen(const std::string &screenId, const std::string &event, const std::string &data);

	enum class AlertIcon
	{
		None = 0,
		Download,
		Pause,
		Upload,
		Checkbox
	};

	void ShowAlert(const std::string &title, const std::string &body, AlertIcon icon);
}
