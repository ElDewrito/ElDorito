#pragma once

#include <cstdint>

namespace Utils
{
	// A rectangle.
	struct Rectangle
	{
		// Constructs an empty rectangle.
		Rectangle() :
			X(0), Y(0), Width(0), Height(0)
		{
		}

		// Constructs a new rectangle.
		Rectangle(int x, int y, int width, int height) :
			X(x), Y(y), Width(width), Height(height)
		{
		}

		int X;
		int Y;
		int Width;
		int Height;

		// Sets all of the rectangle's values to 0.
		void Reset();

		// Returns true if the rectangle has a width or a height of 0.
		bool IsEmpty() const;

		// Adds another rectangle to the rectangle.
		void Add(int x, int y, int width, int height);

		// Adds another rectangle to the rectangle.
		void Add(const Rectangle &other);

		// Copies a rectangle of data from a source buffer to a destination buffer.
		static void Copy(void *dest, int destX, int destY, uint32_t destStride, const void *source, const Rectangle &sourceRect, uint32_t sourceStride, uint32_t elementSize);
	};
}
