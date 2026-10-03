#pragma once
#include <Arduino.h>
#include <IPAddress.h>

// Boot connect: Ethernet first, then WiFi; blocks at most CONNECT_TIMEOUT_MS each.
void network_begin();
// Call every loop: reconnects, starts mDNS, resolves the Flamingo.
void network_loop(uint32_t nowMs);

bool network_connected();
const char* network_type();  // "ethernet" | "wifi" | "disconnected"
String network_ip();
String network_mac();
int32_t network_rssi();      // 0 unless on WiFi
IPAddress network_flamingo_ip();  // 0.0.0.0 until resolved

// Sends one UDP datagram to the Flamingo. False if offline or unresolved.
bool network_send(const uint8_t* data, size_t len);
