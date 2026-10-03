#include <unity.h>
#include "LedColor.h"

void setUp() {}
void tearDown() {}

static void assertRgb(uint8_t r, uint8_t g, uint8_t b, Rgb c) {
    TEST_ASSERT_EQUAL_UINT8(r, c.r);
    TEST_ASSERT_EQUAL_UINT8(g, c.g);
    TEST_ASSERT_EQUAL_UINT8(b, c.b);
}

void test_idle_is_dim_glow() {
    assertRgb(LED_IDLE.r, LED_IDLE.g, LED_IDLE.b, maskColor(0x00));
}

// Palette matches the Flamingo's getBlendedColorFromPackets().
void test_single_buttons() {
    assertRgb(255, 0, 0, maskColor(0x01));
    assertRgb(0, 255, 0, maskColor(0x02));
    assertRgb(0, 0, 255, maskColor(0x04));
    assertRgb(180, 180, 0, maskColor(0x08));
}

void test_white_alone() {
    assertRgb(191, 191, 191, maskColor(0x10));
}

void test_colors_are_averaged() {
    assertRgb(127, 0, 127, maskColor(0x05));            // red + blue
    assertRgb(108, 108, 63, maskColor(0x0F));           // red+green+blue+yellow
}

void test_white_lightens_mix() {
    assertRgb(255, 76, 76, maskColor(0x11));            // red + white: 70% red + 30% white
}

void test_rainbow_spreads_across_strip() {
    TEST_ASSERT_EQUAL_UINT8(0, rainbowHue(0, 30, 0));
    TEST_ASSERT_EQUAL_UINT8(128, rainbowHue(15, 30, 0));
}

void test_rainbow_moves_and_wraps() {
    TEST_ASSERT_EQUAL_UINT8(1, rainbowHue(0, 30, 8));
    TEST_ASSERT_EQUAL_UINT8(0, rainbowHue(0, 30, 256 * 8));
    TEST_ASSERT_EQUAL_UINT8(rainbowHue(0, 30, 0xFFFFFFFFu) + 0, rainbowHue(0, 30, 0xFFFFFFFFu));
}

void test_rainbow_zero_count_is_safe() {
    TEST_ASSERT_EQUAL_UINT8(2, rainbowHue(0, 0, 16));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_idle_is_dim_glow);
    RUN_TEST(test_single_buttons);
    RUN_TEST(test_white_alone);
    RUN_TEST(test_colors_are_averaged);
    RUN_TEST(test_white_lightens_mix);
    RUN_TEST(test_rainbow_spreads_across_strip);
    RUN_TEST(test_rainbow_moves_and_wraps);
    RUN_TEST(test_rainbow_zero_count_is_safe);
    return UNITY_END();
}
