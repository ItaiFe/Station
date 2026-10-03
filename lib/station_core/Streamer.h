#pragma once
#include <stdint.h>

// Decides when to send the button mask to the Flamingo:
// - non-zero mask: immediately on change, then every intervalMs
// - mask drops to zero: send 0 releaseRepeats times, intervalMs apart
// - otherwise idle: send nothing
class Streamer {
public:
    Streamer(uint32_t intervalMs, uint8_t releaseRepeats)
        : intervalMs_(intervalMs), releaseRepeats_(releaseRepeats) {}

    // Call every loop. Returns true when a packet should be sent now and
    // writes the mask to send into *out.
    bool tick(uint8_t mask, uint32_t nowMs, uint8_t* out);

private:
    uint32_t intervalMs_;
    uint8_t releaseRepeats_;
    uint8_t lastMask_ = 0;
    uint32_t lastSendMs_ = 0;
    uint8_t releaseLeft_ = 0;
};
