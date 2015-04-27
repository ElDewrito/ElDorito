#pragma once

#include <cstdint>

typedef char* (__cdecl* tFilter) (std::uint32_t un1, std::uint32_t un2, std::uint32_t un3);

extern tFilter original_event_filter;
char* event_filter(std::uint32_t tag_id, std::uint32_t some_other_id, std::uint32_t unk);