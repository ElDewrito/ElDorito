#pragma once

#include <string>

namespace Web
{
	namespace Ui
	{
		namespace WebVirtualKeyboard
		{
			void Init();
			void Submit(std::wstring& value);
			void Cancel();
		}
	}
}
