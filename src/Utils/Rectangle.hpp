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

		// Returns true if the rectangle has a width or a height of 0.
		bool IsEmpty() const;

		// Returns true if the rectangle intersects another rectangle.
		bool Intersects(const Rectangle &other) const;

		// Returns the intersection of this rectangle with another rectangle.
		Rectangle Intersect(const Rectangle &other) const;

		// Translates the rectangle and returns the result.
		Rectangle Translate(int x, int y) const;

		// Adds another rectangle to the rectangle and returns the result.
		Rectangle Add(int x, int y, int width, int height) const;

		// Adds another rectangle to the rectangle and returns the result.
		Rectangle Add(const Rectangle &other) const;

		// Copies a rectangle of data from a source buffer to a destination buffer.
		static void Copy(void *dest, int destX, int destY, uint32_t destStride, const void *source, const Rectangle &sourceRect, uint32_t sourceStride, uint32_t elementSize);
	};
}
