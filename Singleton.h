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
protected:
	Singleton()
	{
	};
private:
	Singleton(Singleton const&)
	{
	};//Prevent copying
	void operator=(Singleton const&)
	{
	}; //prevent assignment
};
