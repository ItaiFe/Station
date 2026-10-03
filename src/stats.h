#pragma once
#include <stdint.h>

// Counters shared between the main loop and the web status page.
struct StationStats {
    uint8_t currentMask = 0;   // effective mask (physical | simulated)
    uint32_t pressCount = 0;   // physical button presses since boot
    uint32_t packetsSent = 0;  // UDP packets handed to the network stack
};

extern StationStats stats;
