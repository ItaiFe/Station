#include <unity.h>
#include "Parse.h"
#include "SimHold.h"

void setUp() {}
void tearDown() {}

void test_parse_mask_accepts_hex_and_decimal() {
    uint8_t m = 0xFF;
    TEST_ASSERT_TRUE(parseMask("0x1F", &m)); TEST_ASSERT_EQUAL_HEX8(0x1F, m);
    TEST_ASSERT_TRUE(parseMask("0X1f", &m)); TEST_ASSERT_EQUAL_HEX8(0x1F, m);
    TEST_ASSERT_TRUE(parseMask("31", &m));   TEST_ASSERT_EQUAL_HEX8(0x1F, m);
    TEST_ASSERT_TRUE(parseMask("5", &m));    TEST_ASSERT_EQUAL_HEX8(0x05, m);
    TEST_ASSERT_TRUE(parseMask("0", &m));    TEST_ASSERT_EQUAL_HEX8(0x00, m);
    TEST_ASSERT_TRUE(parseMask("0x00", &m)); TEST_ASSERT_EQUAL_HEX8(0x00, m);
}

void test_parse_mask_rejects_out_of_range() {
    uint8_t m = 0xAA;
    TEST_ASSERT_FALSE(parseMask("32", &m));
    TEST_ASSERT_FALSE(parseMask("0x20", &m));
    TEST_ASSERT_FALSE(parseMask("0xFF", &m));
    TEST_ASSERT_FALSE(parseMask("99999999999999999999", &m));
    TEST_ASSERT_EQUAL_HEX8(0xAA, m);  // untouched on failure
}

void test_parse_mask_rejects_garbage() {
    uint8_t m;
    TEST_ASSERT_FALSE(parseMask(nullptr, &m));
    TEST_ASSERT_FALSE(parseMask("", &m));
    TEST_ASSERT_FALSE(parseMask("0x", &m));
    TEST_ASSERT_FALSE(parseMask("abc", &m));
    TEST_ASSERT_FALSE(parseMask("1f", &m));
    TEST_ASSERT_FALSE(parseMask("-1", &m));
    TEST_ASSERT_FALSE(parseMask(" 1", &m));
    TEST_ASSERT_FALSE(parseMask("1 ", &m));
}

void test_parse_duration_defaults_and_clamps() {
    uint32_t ms = 0;
    TEST_ASSERT_TRUE(parseDurationMs(nullptr, 1000, 10000, &ms)); TEST_ASSERT_EQUAL_UINT32(1000, ms);
    TEST_ASSERT_TRUE(parseDurationMs("", 1000, 10000, &ms));      TEST_ASSERT_EQUAL_UINT32(1000, ms);
    TEST_ASSERT_TRUE(parseDurationMs("500", 1000, 10000, &ms));   TEST_ASSERT_EQUAL_UINT32(500, ms);
    TEST_ASSERT_TRUE(parseDurationMs("20000", 1000, 10000, &ms)); TEST_ASSERT_EQUAL_UINT32(10000, ms);
    TEST_ASSERT_TRUE(parseDurationMs("99999999999999999999", 1000, 10000, &ms));
    TEST_ASSERT_EQUAL_UINT32(10000, ms);
}

void test_parse_duration_rejects_zero_negative_garbage() {
    uint32_t ms;
    TEST_ASSERT_FALSE(parseDurationMs("0", 1000, 10000, &ms));
    TEST_ASSERT_FALSE(parseDurationMs("-5", 1000, 10000, &ms));
    TEST_ASSERT_FALSE(parseDurationMs("abc", 1000, 10000, &ms));
    TEST_ASSERT_FALSE(parseDurationMs("0x10", 1000, 10000, &ms));
}

void test_sim_hold_lasts_duration() {
    SimHold h;
    TEST_ASSERT_EQUAL_HEX8(0x00, h.mask(0));
    h.start(0x05, 1000, 500);
    TEST_ASSERT_EQUAL_HEX8(0x05, h.mask(500));
    TEST_ASSERT_EQUAL_HEX8(0x05, h.mask(1499));
    TEST_ASSERT_EQUAL_HEX8(0x00, h.mask(1500));
}

void test_sim_hold_restart_replaces() {
    SimHold h;
    h.start(0x05, 1000, 0);
    h.start(0x00, 1000, 100);  // "Release" button
    TEST_ASSERT_EQUAL_HEX8(0x00, h.mask(200));
}

void test_sim_hold_wraparound() {
    SimHold h;
    h.start(0x1F, 100, 0xFFFFFFC0u);
    TEST_ASSERT_EQUAL_HEX8(0x1F, h.mask(0x00000010u));  // 80 ms later
    TEST_ASSERT_EQUAL_HEX8(0x00, h.mask(0x00000024u));  // 100 ms later
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_parse_mask_accepts_hex_and_decimal);
    RUN_TEST(test_parse_mask_rejects_out_of_range);
    RUN_TEST(test_parse_mask_rejects_garbage);
    RUN_TEST(test_parse_duration_defaults_and_clamps);
    RUN_TEST(test_parse_duration_rejects_zero_negative_garbage);
    RUN_TEST(test_sim_hold_lasts_duration);
    RUN_TEST(test_sim_hold_restart_replaces);
    RUN_TEST(test_sim_hold_wraparound);
    return UNITY_END();
}
