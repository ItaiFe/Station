#include "LedColor.h"

static const Rgb PALETTE[4] = {
    {255, 0, 0},    // red
    {0, 255, 0},    // green
    {0, 0, 255},    // blue
    {180, 180, 0},  // yellow at ~70%
};
static const uint8_t WHITE_BIT = 1u << 4;

Rgb maskColor(uint8_t mask) {
    if (mask == 0) return LED_IDLE;

    unsigned r = 0, g = 0, b = 0, count = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (mask & (1u << i)) {
            r += PALETTE[i].r;
            g += PALETTE[i].g;
            b += PALETTE[i].b;
            count++;
        }
    }
    const bool white = mask & WHITE_BIT;
    if (count == 0) return white ? Rgb{191, 191, 191} : LED_IDLE;

    Rgb c = {uint8_t(r / count), uint8_t(g / count), uint8_t(b / count)};
    if (white) {
        c.r = (c.r * 7 + 255 * 3) / 10;
        c.g = (c.g * 7 + 255 * 3) / 10;
        c.b = (c.b * 7 + 255 * 3) / 10;
    }
    return c;
}

uint8_t rainbowHue(uint16_t index, uint16_t count, uint32_t nowMs) {
    const uint32_t spread = count ? uint32_t(index) * 256 / count : 0;
    return uint8_t(spread + nowMs / 8);
}
