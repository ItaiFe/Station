#pragma once
#include <stdint.h>

struct Rgb {
    uint8_t r, g, b;
};

constexpr Rgb LED_IDLE = {8, 4, 0};  // dim warm glow when nothing is pressed

// Strip color for a button mask, identical to the Flamingo's
// getBlendedColorFromPackets(): red/green/blue/yellow are averaged; white alone is
// (191,191,191), white with colors lightens the average to 70% color + 30% white.
// Mask 0 -> LED_IDLE.
Rgb maskColor(uint8_t mask);

// Hue (0..255) of LED `index` in the all-buttons rainbow: spread once across
// the strip, advancing one step every 8 ms.
uint8_t rainbowHue(uint16_t index, uint16_t count, uint32_t nowMs);
