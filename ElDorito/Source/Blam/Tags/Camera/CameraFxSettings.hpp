#pragma once
#include "..\Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		namespace Camera
		{
			struct FxSettings : TagGroup<'cfxs'>
			{
				struct Unknown33;
				struct Unknown35;
				struct Unknown37;
				struct Unknown39;
				struct Unknown41;
				struct Unknown43;

				uint16_t Flags;
				int16_t Unknown;
				float OverexposureAmount;
				float OverexposureUnknown;
				float OverexposureUnknown2;
				float BrightnessAmount;
				float BrightnessUnknown;
				float BrightnessUnknown2;
				float BrightnessUnknown3;
				uint16_t Flags2;
				int16_t Unknown2;
				float Unknown3;
				uint16_t Flags3;
				int16_t Unknown4;
				float Unknown5;
				uint16_t Flags4;
				int16_t Unknown6;
				float Base;
				float Min;
				float Max;
				uint16_t Flags5;
				int16_t Unknown7;
				float Base2;
				float Min2;
				float Max2;
				uint16_t Flags6;
				int16_t Unknown8;
				float Base3;
				float Min3;
				float Max3;
				uint16_t Flags7;
				int16_t Unknown9;
				float Red;
				float Green;
				float Blue;
				uint16_t Flags8;
				int16_t Unknown10;
				float Red2;
				float Green2;
				float Blue2;
				uint16_t Flags9;
				int16_t Unknown11;
				float Red3;
				float Green3;
				float Blue3;
				uint16_t Flags10;
				int16_t Unknown12;
				float Unknown13;
				float Unknown14;
				float Unknown15;
				uint16_t Flags11;
				int16_t Unknown16;
				float Unknown17;
				float Unknown18;
				float Unknown19;
				uint16_t Flags12;
				int16_t Unknown20;
				float Unknown21;
				float Unknown22;
				float Unknown23;
				int32_t Unknown24;
				uint16_t Flags13;
				int16_t Unknown25;
				float Base4;
				float Min4;
				float Max4;
				uint16_t Flags14;
				int16_t Unknown26;
				float Base5;
				float Min5;
				float Max5;
				uint16_t Flags15;
				int16_t Unknown27;
				float Unknown28;
				float Unknown29;
				float Unknown30;
				uint16_t Flags16;
				int16_t Unknown31;
				float Unknown32;
				TagBlock<Unknown33> Unknown34;
				TagBlock<Unknown35> Unknown36;
				TagBlock<Unknown37> Unknown38;
				TagBlock<Unknown39> Unknown40;
				TagBlock<Unknown41> Unknown42;
				TagBlock<Unknown43> Unknown44;
				uint16_t Flags17;
				int16_t Unknown45;
				float PositionX;
				float PositionY;
				float ColorR;
				float ColorG;
				float ColorB;
				float Unknown46;
				float Unknown47;
				float Unknown48;
				float Unknown49;
				float Unknown50;

				struct Unknown33
				{
					float Unknown;
					int32_t Unknown2;
					TagData<uint8_t> Unknown3;
					TagData<uint8_t> Unknown4;
					TagData<uint8_t> Unknown5;
					TagData<uint8_t> Unknown6;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown33, 0x58);

				struct Unknown35
				{
					int32_t Unknown;
					float Unknown2;
					float Unknown3;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown35, 0xC);

				struct Unknown37
				{
					uint32_t Unknown;
					uint32_t Unknown2;
					float Unknown3;
					uint32_t Unknown4;
					uint32_t Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown37, 0x14);

				struct Unknown39
				{
					uint32_t Unknown;
					float Unknown2;
					float Unknown3;
					float Unknown4;
					float Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown39, 0x14);

				struct Unknown41
				{
					int32_t Unknown;
					float Unknown2;
					float Unknown3;
					float Unknown4;
					uint32_t Unknown5;
					uint32_t Unknown6;
					uint32_t Unknown7;
					float Unknown8;
					float Unknown9;
					float Unknown10;
					float Unknown11;
					float Unknown12;
					float Unknown13;
					float Unknown14;
					uint32_t Unknown15;
					uint32_t Unknown16;
					float Unknown17;
					uint32_t Unknown18;
					uint32_t Unknown19;
					uint32_t Unknown20;
					uint32_t Unknown21;
					uint32_t Unknown22;
					uint32_t Unknown23;
					uint32_t Unknown24;
					uint32_t Unknown25;
					float Unknown26;
					float Unknown27;
					float Unknown28;
					uint32_t Unknown29;
					uint32_t Unknown30;
					uint32_t Unknown31;
					uint32_t Unknown32;
					float Unknown33;
					uint32_t Unknown34;
					float Unknown35;
					float Unknown36;
					float Unknown37;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown41, 0x94);

				struct Unknown43
				{
					int32_t Unknown;
					uint32_t Unknown2;
					uint32_t Unknown3;
					float Unknown4;
					float Unknown5;
					float Unknown6;
					float Unknown7;
					float Unknown8;
					uint32_t Unknown9;
					float Unknown10;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown43, 0x28);
			};
			TAG_STRUCT_SIZE_ASSERT(FxSettings, 0x170);
		}
	}
}
