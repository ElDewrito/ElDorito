#pragma once

namespace Utils
{
	template <typename T>
	class Singleton
	{
	public:
		static T& Instance()
		{
			static T Inst;
			return Inst;
		}

		Singleton(Singleton const&) = delete;
		Singleton& operator=(Singleton const&) = delete;
	protected:
		Singleton()
		{
		};
	};
}