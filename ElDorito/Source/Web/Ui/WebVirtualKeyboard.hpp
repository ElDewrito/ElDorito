#pragma once

#include <string>

namespace Web::Ui::WebVirtualKeyboard
{
	void Init();
	void Submit(std::wstring& value);
	void Cancel();
}
