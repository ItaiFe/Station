#include "Streamer.h"

bool Streamer::tick(uint8_t mask, uint32_t nowMs, uint8_t* out) {
    const bool changed = mask != lastMask_;
    const bool due = nowMs - lastSendMs_ >= intervalMs_;

    if (mask != 0) {
        releaseLeft_ = 0;
        if (!changed && !due) return false;
    } else if (changed) {
        releaseLeft_ = releaseRepeats_;  // fresh release: burst starts now
    } else if (releaseLeft_ == 0 || !due) {
        return false;
    }

    if (mask == 0 && releaseLeft_ > 0) releaseLeft_--;
    lastMask_ = mask;
    lastSendMs_ = nowMs;
    *out = mask;
    return true;
}
