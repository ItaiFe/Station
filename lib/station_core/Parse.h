#pragma once
#include <stdint.h>

// Button mask from "0x1F", "0X1f" or decimal "31". Accepts 0..ALL_BUTTONS_MASK.
// Leaves *out untouched and returns false on anything else.
bool parseMask(const char* s, uint8_t* out);

// Decimal milliseconds. nullptr/empty -> defaultMs; 0 or non-digits -> false;
// values above maxMs are clamped to maxMs.
bool parseDurationMs(const char* s, uint32_t defaultMs, uint32_t maxMs, uint32_t* out);
