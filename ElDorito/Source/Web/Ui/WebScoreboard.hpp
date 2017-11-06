#pragma once

#include <string>

namespace Web::Ui::WebScoreboard
{
	void Init();
	void Show(bool locked, bool postgame);
	void Hide();
	void Tick();
	std::string getScoreboard();
}
