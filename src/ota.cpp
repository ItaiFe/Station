#include "ota.h"
#include <ArduinoOTA.h>
#include "config.h"

void ota_begin() {
    ArduinoOTA.setHostname(STATION_NAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart([] { Serial.println("OTA start"); });
    ArduinoOTA.onEnd([] { Serial.println("\nOTA done, rebooting"); });
    ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
        Serial.printf("OTA %u%%\r", done * 100 / total);
    });
    ArduinoOTA.onError([](ota_error_t error) { Serial.printf("OTA error %u\n", error); });
    ArduinoOTA.begin();
    Serial.printf("OTA ready: %s.local\n", STATION_NAME);
}

void ota_handle() { ArduinoOTA.handle(); }
