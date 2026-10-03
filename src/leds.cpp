#include "leds.h"
#include <FastLED.h>
#include "LedColor.h"
#include "config.h"

static CRGB leds[LED_COUNT];
static uint8_t shownMask = 0xFF;  // nothing drawn yet
static uint32_t lastFrameMs = 0;

static void draw(uint8_t mask, uint32_t nowMs) {
    if (mask == ALL_BUTTONS_MASK) {
        for (uint16_t i = 0; i < LED_COUNT; i++) {
            leds[i] = CHSV(rainbowHue(i, LED_COUNT, nowMs), 255, 255);
        }
    } else {
        const Rgb c = maskColor(mask);
        fill_solid(leds, LED_COUNT, CRGB(c.r, c.g, c.b));
    }
    FastLED.show();
    shownMask = mask;
    lastFrameMs = nowMs;
}

void leds_begin() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
    FastLED.setBrightness(LED_BRIGHTNESS);
    draw(0, millis());
}

void leds_update(uint8_t mask, uint32_t nowMs) {
    // Solid colors only redraw on change; the rainbow animates at LED_FRAME_MS.
    const bool animate = mask == ALL_BUTTONS_MASK && nowMs - lastFrameMs >= LED_FRAME_MS;
    if (mask != shownMask || animate) draw(mask, nowMs);
}
