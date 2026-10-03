#include "Debouncer.h"

uint8_t Debouncer::update(uint8_t raw, uint32_t nowMs) {
    for (uint8_t i = 0; i < 8; i++) {
        const uint8_t bit = 1u << i;
        if ((raw & bit) != (lastRaw_ & bit)) {
            lastChangeMs_[i] = nowMs;
        }
        if ((raw & bit) != (stable_ & bit) && nowMs - lastChangeMs_[i] >= debounceMs_) {
            stable_ = (stable_ & ~bit) | (raw & bit);
        }
    }
    lastRaw_ = raw;
    return stable_;
}
