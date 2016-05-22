#include "Rectangle.hpp"
#include <algorithm>

using namespace Utils;

bool Rectangle::IsEmpty() const
{
	return Width == 0 || Height == 0;
}

bool Rectangle::Intersects(const Rectangle &other) const
{
	if (IsEmpty() || other.IsEmpty())
		return false;
	auto right = X + Width;
	auto bottom = Y + Height;
	auto otherRight = other.X + other.Width;
	auto otherBottom = other.Y + other.Height;
	return (X < otherRight && right > other.X && Y < otherBottom && bottom > other.Y);
}

Rectangle Rectangle::Intersect(const Rectangle &other) const
{
	if (IsEmpty() || other.IsEmpty())
		return Rectangle();
	
	Rectangle result;
	result.X = std::max(X, other.X);
	result.Y = std::max(Y, other.Y);
	auto right = std::min(X + Width, other.X + other.Width);
	auto bottom = std::min(Y + Height, other.Y + other.Height);
	if (right <= result.X || bottom <= result.Y)
		return Rectangle();
	result.Width = right - result.X;
	result.Height = bottom - result.Y;
	return result;
}

Rectangle Rectangle::Translate(int x, int y) const
{
	return Rectangle(X + x, Y + y, Width, Height);
}

Rectangle Rectangle::Add(int x, int y, int width, int height) const
{
	return Add(Rectangle(x, y, width, height));
}

Rectangle Rectangle::Add(const Rectangle &other) const
{
	if (IsEmpty())
		return other;
	if (other.IsEmpty())
		return *this;

	Rectangle result;
	result.X = std::min(X, other.X);
	result.Y = std::min(Y, other.Y);
	auto right = std::max(X + Width, other.X + other.Width);
	auto bottom = std::max(Y + Height, other.Y + other.Height);
	result.Width = right - result.X;
	result.Height = bottom - result.Y;
	return result;
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
