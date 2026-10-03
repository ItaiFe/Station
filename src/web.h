#pragma once
#include <stdint.h>

void web_begin();
void web_handle();
// Mask currently held by POST /press (0 when no hold is active).
uint8_t web_simulated_mask(uint32_t nowMs);
