#pragma once

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
	void operator=(Singleton const&) = delete;
protected:
	Singleton()
	{
	};
};
