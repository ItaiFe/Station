#include <Arduino.h>
#include "Debouncer.h"
#include "Protocol.h"
#include "Streamer.h"
#include "config.h"
#include "network.h"
#include "ota.h"
#include "stats.h"
#include "web.h"

StationStats stats;

static Debouncer debouncer(DEBOUNCE_MS);
static Streamer streamer(STREAM_INTERVAL_MS, RELEASE_REPEATS);
static uint32_t lastSampleMs = 0;
static uint8_t physicalMask = 0;
static bool servicesStarted = false;

// Web (and OTA) need a live network; start them on the first connection.
static void serviceLoop() {
    if (!servicesStarted && network_connected()) {
        ota_begin();
        web_begin();
        servicesStarted = true;
    }
    if (servicesStarted) {
        ota_handle();
        web_handle();
    }
}

static uint8_t readRawButtons() {
    uint8_t raw = 0;
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        if (digitalRead(BUTTON_PINS[i]) == LOW) raw |= 1u << i;
    }
    return raw;
}

static void printMask(const char* prefix, uint8_t mask) {
    Serial.printf("%s 0x%02X", prefix, mask);
    if (mask == 0) Serial.print(" released");
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        if (mask & (1u << i)) Serial.printf(" %s", BUTTON_NAMES[i]);
    }
    Serial.println();
}

static void updatePhysical(uint8_t mask) {
    if (mask == physicalMask) return;
    stats.pressCount += __builtin_popcount(mask & ~physicalMask);
    physicalMask = mask;
    printMask("Buttons:", mask);
}

static void streamMask(uint32_t nowMs) {
    uint8_t mask;
    if (!streamer.tick(stats.currentMask, nowMs, &mask)) return;
    uint8_t packet[PACKET_SIZE];
    encodePacket(STATION_ID, mask, packet);
    if (network_send(packet, PACKET_SIZE)) stats.packetsSent++;
}

void setup() {
    Serial.begin(115200);
    Serial.printf("\n=== %s (id %d) firmware %s ===\n", STATION_NAME, STATION_ID, FIRMWARE_VERSION);
    for (uint8_t pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);
    network_begin();
}

void loop() {
    const uint32_t now = millis();
    network_loop(now);
    serviceLoop();

    if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
        lastSampleMs = now;
        updatePhysical(debouncer.update(readRawButtons(), now));
    }

    stats.currentMask = physicalMask | web_simulated_mask(now);
    streamMask(now);
    delay(1);
}
