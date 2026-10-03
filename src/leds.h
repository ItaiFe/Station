#pragma once
#include <stdint.h>

// Shows the idle glow; call once in setup().
void leds_begin();
// Call every loop with the effective mask: solid blend, or rainbow for all five.
void leds_update(uint8_t mask, uint32_t nowMs);
