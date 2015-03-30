#pragma once
#include <string>
#include <stdint.h>

unsigned int GetMainThreadId();
void* GetModuleBase();
void* GetModuleBase(const std::string& ModuleName);
inline void* GetBasePointer()
{
	return GetModuleBase();
}

// Some pages require changes to VirtualProtect.
// Use this to change to flags such as:
//		PAGE_EXECUTE_READWRITE
void SetPageAccess(size_t Offset, size_t Size, size_t AccessFlags);

class Pointer
{
public:
	Pointer() : _Pointer(nullptr)
	{
	}
	Pointer(void* Pointer) : _Pointer(Pointer)
	{
	}
	Pointer(size_t Offset) : _Pointer((void*)Offset)
	{
	}
	~Pointer()
	{
	}

	//Resolves offsets the current pointer by the offset and picks up the 4 byte integer value
	// at the new location and returns it as a new pointer object
	inline Pointer operator [](const ptrdiff_t Offset) const
	{
		if( _Pointer )
			if( (unsigned int*)((char*)_Pointer + Offset) )
				return Pointer(*(unsigned int*)((char*)_Pointer + Offset));

		return Pointer(nullptr);
	}

	template <typename T>
	inline T* operator ->() const
	{
		return (T*)_Pointer;
	}

	//Returns the position of the pointer offset by a number of bytes(or stride value)
	inline Pointer operator() (ptrdiff_t Offset = 0, size_t Stride = 1) const
	{
		return Pointer((unsigned char*)_Pointer + (Offset*Stride));
	}

	inline Pointer& operator+ (ptrdiff_t offset)
	{
		_Pointer = (uint8_t*)_Pointer + offset;
		return *this;
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

	//implicit conversion
	operator void*() const
	{
		return _Pointer;
	}

	inline operator bool() const
	{
		return _Pointer != nullptr ? true : false;
	}

	// Template write
	template <class T>
	inline void Write(const T value)
	{
		*((T*)_Pointer) = value;
	}

	inline void Write(const void* data, size_t size)
	{
		memcpy(_Pointer, data, size);
	}

	inline void WriteCall(void* newFunction)
	{
		uint8_t tempJMP[5] = { 0xE8, 0x90, 0x90, 0x90, 0x90 };
		uint32_t JMPSize = ((uint32_t)newFunction - (uint32_t)_Pointer - 5);
		memcpy(&tempJMP[1], &JMPSize, 4);
		memcpy(_Pointer, tempJMP, 5);
	}

	inline void WriteJump(void* newFunction)
	{
		uint8_t tempJMP[5] = { 0xE9, 0x90, 0x90, 0x90, 0x90 };
		uint32_t JMPSize = ((uint32_t)newFunction - (uint32_t)_Pointer - 5);
		memcpy(&tempJMP[1], &JMPSize, 4);
		memcpy(_Pointer, tempJMP, 5);
	}

	// Template read
	template <class T>
	inline T& Read()
	{
		return *((T*)_Pointer);
	}

	inline void Read(void* Dest, size_t size)
	{
		memcpy(Dest, _Pointer, size);
	}

	inline static const Pointer Base()
	{
		const static Pointer ModuleBase(GetModuleBase());
		return ModuleBase;
	}

	inline static const Pointer Base(const std::string& Module)
	{
		return Pointer(GetModuleBase(Module));
	}

private:
	void* _Pointer;
};
