#pragma once
#include <stdint.h>

// Wire format shared with the Flamingo (flamingo/src/main.cpp handleUDP):
// one UDP packet = [station_id (1..4), button_mask], mask bit i = BUTTON_NAMES[i].
constexpr uint8_t BUTTON_COUNT = 5;
constexpr uint8_t ALL_BUTTONS_MASK = (1u << BUTTON_COUNT) - 1;  // Flamingo special mode
constexpr uint8_t PACKET_SIZE = 2;
constexpr const char* BUTTON_NAMES[BUTTON_COUNT] = {"red", "green", "blue", "yellow", "white"};

inline void encodePacket(uint8_t stationId, uint8_t mask, uint8_t out[PACKET_SIZE]) {
    out[0] = stationId;
    out[1] = mask;
}
