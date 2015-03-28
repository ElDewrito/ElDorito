#pragma once
#include <iostream>
#include <string>

unsigned int GetMainThreadId();
void* GetModuleBase();
void* GetModuleBase(const std::string& ModuleName);
inline void* GetBasePointer()
{
	return GetModuleBase();
}

class Pointer
{
public:
	Pointer() : _Pointer(nullptr)
	{
	}
	Pointer(void* Pointer) : _Pointer(Pointer)
	{
	}
	Pointer(unsigned int Offset) : _Pointer((void*)Offset)
	{
	}
	~Pointer()
	{
	}

	//Resolves offsets the current pointer by the offset and picks up the 4 byte integer value
	// at the new location and returns it as a new pointer object
	inline Pointer operator [](const unsigned int Offset) const
	{
		if( _Pointer )
		{
			if( (unsigned int*)((char*)_Pointer + Offset) )
			{
				return Pointer(*(unsigned int*)((char*)_Pointer + Offset));
			}
		}
		return Pointer(nullptr);
	}

	template <typename T>
	inline T* operator ->() const
	{
		return (T*)_Pointer;
	}

	template <typename T>
	inline T as() const
	{
		if( _Pointer )
		{
			return *(T*)_Pointer;
		}
		return T();
	}

	//Returns the position of the pointer offset by a number of bytes(or stride value)
	inline Pointer operator() (unsigned int Offset = 0, unsigned int Stride = 1) const
	{
		return Pointer((unsigned char*)_Pointer + (Offset*Stride));
	}

	//Assignment
	template <class T>
	inline void operator= (T* Pointer)
	{
		_Pointer = (void*)Pointer;
	}

	inline void operator= (Pointer Pointer)
	{
		_Pointer = Pointer._Pointer;
	}

	//Implicit cast to any other pointer type
	template <class T>
	inline operator T* () const
	{
		return (T*)_Pointer;
	}

	inline operator bool() const
	{
		return _Pointer != nullptr ? true : false;
	}

	inline unsigned int asUint() const
	{
		return as<unsigned int>();
	}

	inline unsigned short asUshort() const
	{
		return as<unsigned short>();
	}

	inline unsigned char asUchar() const
	{
		return as<unsigned char>();
	}

	inline void Set(unsigned int Pointer)
	{
		_Pointer = (void*)Pointer;
	}

private:
	void* _Pointer;
};