#pragma once
#include "..\Tags.hpp"

namespace Blam::Tags::Sounds
{
	struct Classes : TagGroup<'sncl'>
	{
		struct Unknown;

		TagBlock<Unknown> Unknown2;
		uint32_t Unknown3;

		enum class CacheMissMode : int8_t
		{
			Discard,
			Postpone
		};

		struct Unknown
		{
			int16_t MaxSoundsPerTag;
			int16_t MaxSoundsPerObject;
			int32_t PreemptionTime;
			uint16_t InternalFlags;
			uint16_t Flags;
			int16_t Priority;
			CacheMissMode CacheMissMode;
			int8_t Unknown1;
			int8_t Unknown2;
			int8_t Unknown3;
			int8_t Unknown4;
			int8_t Unknown5;
			float ReverbGain;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			float Unknown9;
			float Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			float Unknown13;
			float Unknown14;
			float Unknown15;
			float DistanceBoundsMin;
			float DistanceBoundsMax;
			float GainBoundsMin;
			float GainBoundsMax;
			float CutsceneDucking;
			float CutsceneDuckingFadeInTime;
			float CutsceneDuckingSustain;
			float CutsceneDuckingFadeOutTime;
			float ScriptedDialogDucking;
			float ScriptedDialogDuckingFadeIn;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			uint32_t Unknown22;
			uint32_t UnknowndoplerFactor;
			int8_t StereoPlaybackType;
			int8_t Unknown23;
			int8_t Unknown24;
			int8_t Unknown25;
			float TransmissionMultiplier;
			float ObstructionMaxBend;
			float OcclusionMaxBend;
			int32_t Unknown26;
			uint32_t Unknown27;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown, 0xA0);
	};
	TAG_STRUCT_SIZE_ASSERT(Classes, 0x10);
}
