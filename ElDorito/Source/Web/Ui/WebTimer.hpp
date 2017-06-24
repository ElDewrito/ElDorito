#pragma once
#include <string>

namespace Web
{
	namespace Ui
	{
		namespace WebTimer
		{
			void Start(const std::string& type, int initialValue);
			void Update(int value);
			void End();
		}
	}
}
