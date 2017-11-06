#pragma once
#include <cstdint>
#include "../Tags.hpp"

namespace Blam::Tags::UI
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;

	struct GfxTexturesList : TagGroup<'gfxt'>
	{
		struct Texture;

		TagBlock<Texture> Textures;
		float Unknown;

		struct Texture
		{
			char FileName[256];
			TagReference Bitmap;
		};
		TAG_STRUCT_SIZE_ASSERT(Texture, 0x110);
	};
}
