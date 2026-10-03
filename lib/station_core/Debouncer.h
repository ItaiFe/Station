#pragma once
#include <stdint.h>

// Per-bit time debouncer: a bit's stable value changes only after its raw
// value has held steady for debounceMs. Wraparound-safe (unsigned math).
class Debouncer {
public:
    explicit Debouncer(uint32_t debounceMs) : debounceMs_(debounceMs) {}

    // raw: bit i set = button i currently reads pressed. Returns stable mask.
    uint8_t update(uint8_t raw, uint32_t nowMs);
    uint8_t stable() const { return stable_; }

private:
    uint32_t debounceMs_;
    uint8_t stable_ = 0;
    uint8_t lastRaw_ = 0;
    uint32_t lastChangeMs_[8] = {};
};
