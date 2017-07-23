#pragma once
#include <string>

namespace Web::Ui::WebTimer
{
	void Start(const std::string& type, int initialValue);
	void Update(int value);
	void End();
}
