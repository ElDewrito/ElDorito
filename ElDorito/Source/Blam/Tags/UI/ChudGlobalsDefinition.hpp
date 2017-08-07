#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::UI
{
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct ChudGlobalsDefinition : TagGroup<'chgd'>
	{
		struct HudGlobal;
		struct HudShader;
		struct Unknown0;
		struct Unknown3;
		struct PlayerTrainingDatum;

		TagBlock<HudGlobal> HudGlobals;
		TagBlock<HudShader> HudShaders;
		TagBlock<Unknown0> Unknown2;
		TagBlock<Unknown3> Unknown4;
		TagBlock<PlayerTrainingDatum> PlayerTrainingData;
		TagReference StartMenuEmblems;
		TagReference CampaignMedals;
		TagReference CampaignMedalHudAnimation;
		int16_t Unknown5;
		int16_t Unknown6;
		float CampaignMedalScale;
		float CampaignMedalSpacing;
		float CampaignMedalOffsetX;
		float CampaignMedalOffsetY;
		float MetagameScoreboardTopY;
		float MetagameScoreboardSpacing;
		TagReference UnitDamageGrid;
		float MicroTextureTileAmount;
		float MediumSensorBlipScale;
		float SmallSensorBlipScale;
		float LargeSensorBlipScale;
		float SensorBlipGlowAmount;
		float SensorBlipGlowRadius;
		float SensorBlipGlowOpacity;
		TagReference MotionSensorBlip;
		TagReference BirthdayPartyEffect;
		TagReference CampaignFloodMask;
		TagReference CampaignFloodMaskTile;
		float Unknown7;
		float ShieldMinorThreshold;
		float ShieldMajorThreshold;
		float ShieldCriticalThreshold;
		float HealthMinorThreshold;
		float HealthMajorThreshold;
		float HealthCriticalThreshold;
		float Unknown8;
		float Unknown9;
		uint32_t Unknownundefined;
		uint32_t Unknown10;
		uint32_t Unknown11;
		uint32_t Unknown12;
		uint32_t Unknown13;
		uint32_t Unknown14;
		uint32_t Unknown15;
		uint32_t Unknown16;
		uint32_t Unknown17;
		uint32_t Unknown18;
		uint32_t Unknown19;
		uint32_t Unknown20;
		uint32_t Unknown21;
		uint32_t Unknown22;
		TagData<uint8_t> Unknown23;
		uint32_t Unknown24;
		uint32_t Unknown25;
		uint32_t Unknown26;
		uint32_t Unknown27;
		uint32_t Unknown28;
		uint32_t Unknown29;
		uint32_t Unknown30;
		uint32_t Unknown31;
		uint32_t Unknown32;
		uint32_t Unknown33;
		uint32_t Unknown34;
		uint32_t Unknown35;
		uint32_t Unknown36;
		uint32_t Unknown37;
		uint32_t Unknown38;
		uint32_t Unknown39;
		TagData<uint8_t> Unknown40;
		float SprintFovMultiplier;
		float SprintFovTransitionInTime;
		float SprintFovTransitionOutTime;
		TagReference ParallaxData;
		uint32_t Unknown41;
		TagData<uint8_t> Unknown42;
		TagData<uint8_t> Unknown43;
		TagData<uint8_t> Unknown44;
		TagData<uint8_t> Unknown45;
		TagData<uint8_t> Unknown46;
		uint32_t Unknown47;
		TagData<uint8_t> Unknown48;
		TagData<uint8_t> Unknown49;
		TagData<uint8_t> Unknown50;
		TagData<uint8_t> Unknown51;
		TagData<uint8_t> Unknown52;
		TagReference Unknown53;
		uint32_t Unknown54;
		uint32_t Unknown55;
		uint32_t Unknown56;

		struct HudGlobal
		{
			struct HudAttribute;
			struct HudSound;
			struct MultiplayerMedal;

			struct GlobalDynamicColor
			{
				uint8_t Alpha;
				uint8_t R;
				uint8_t G;
				uint8_t B;
			};

			enum BipedValue : int32_t
			{
				Spartan,
				Elite,
				Monitor,
			};

			BipedValue Biped;
			GlobalDynamicColor GlobalDynamicColors[37];
			TagBlock<HudAttribute> HudAttributes;
			TagBlock<HudSound> HudSounds;
			TagReference Unknown;
			TagReference FragGrenadeSwapSound;
			TagReference PlasmaGrenadeSwapSound;
			TagReference SpikeGrenadeSwapSound;
			TagReference FirebombGrenadeSwapSound;
			TagReference DamageMicrotexture;
			TagReference DamageNoise;
			TagReference DirectionalArrow;
			TagReference Unknown2;
			TagReference Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			TagReference Waypoints;
			TagReference Unknown10;
			TagReference ScoreboardHud;
			TagReference MetagameScoreboardHud;
			TagReference SurvivalHud;
			TagReference Unknown11;
			TagReference TheaterHud;
			TagReference ForgeHud;
			TagReference HudStrings;
			TagReference Medals;
			TagBlock<MultiplayerMedal> MultiplayerMedals;
			TagReference MedalHudAnimation;
			TagReference MedalHudAnimation2;
			TagReference CortanaChannel;
			TagReference Unknown12;
			TagReference Unknown13;
			TagReference Unknown14;
			TagReference Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			float GrenadeScematicsSpacing;
			float EquipmentScematicOffsetY;
			float DualEquipmentScematicOffsetY;
			float Unknown18;
			float Unknown19;
			float ScoreboardLeaderOffsetY;
			float Unknown20;
			float WaypointScale;
			uint32_t Unknown21;

			struct HudAttribute
			{
				uint32_t ResolutionFlags;
				float WarpAngle;
				float WarpAmount;
				float WarpDirection;
				uint32_t ResolutionWidth;
				uint32_t ResolutionHeight;
				float MotionSensorOffsetX;
				float MotionSensorOffsetY;
				float MotionSensorRadius;
				float MotionSensorScale;
				float HorizontalScale;
				float VerticalScale;
				float HorizontalStretch;
				float VerticalStretch;
				TagReference Unknown;
				TagReference Unknown2;
				TagReference FirstPersonDamageBorder;
				TagReference ThirdPersonDamageBorder;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;
				uint32_t StateCenterOffsetY;
				uint32_t Unknown10;
				uint32_t Unknown11;
				uint32_t Unknown12;
				uint32_t Unknown13;
				uint32_t Unknown14;
				uint32_t Unknown15;
				uint32_t Unknown16;
				uint32_t Unknown17;
				uint32_t NotificationScale;
				uint32_t NotificationLineSpacing;
				uint32_t Unknown18;
				uint32_t Unknown19;
				uint32_t NotificationOffsetY;
				uint32_t Unknown20;
				uint32_t Unknown21;
				uint32_t Unknown22;
				uint32_t Unknown23;
				uint32_t Unknown24;
				uint32_t Unknown25;
				uint32_t Unknown26;
			};
			TAG_STRUCT_SIZE_ASSERT(HudAttribute, 0xE8);

			struct HudSound
			{
				struct Unknown;

				uint32_t LatchedTo;
				float Scale;
				TagBlock<Unknown> Unknown2;

				struct Unknown
				{
					int32_t Biped;
					TagReference Sound;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown, 0x14);
			};
			TAG_STRUCT_SIZE_ASSERT(HudSound, 0x14);

			struct MultiplayerMedal
			{
				int32_t Medal;
			};
			TAG_STRUCT_SIZE_ASSERT(MultiplayerMedal, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(HudGlobal, 0x2B0);

		struct HudShader
		{
			TagReference VertexShader;
			TagReference PixelShader;
		};
		TAG_STRUCT_SIZE_ASSERT(HudShader, 0x20);

		struct Unknown0
		{
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown0, 0x40);

		struct Unknown3
		{
			struct Unknown2;

			uint32_t Unknown;
			TagBlock<Unknown2> Unknown3a;

			struct Unknown2
			{
				uint32_t Unknown;
				uint32_t Unknown2a;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
				uint32_t Unknown7;
				uint32_t Unknown8;
				uint32_t Unknown9;
				uint32_t Unknown10;
				uint32_t Unknown11;
				uint32_t Unknown12;
				uint32_t Unknown13;
				uint32_t Unknown14;
				uint32_t Unknown15;
				uint32_t Unknown16;
				uint32_t Unknown17;
				uint32_t Unknown18;
				uint32_t Unknown19;
				uint32_t Unknown20;
				uint32_t Unknown21;
				uint32_t Unknown22;
				uint32_t Unknown23;
				uint32_t Unknown24;
				uint32_t Unknown25;
				TagReference Sound;
				uint32_t Unknown26;
				uint32_t Unknown27;
				uint32_t Unknown28;
				uint32_t Unknown29;
				uint32_t Unknown30;
				uint32_t Unknown31;
				uint32_t Unknown32;
				uint32_t Unknown33;
				uint32_t Unknown34;
				uint32_t Unknown35;
				uint32_t Unknown36;
				uint32_t Unknown37;
				uint32_t Unknown38;
				uint32_t Unknown39;
				uint32_t Unknown40;
				uint32_t Unknown41;
				uint32_t Unknown42;
				uint32_t Unknown43;
				uint32_t Unknown44;
				uint32_t Unknown45;
				uint32_t Unknown46;
				uint32_t Unknown47;
				uint32_t Unknown48;
				uint32_t Unknown49;
				TagReference Sound2;
			};
			TAG_STRUCT_SIZE_ASSERT(Unknown2, 0xE4);
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown3, 0x10);

		struct PlayerTrainingDatum
		{
			int32_t DisplayString;
			int16_t MaxDisplayTime;
			int16_t DisplayCount;
			int16_t DisappearDelay;
			int16_t RedisplayDelay;
			float DisplayDelay;
			uint16_t Flags;
			int16_t Unknown;
		};
		TAG_STRUCT_SIZE_ASSERT(PlayerTrainingDatum, 0x14);
	};
	TAG_STRUCT_SIZE_ASSERT(ChudGlobalsDefinition, 0x2C0);
}
