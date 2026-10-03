#include <unity.h>
#include "Streamer.h"

void setUp() {}
void tearDown() {}

static int countSends(Streamer& s, uint8_t mask, uint32_t from, uint32_t to) {
    int n = 0;
    uint8_t out;
    for (uint32_t t = from; t <= to; t++) {
        if (s.tick(mask, t, &out)) {
            TEST_ASSERT_EQUAL_HEX8(mask, out);
            n++;
        }
    }
    return n;
}

void test_idle_sends_nothing() {
    Streamer s(20, 3);
    TEST_ASSERT_EQUAL_INT(0, countSends(s, 0x00, 0, 2000));
}

void test_press_sends_immediately() {
    Streamer s(20, 3);
    uint8_t out = 0xFF;
    TEST_ASSERT_TRUE(s.tick(0x01, 100, &out));
    TEST_ASSERT_EQUAL_HEX8(0x01, out);
}

void test_held_resends_every_interval() {
    Streamer s(20, 3);
    uint8_t out;
    TEST_ASSERT_TRUE(s.tick(0x01, 100, &out));
    TEST_ASSERT_FALSE(s.tick(0x01, 119, &out));
    TEST_ASSERT_TRUE(s.tick(0x01, 120, &out));
    // 121..1120 inclusive: sends at 140, 160, ..., 1120 = 50 sends.
    TEST_ASSERT_EQUAL_INT(50, countSends(s, 0x01, 121, 1120));
}

void test_mask_change_sends_immediately() {
    Streamer s(20, 3);
    uint8_t out;
    s.tick(0x01, 100, &out);
    TEST_ASSERT_TRUE(s.tick(0x03, 105, &out));
    TEST_ASSERT_EQUAL_HEX8(0x03, out);
    TEST_ASSERT_FALSE(s.tick(0x03, 110, &out));
}

void test_release_sends_three_zeros_then_silence() {
    Streamer s(20, 3);
    uint8_t out = 0xFF;
    s.tick(0x01, 100, &out);
    TEST_ASSERT_TRUE(s.tick(0x00, 110, &out));
    TEST_ASSERT_EQUAL_HEX8(0x00, out);
    TEST_ASSERT_FALSE(s.tick(0x00, 129, &out));
    TEST_ASSERT_TRUE(s.tick(0x00, 130, &out));
    TEST_ASSERT_TRUE(s.tick(0x00, 150, &out));
    TEST_ASSERT_EQUAL_INT(0, countSends(s, 0x00, 151, 5000));
}

void test_press_during_release_burst() {
    Streamer s(20, 3);
    uint8_t out;
    s.tick(0x01, 100, &out);
    s.tick(0x00, 110, &out);              // first release zero
    TEST_ASSERT_TRUE(s.tick(0x02, 115, &out));
    TEST_ASSERT_EQUAL_HEX8(0x02, out);
    // Held: only 0x02 is sent from here on, no leftover zeros.
    TEST_ASSERT_EQUAL_INT(50, countSends(s, 0x02, 116, 1115));
}

void test_all_buttons_streams_0x1F() {
    Streamer s(20, 3);
    uint8_t out;
    TEST_ASSERT_TRUE(s.tick(0x1F, 0, &out));
    TEST_ASSERT_EQUAL_HEX8(0x1F, out);
}

void test_wraparound() {
    Streamer s(20, 3);
    uint8_t out;
    TEST_ASSERT_TRUE(s.tick(0x01, 0xFFFFFFF0u, &out));
    TEST_ASSERT_FALSE(s.tick(0x01, 0x00000003u, &out));  // 19 ms later
    TEST_ASSERT_TRUE(s.tick(0x01, 0x00000004u, &out));   // 20 ms later
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_idle_sends_nothing);
    RUN_TEST(test_press_sends_immediately);
    RUN_TEST(test_held_resends_every_interval);
    RUN_TEST(test_mask_change_sends_immediately);
    RUN_TEST(test_release_sends_three_zeros_then_silence);
    RUN_TEST(test_press_during_release_burst);
    RUN_TEST(test_all_buttons_streams_0x1F);
    RUN_TEST(test_wraparound);
    return UNITY_END();
}
