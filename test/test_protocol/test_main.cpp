#include <unity.h>
#include "Protocol.h"

void setUp() {}
void tearDown() {}

// The Flamingo triggers special modes on exactly 0x1F.
void test_all_buttons_mask_is_0x1F() {
    TEST_ASSERT_EQUAL_HEX8(0x1F, ALL_BUTTONS_MASK);
}

// Bit order must match flamingo/src/main.cpp getBlendedColorFromPackets().
void test_bit_order_matches_flamingo() {
    TEST_ASSERT_EQUAL_STRING("red", BUTTON_NAMES[0]);
    TEST_ASSERT_EQUAL_STRING("green", BUTTON_NAMES[1]);
    TEST_ASSERT_EQUAL_STRING("blue", BUTTON_NAMES[2]);
    TEST_ASSERT_EQUAL_STRING("yellow", BUTTON_NAMES[3]);
    TEST_ASSERT_EQUAL_STRING("white", BUTTON_NAMES[4]);
}

void test_encode_packet_is_id_then_mask() {
    uint8_t packet[PACKET_SIZE];
    encodePacket(3, 0x05, packet);
    TEST_ASSERT_EQUAL_UINT8(2, PACKET_SIZE);
    TEST_ASSERT_EQUAL_UINT8(3, packet[0]);
    TEST_ASSERT_EQUAL_HEX8(0x05, packet[1]);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_all_buttons_mask_is_0x1F);
    RUN_TEST(test_bit_order_matches_flamingo);
    RUN_TEST(test_encode_packet_is_id_then_mask);
    return UNITY_END();
}
