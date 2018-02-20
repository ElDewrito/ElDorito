#pragma once
#include <cstdint>
#include "BlamData.hpp"
#include "Tags/Tags.hpp"

namespace Blam::Events
{
	// Event types.
	enum EventType : short
	{
		eEventTypeGeneral,
		eEventTypeFlavor,
		eEventTypeSlayer,
		eEventTypeCtf,
		eEventTypeOddball,
		eEventTypeForge,
		eEventTypeKoth,
		eEventTypeVip,
		eEventTypeJuggernaut,
		eEventTypeTerritories,
		eEventTypeAssault,
		eEventTypeInfection,
		eEventTypeSurvival,
		eEventTypeWp
	};

	// Event audiences.
	enum EventAudience : short
	{
		eEventAudienceCausePlayer,
		eEventAudienceCauseTeam,
		eEventAudienceEffectPlayer,
		eEventAudienceEffectTeam,
		eEventAudienceAll
	};

	// An event that was fired.
	struct Event
	{
		EventType Type;          // The event's type
		uint32_t NameStringId;   // A string_id representing the event name
		int Unknown8;
		int UnknownC;
		DatumHandle CausePlayer;  // The player that "caused" the event
		int CauseTeam;           // The team index that "caused" the event, or -1 for none
		DatumHandle EffectPlayer; // The player that the event "effects"
		int EffectTeam;          // The team index that the event "effects", or -1 for none
		int Unknown20;
		short Unknown24;
	};
	static_assert(sizeof(Event) == 0x28, "Invalid Event size");

	// Defines an event from a multiplayer_globals (mulg) tag.
	// Based off of Zedd's mulg plugin for Assembly.
	struct EventDefinition
	{
		uint16_t Flags;
		EventType Type;
		uint32_t NameStringId;
		EventAudience Audience;
		uint16_t UnknownA;
		short Team;
		uint16_t UnknownE;
		uint32_t MessageStringId;
		uint32_t MedalStringId;
		uint32_t Unknown18;
		uint32_t Unknown1C;
		uint16_t RequiredField;
		uint16_t ExcludedAudience;
		uint16_t RequiredField2;
		uint16_t ExcludedAudience2;
		uint32_t PrimaryStringId;
		int PrimaryStringDuration;
		uint32_t PluralDisplayStringId;
		float SoundDelay;
		uint16_t SoundFlags;
		uint16_t Unknown3A;
		Tags::TagReference LanguageSounds[12];
		uint32_t UnknownFC;
		uint32_t Unknown100;
		uint32_t Unknown104;
		uint32_t Unknown108;
	};
	TAG_STRUCT_SIZE_ASSERT(EventDefinition, 0x10C);
}
