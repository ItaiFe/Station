#include <Arduino.h>
#include "config.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("\n=== %s (id %d) firmware %s ===\n", STATION_NAME, STATION_ID, FIRMWARE_VERSION);
    for (uint8_t pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);
}

void loop() {
    delay(10);
}
