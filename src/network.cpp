#include "network.h"
#include <ESPmDNS.h>
#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "config.h"

static volatile bool ethConnected = false;  // written from the WiFi event task
static bool mdnsStarted = false;
static IPAddress flamingoIP;
static uint32_t lastResolveMs = 0;
static uint32_t lastRetryMs = 0;
static WiFiUDP udp;

static bool isUnset(const IPAddress& ip) { return ip == IPAddress(); }
static bool ethUp() { return ethConnected; }
static bool wifiUp() { return WiFi.status() == WL_CONNECTED; }

static void onEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            ETH.setHostname(STATION_NAME);
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            ethConnected = true;
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
        case ARDUINO_EVENT_ETH_STOP:
            ethConnected = false;
            break;
        default:
            break;
    }
}

static bool waitFor(bool (*up)(), uint32_t timeoutMs) {
    const uint32_t start = millis();
    while (!up() && millis() - start < timeoutMs) {
        delay(250);
        Serial.print('.');
    }
    Serial.println();
    return up();
}

static void startWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(STATION_NAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

static void resolveFlamingo() {
    IPAddress ip;
    if (ip.fromString(FLAMINGO_HOST) || WiFi.hostByName(FLAMINGO_HOST, ip)) {
        if (!isUnset(ip)) {
            flamingoIP = ip;
            Serial.printf("Flamingo %s -> %s:%u\n", FLAMINGO_HOST, ip.toString().c_str(), FLAMINGO_PORT);
            return;
        }
    }
    Serial.printf("Could not resolve %s, retrying in %u s\n", FLAMINGO_HOST,
                  (unsigned)(FLAMINGO_RESOLVE_RETRY_MS / 1000));
}

void network_begin() {
    WiFi.onEvent(onEvent);

    pinMode(ETH01_POWER_ENABLE_PIN, OUTPUT);
    digitalWrite(ETH01_POWER_ENABLE_PIN, HIGH);
    delay(500);

    Serial.print("Ethernet");
    if (!ETH.begin(ETH01_PHY_ADDR, ETH01_PHY_POWER_PIN, ETH01_MDC_PIN, ETH01_MDIO_PIN,
                   ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN)) {
        Serial.print(" (ETH.begin failed)");
    }
    if (waitFor(ethUp, CONNECT_TIMEOUT_MS)) return;

    Serial.printf("WiFi '%s'", WIFI_SSID);
    startWifi();
    if (!waitFor(wifiUp, CONNECT_TIMEOUT_MS)) {
        Serial.println("No network yet - will keep retrying");
    }
}

void network_loop(uint32_t nowMs) {
    static bool wasConnected = false;
    const bool connected = network_connected();
    if (connected != wasConnected) {
        wasConnected = connected;
        Serial.printf("Network %s: %s %s\n", connected ? "up" : "down", network_type(),
                      network_ip().c_str());
        flamingoIP = IPAddress();
        lastResolveMs = nowMs - FLAMINGO_RESOLVE_RETRY_MS;  // resolve right away
    }

    // Prefer Ethernet: drop WiFi once the cable is up so routing is unambiguous.
    if (ethConnected && WiFi.getMode() != WIFI_OFF) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }

    if (!connected) {
        if (nowMs - lastRetryMs >= NETWORK_RETRY_MS) {
            lastRetryMs = nowMs;
            Serial.println("Offline - retrying WiFi");
            if (WiFi.getMode() == WIFI_OFF) startWifi();
            else WiFi.reconnect();
        }
        return;
    }

    if (!mdnsStarted) {
        mdnsStarted = MDNS.begin(STATION_NAME);
        if (mdnsStarted) Serial.printf("mDNS: %s.local\n", STATION_NAME);
    }

    if (isUnset(flamingoIP) && nowMs - lastResolveMs >= FLAMINGO_RESOLVE_RETRY_MS) {
        lastResolveMs = nowMs;
        resolveFlamingo();
    }
}

bool network_connected() { return ethConnected || wifiUp(); }

const char* network_type() {
    if (ethConnected) return "ethernet";
    if (wifiUp()) return "wifi";
    return "disconnected";
}

String network_ip() {
    if (ethConnected) return ETH.localIP().toString();
    if (wifiUp()) return WiFi.localIP().toString();
    return "0.0.0.0";
}

String network_mac() { return ethConnected ? ETH.macAddress() : WiFi.macAddress(); }

int32_t network_rssi() { return !ethConnected && wifiUp() ? WiFi.RSSI() : 0; }

IPAddress network_flamingo_ip() { return flamingoIP; }

bool network_send(const uint8_t* data, size_t len) {
    if (!network_connected() || isUnset(flamingoIP)) return false;
    if (!udp.beginPacket(flamingoIP, FLAMINGO_PORT)) return false;
    udp.write(data, len);
    return udp.endPacket() == 1;
}
