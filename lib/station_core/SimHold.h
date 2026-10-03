#pragma once
#include <stdint.h>

// A simulated button press from the web test page, held for a fixed time.
class SimHold {
public:
    void start(uint8_t mask, uint32_t durationMs, uint32_t nowMs) {
        mask_ = mask;
        durationMs_ = durationMs;
        startMs_ = nowMs;
    }

    uint8_t mask(uint32_t nowMs) const {
        return nowMs - startMs_ < durationMs_ ? mask_ : 0;
    }

private:
    uint8_t mask_ = 0;
    uint32_t startMs_ = 0;
    uint32_t durationMs_ = 0;
};
