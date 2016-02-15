#include "Rectangle.hpp"
#include <algorithm>

using namespace Utils;

void Rectangle::Reset()
{
	X = 0;
	Y = 0;
	Width = 0;
	Height = 0;
}

bool Rectangle::IsEmpty() const
{
	return Width == 0 || Height == 0;
}

void Rectangle::Add(int x, int y, int width, int height)
{
	Add(Rectangle(x, y, width, height));
}

void Rectangle::Add(const Rectangle &other)
{
	if (IsEmpty())
	{
		*this = other;
		return;
	}
	
	if (other.IsEmpty())
		return;

	auto right = std::max(X + Width, other.X + other.Width);
	auto bottom = std::max(Y + Height, other.Y + other.Height);
	X = std::min(X, other.X);
	Y = std::min(Y, other.Y);
	Width = right - X;
	Height = bottom - Y;
}

void Rectangle::Copy(void *dest, int destX, int destY, uint32_t destStride, const void *source, const Rectangle &sourceRect, uint32_t sourceStride, uint32_t elementSize)
{
	auto srcOffset = sourceStride * sourceRect.Y + sourceRect.X * elementSize;
	auto destOffset = destStride * destY + destX * elementSize;
	auto srcBytes = static_cast<const uint8_t*>(source) + srcOffset;
	auto destBytes = static_cast<uint8_t*>(dest) + destOffset;
	auto copyStride = sourceRect.Width * elementSize;
	if (destStride == sourceStride && destStride == copyStride)
	{
		// If the strides are the same, then the rectangle can be copied in one call
		memcpy(destBytes, srcBytes, copyStride * sourceRect.Height);
	}
	else
	{
		// Rectangle must be copied line-by-line
		for (auto i = 0; i < sourceRect.Height; i++)
		{
			memcpy(destBytes, srcBytes, copyStride);
			srcBytes += sourceStride;
			destBytes += destStride;
		}
	}
}
