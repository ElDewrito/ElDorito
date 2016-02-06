#pragma once
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct ModelAnimationGraph : Tag<'jmad'>
		{
			struct TagBlock0;
			struct TagBlock1;
			struct TagBlock2;
			struct TagBlock3;
			struct Animation;
			struct TagBlock10;
			struct TagBlock26;
			struct TagBlock27;
			struct TagBlock28;
			struct TagBlock31;
			struct TagBlock32;

			TagReference Unknown0;
			int32_t Unknown10;
			TagBlock<TagBlock0> Unknown14;
			TagBlock<TagBlock1> Unknown20;
			TagBlock<TagBlock2> Unknown2c;
			TagBlock<TagBlock3> Unknown38;
			int32_t Unknown44;
			int32_t Unknown48;
			int32_t Unknown4c;
			TagBlock<Animation> Animations;
			TagBlock<TagBlock10> Unknown5c;
			TagBlock<TagBlock26> Unknown68;
			TagBlock<TagBlock27> Unknown74;
			TagBlock<TagBlock28> Unknown80;
			TagBlock<TagBlock31> Unknown8c;
			int32_t Unknown98;
			int32_t Unknown9c;
			int32_t UnknownA0;
			int32_t UnknownA4;
			int32_t UnknownA8;
			int32_t UnknownAc;
			int32_t UnknownB0;
			int32_t UnknownB4;
			int32_t UnknownB8;
			int32_t UnknownBc;
			int32_t UnknownC0;
			int32_t UnknownC4;
			int32_t UnknownC8;
			int32_t UnknownCc;
			int32_t UnknownD0;
			int32_t UnknownD4;
			int32_t UnknownD8;
			int32_t UnknownDc;
			int32_t UnknownE0;
			int32_t UnknownE4;
			int32_t UnknownE8;
			int32_t UnknownEc;
			int32_t UnknownF0;
			int32_t UnknownF4;
			TagBlock<TagBlock32> UnknownF8;

			struct TagBlock0
			{
				int32_t Unknown0;
				int32_t Unknown4;
				int32_t Unknown8;
				int32_t UnknownC;
				int32_t Unknown10;
				int32_t Unknown14;
				int32_t Unknown18;
				int32_t Unknown1c;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock0, 0x20);

			struct TagBlock1
			{
				TagReference Unknown0;
				int32_t Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock1, 0x14);

			struct TagBlock2
			{
				TagReference Unknown0;
				int32_t Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock2, 0x14);

			struct TagBlock3
			{
				int32_t Unknown0;
				int32_t Unknown4;
				int32_t Unknown8;
				int32_t UnknownC;
				int32_t Unknown10;
				int32_t Unknown14;
				int32_t Unknown18;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock3, 0x1C);

			struct Animation
			{
				struct TagBlock5;
				struct TagBlock6;
				struct TagBlock7;
				struct TagBlock8;
				struct TagBlock9;

				int32_t Unknown0;
				int32_t Unknown4;
				int32_t Unknown8;
				int32_t UnknownC;
				int16_t FrameCount;
				int32_t Unknown14;
				int32_t Unknown18;
				int32_t Unknown1c;
				int32_t Unknown20;
				int32_t Unknown24;
				int32_t Unknown28;
				TagBlock<TagBlock5> Unknown2c;
				TagBlock<TagBlock6> Unknown38;
				TagBlock<TagBlock7> Unknown44;
				TagBlock<TagBlock8> Unknown50;
				TagBlock<TagBlock9> Unknown5c;
				int32_t Unknown68;
				int32_t Unknown6c;
				int32_t Unknown70;
				int32_t Unknown74;
				int32_t Unknown78;
				int32_t Unknown7c;
				int32_t Unknown80;
				int32_t Unknown84;

				struct TagBlock5
				{
					int32_t Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock5, 0x4);

				struct TagBlock6
				{
					int32_t Unknown0;
					int32_t Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock6, 0x8);

				struct TagBlock7
				{
					int32_t Unknown0;
					int32_t Unknown4;
					int32_t Unknown8;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock7, 0xC);

				struct TagBlock8
				{
					int32_t Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock8, 0x4);

				struct TagBlock9
				{
					int32_t Unknown0;
					int32_t Unknown4;
					int32_t Unknown8;
					int32_t UnknownC;
					int32_t Unknown10;
					int32_t Unknown14;
					int32_t Unknown18;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock9, 0x1C);
			};
			TAG_STRUCT_SIZE_ASSERT(Animation, 0x88);

			struct TagBlock10
			{
				struct TagBlock11;
				struct TagBlock25;

				int32_t Unknown0;
				TagBlock<TagBlock11> Unknown4;
				TagBlock<TagBlock25> Unknown10;
				int32_t Unknown1c;
				int32_t Unknown20;
				int32_t Unknown24;

				struct TagBlock11
				{
					struct TagBlock12;
					struct TagBlock20;
					struct TagBlock21;

					int32_t Unknown0;
					TagBlock<TagBlock12> Unknown4;
					TagBlock<TagBlock20> Unknown10;
					TagBlock<TagBlock21> Unknown1c;

					struct TagBlock12
					{
						struct TagBlock13;
						struct TagBlock14;
						struct TagBlock15;
						struct TagBlock18;

						int32_t Unknown0;
						TagBlock<TagBlock13> Unknown4;
						TagBlock<TagBlock14> Unknown10;
						TagBlock<TagBlock15> Unknown1c;
						TagBlock<TagBlock18> Unknown28;

						struct TagBlock13
						{
							int32_t Unknown0;
							int32_t Unknown4;
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock13, 0x8);

						struct TagBlock14
						{
							int32_t Unknown0;
							int32_t Unknown4;
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock14, 0x8);

						struct TagBlock15
						{
							struct TagBlock16;

							int32_t Unknown0;
							TagBlock<TagBlock16> Unknown4;

							struct TagBlock16
							{
								struct TagBlock17;

								TagBlock<TagBlock17> Unknown0;

								struct TagBlock17
								{
									int32_t Unknown0;
								};
								TAG_STRUCT_SIZE_ASSERT(TagBlock17, 0x4);
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock16, 0xC);
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock15, 0x10);

						struct TagBlock18
						{
							struct TagBlock19;

							int32_t Unknown0;
							int32_t Unknown4;
							int32_t Unknown8;
							TagBlock<TagBlock19> UnknownC;

							struct TagBlock19
							{
								int32_t Unknown0;
								int32_t Unknown4;
								int32_t Unknown8;
								int32_t UnknownC;
								int32_t Unknown10;
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock19, 0x14);
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock18, 0x18);
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock12, 0x34);

					struct TagBlock20
					{
						int32_t Unknown0;
						int32_t Unknown4;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock20, 0x8);

					struct TagBlock21
					{
						struct TagBlock22;

						int32_t Unknown0;
						TagBlock<TagBlock22> Unknown4;

						struct TagBlock22
						{
							struct TagBlock23;
							struct TagBlock24;

							int32_t Unknown0;
							TagBlock<TagBlock23> Unknown4;
							TagBlock<TagBlock24> Unknown10;

							struct TagBlock23
							{
								int32_t Unknown0;
								int32_t Unknown4;
								int32_t Unknown8;
								int32_t UnknownC;
								int32_t Unknown10;
								int32_t Unknown14;
								int32_t Unknown18;
								int32_t Unknown1c;
								int32_t Unknown20;
								int32_t Unknown24;
								int32_t Unknown28;
								int32_t Unknown2c;
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock23, 0x30);

							struct TagBlock24
							{
								int32_t Unknown0;
								TagReference Unknown4;
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock24, 0x14);
						};
						TAG_STRUCT_SIZE_ASSERT(TagBlock22, 0x1C);
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock21, 0x10);
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock11, 0x28);

				struct TagBlock25
				{
					int32_t Unknown0;
					int32_t Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock25, 0x8);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock10, 0x28);

			struct TagBlock26
			{
				int32_t Unknown0;
				int32_t Unknown4;
				int32_t Unknown8;
				int32_t UnknownC;
				int32_t Unknown10;
				int32_t Unknown14;
				int32_t Unknown18;
				int32_t Unknown1c;
				int32_t Unknown20;
				int32_t Unknown24;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock26, 0x28);

			struct TagBlock27
			{
				int32_t Unknown0;
				int32_t Unknown4;
				int32_t Unknown8;
				int32_t UnknownC;
				int32_t Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock27, 0x14);

			struct TagBlock28
			{
				struct TagBlock29;
				struct TagBlock30;

				TagReference Unknown0;
				TagBlock<TagBlock29> Unknown10;
				TagBlock<TagBlock30> Unknown1c;
				int32_t Unknown28;
				int32_t Unknown2c;

				struct TagBlock29
				{
					int16_t Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock29, 0x2);

				struct TagBlock30
				{
					int32_t Unknown0;
				};
				TAG_STRUCT_SIZE_ASSERT(TagBlock30, 0x4);
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock28, 0x30);

			struct TagBlock31
			{
				int32_t Unknown0;
				int32_t Unknown4;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock31, 0x8);

			struct TagBlock32
			{
				int32_t Unknown0;
				void* Unknown4;
				int32_t Unknown8;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock32, 0xC);
		};
		TAG_STRUCT_SIZE_ASSERT(ModelAnimationGraph, 0x104);
	}
}