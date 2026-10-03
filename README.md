# Station ESP32

Button panel for the Flamingods installation. Debounces 5 buttons and streams
the pressed set to the Flamingo ESP over UDP.

## Build & flash

```bash
pio run -e station2 -t upload && pio device monitor   # USB, station 2
export ESP_OTA_PASSWORD=flamingods2024
pio run -e station2-ota -t upload                       # OTA to station-2.local
pio test -e native                                      # host unit tests
```

Each `stationN` env sets `-DSTATION_ID=N`; there is no config file to copy.
Build output goes to `~/.platformio/workspaces/station` (kept out of iCloud-synced
`~/Documents`, which makes builds crawl).

## Wiring (ESP32-ETH01 v1.4)

| Button | GPIO | Mask bit |
|--------|------|----------|
| Red    | 15   | 0x01 |
| Green  | 2    | 0x02 |
| Blue   | 4    | 0x04 |
| Yellow | 12   | 0x08 |
| White  | 14   | 0x10 |

Each button: GPIO ↔ GND, internal pull-up (pressed = LOW).

LED strip (WS2812B): data → **GPIO 33**, plus 5 V and GND (share GND with the ESP32).
GPIO 34–39 are input-only on the ESP32 and cannot drive a strip. Set the strip
length in `LED_COUNT` (`src/config.h`, default 30).

The strip mirrors the pressed buttons with the Flamingo's color blend, shows a
moving rainbow while all five are held, and glows dimly when idle.

## Protocol

UDP to `flamingo-esp32.local:5000`, 2 bytes: `[station_id, mask]`.
Sent immediately on change and every 20 ms while any button is held; on release
`0` is sent 3 times, then nothing. `0x1F` (all five) triggers the station's
special mode on the Flamingo.

## Web

- `http://station-N.local/` — test page (simulate presses, live status)
- `GET /status` — JSON status
- `POST /press?mask=0x05&ms=1000` — hold a mask for `ms` (1..10000, default 1000); `mask=0` releases

## Testing without the Flamingo

```bash
python3 tools/listen.py
```

Then build the station pointed at your machine:
`PLATFORMIO_BUILD_FLAGS='-DFLAMINGO_HOST=\"my-mac.local\"' pio run -e station1 -t upload`

## Config

Pins, timings, WiFi credentials, and OTA password are in `src/config.h`.
