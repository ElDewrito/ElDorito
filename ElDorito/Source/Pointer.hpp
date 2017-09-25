#pragma once
#include <string>
#include <stdint.h>
#include <Windows.h>
#include <sstream>

enum HookFlags : int
{
	None = 0,
	IsCall = 1,
	IsJmpIfEqual = 2,
	IsJmpIfNotEqual = 4 // unimplemented
};

size_t GetMainThreadId();
size_t GetMainThreadId(size_t ProcessId);

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
	inline Pointer operator() (size_t Offset = 0, size_t Stride = 1) const
	{
		return Pointer((unsigned char*)_Pointer + (Offset*Stride));
	}

	inline Pointer& operator+ (size_t offset)
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
	inline void Write(const T value) const
	{
		DWORD temp;
		DWORD temp2;
		if (!VirtualProtect(_Pointer, sizeof(T), PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << _Pointer;
			OutputDebugString(ss.str().c_str());
			return;
		}
		*((T*)_Pointer) = value;
		VirtualProtect(_Pointer, sizeof(T), temp, &temp2);
	}

	template <class T>
	inline void WriteFast(const T value) const
	{
		*((T*)_Pointer) = value;
	}

	inline void Write(const void* data, size_t size) const
	{
		DWORD temp;
		DWORD temp2;
		if (!VirtualProtect(_Pointer, size, PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << _Pointer;
			OutputDebugString(ss.str().c_str());
			return;
		}
		memcpy(_Pointer, data, size);
		VirtualProtect(_Pointer, size, temp, &temp2);
	}

	inline void WriteCall(void* newFunction) const
	{
		DWORD temp;
		DWORD temp2;
		uint8_t tempJMP[5] = { 0xE8, 0x90, 0x90, 0x90, 0x90 };
		uint32_t JMPSize = ((uint32_t)newFunction - (uint32_t)_Pointer - 5);
		if (!VirtualProtect(_Pointer, 5, PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << _Pointer;
			OutputDebugString(ss.str().c_str());
			return;
		}
		memcpy(&tempJMP[1], &JMPSize, 4);
		memcpy(_Pointer, tempJMP, 5);
		VirtualProtect(_Pointer, 5, temp, &temp2);
	}

	inline void WriteJump(void* newFunction, int jmpFlags) const
	{
		DWORD temp;
		DWORD temp2;
		uint8_t tempJMP[5] = { 0xE9, 0x90, 0x90, 0x90, 0x90 };
		uint8_t tempJE[6] = { 0x0F, 0x84, 0x90, 0x90, 0x90, 0x90 };
		uint32_t patchSize = (jmpFlags & HookFlags::IsJmpIfEqual) ? 6 : 5;
		uint32_t JMPSize = ((uint32_t)newFunction - (uint32_t)_Pointer - patchSize);
		if(!VirtualProtect(_Pointer, patchSize, PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << _Pointer;
			OutputDebugString(ss.str().c_str());
			return;
		}
		if (jmpFlags & HookFlags::IsJmpIfEqual)
		{
			memcpy(&tempJE[2], &JMPSize, 4);
			memcpy(_Pointer, tempJE, 6);
		}
		else
		{
			memcpy(&tempJMP[1], &JMPSize, 4);
			memcpy(_Pointer, tempJMP, 5);
		}
		VirtualProtect(_Pointer, patchSize, temp, &temp2);
	}

	// Template read
	template <class T>
	inline T& Read() const
	{
		return *((T*)_Pointer);
	}

	inline void Read(void* Dest, size_t size) const
	{
		memcpy(Dest, _Pointer, size);
	}

	// Turns a module-relative address into an absolute address
	inline static const Pointer Base(size_t Offset = 0, size_t Stride = 1)
	{
		const static Pointer ModuleBase(GetModuleBase());
		return ModuleBase(Offset, Stride);
	}

	inline static const Pointer Base(const std::string& Module, size_t Offset = 0, size_t Stride = 1)
	{
		return Pointer(GetModuleBase(Module))(Offset, Stride);
	}

	// turns an absolute address to be relative to a module
	inline static const Pointer Rebase(size_t Offset = 0)
	{
		return Pointer(Offset - Base());
	}

	inline static const Pointer Rebase(const std::string& Module, size_t Offset = 0)
	{
		return Pointer(Offset - Base(Module));
	}

private:
	void* _Pointer;
};
