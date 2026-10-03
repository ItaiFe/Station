#include <unity.h>
#include "Debouncer.h"

void setUp() {}
void tearDown() {}

void test_press_registers_after_debounce_time() {
    Debouncer d(50);
    TEST_ASSERT_EQUAL_HEX8(0x00, d.update(0x01, 1000));
    TEST_ASSERT_EQUAL_HEX8(0x00, d.update(0x01, 1049));
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x01, 1050));
    TEST_ASSERT_EQUAL_HEX8(0x01, d.stable());
}

void test_release_registers_after_debounce_time() {
    Debouncer d(50);
    d.update(0x01, 0);
    d.update(0x01, 50);
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x00, 200));
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x00, 249));
    TEST_ASSERT_EQUAL_HEX8(0x00, d.update(0x00, 250));
}

void test_bounce_is_rejected() {
    Debouncer d(50);
    // Chatter every 10 ms for 100 ms never settles.
    for (uint32_t t = 0; t <= 100; t += 10) {
        TEST_ASSERT_EQUAL_HEX8(0x00, d.update((t / 10) % 2 ? 0x00 : 0x01, t));
    }
    // Then settles pressed at t=100 (last write above was 0x01 at t=100).
    TEST_ASSERT_EQUAL_HEX8(0x00, d.update(0x01, 149));
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x01, 150));
    // Release chatter is rejected too.
    for (uint32_t t = 200; t <= 300; t += 10) {
        TEST_ASSERT_EQUAL_HEX8(0x01, d.update((t / 10) % 2 ? 0x01 : 0x00, t));
    }
}

void test_buttons_debounce_independently() {
    Debouncer d(50);
    d.update(0x01, 0);
    d.update(0x03, 30);
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x03, 50));
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x03, 79));
    TEST_ASSERT_EQUAL_HEX8(0x03, d.update(0x03, 80));
}

void test_all_five_buttons() {
    Debouncer d(50);
    d.update(0x1F, 0);
    TEST_ASSERT_EQUAL_HEX8(0x1F, d.update(0x1F, 50));
}

void test_wraparound() {
    Debouncer d(50);
    const uint32_t start = 0xFFFFFFE0u;  // 32 ms before millis() wraps
    d.update(0x01, start);
    TEST_ASSERT_EQUAL_HEX8(0x00, d.update(0x01, start + 49));  // wraps to 0x11
    TEST_ASSERT_EQUAL_HEX8(0x01, d.update(0x01, start + 50));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_press_registers_after_debounce_time);
    RUN_TEST(test_release_registers_after_debounce_time);
    RUN_TEST(test_bounce_is_rejected);
    RUN_TEST(test_buttons_debounce_independently);
    RUN_TEST(test_all_five_buttons);
    RUN_TEST(test_wraparound);
    return UNITY_END();
}
