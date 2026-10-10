# ESP32 CYD MP3 player

A battery-powered MP3 player built on the ESP32 "Cheap Yellow Display" (CYD).
It reads MP3 files from the board's micro SD card, decodes them on the ESP32,
and streams the audio to a Bluetooth speaker (the ESP32 acts as an A2DP
source). The 2.8" touchscreen is the user interface, and a 3000 mAh LiPo
powers the whole thing.

Firmware: Arduino (ESP32 Arduino core 3.3.10), built with arduino-cli or the
Arduino IDE.

## Status

| Part | State |
| --- | --- |
| Sketch and pinned libraries, compiles for both panel types | done |
| CI build (`docs/ci-build.yml`, move to `.github/workflows/` to enable) | ready |
| SD card: list `/music/*.mp3` and play them in order, on repeat | done |
| Bluetooth: find a speaker, connect, reconnect after a drop | done |
| MP3 decoding (libhelix) into the Bluetooth stream | done, untested on hardware |
| Screen: status line (looking for speaker / now playing) | done |
| Touch UI: track list, play/pause, next/prev, volume | next |
| Proper resampling for 48 kHz and 22.05 kHz files | planned (nearest-sample for now) |
| Battery level, auto-off, deep sleep | planned |

## Hardware

- **Board:** ESP32-2432S028R ("Cheap Yellow Display"): ESP32-WROOM-32 (dual
  core, 240 MHz, 520 KB SRAM, **no PSRAM**, 4 MB flash), 2.8" 320x240 TFT,
  XPT2046 resistive touch, micro SD slot, RGB LED, light sensor.
  Boards sold under this name come with either an ILI9341 or an ST7789
  panel (often the dual-USB batch). The pins are the same; pick the matching
  display setup when installing the libraries.
- **Battery:** 3.7 V 3000 mAh LiPo.
- **Speaker:** any Bluetooth speaker or headphones (A2DP sink).
- **Micro SD card:** FAT32, MP3 files in a `/music` folder.

### Pin map

| Function | GPIO | Bus |
| --- | --- | --- |
| LCD MOSI / MISO / SCLK | 13 / 12 / 14 | HSPI |
| LCD CS / DC / backlight | 15 / 2 / 21 | |
| LCD reset | EN (not on a GPIO) | |
| Touch CLK / MOSI / MISO | 25 / 32 / 39 | |
| Touch CS / IRQ | 33 / 36 | |
| SD MOSI / MISO / SCLK / CS | 23 / 19 / 18 / 5 | VSPI |
| RGB LED R / G / B (active low) | 4 / 16 / 17 | |
| Light sensor (LDR) | 34 | |
| On-board speaker amp | 26 | unused, audio goes to Bluetooth |
| BOOT button | 0 | |

The same table lives in code in [`cyd_mp3_player/board_pins.h`](cyd_mp3_player/board_pins.h).

### Powering it from the battery

The CYD has **no LiPo charger or battery input**: its 5 V rail feeds an
AMS1117 3.3 V regulator, which needs about 4.3 V in. A bare LiPo (3.0 to 4.2 V)
on the 5 V pin will brown out. Use either:

1. a LiPo charger with 5 V boost output (one module, e.g. "IP5306" or
   "TP4056 + boost" boards) wired to the CYD's 5 V/GND power connector, or
2. a TP4056 charger plus a separate 5 V boost converter (e.g. MT3608).

Add a power switch between the boost output and the board. To show battery
level, run the cell voltage through a 100k/100k divider into a free ADC1
input such as GPIO35 (on the P3 header).

Rough runtime (estimate, not measured): Bluetooth streaming plus backlight
draws around 150 to 250 mA at 5 V, so 3000 mAh lasts about 8 to 12 hours with
the screen on, longer with the backlight dimmed or off.

## Software design

```
SD card (/music) --> player task (core 1) --> libhelix MP3 decoder --> resample to 44.1 kHz stereo
                                                                              |
                                                                    16 KB PCM stream buffer
                                                                              |
                     screen + touch (loop) <-- status --  ESP32-A2DP source --+--> speaker
```

| File | What it does |
| --- | --- |
| `cyd_mp3_player/cyd_mp3_player.ino` | start-up: screen, SD card, player, Bluetooth; status display |
| `cyd_mp3_player/config.h` | your settings: speaker name, music folder, start volume |
| `cyd_mp3_player/board_pins.h` | CYD pin map |
| `cyd_mp3_player/player.cpp` | scans the music folder, decodes MP3 to PCM |
| `cyd_mp3_player/audio_out.cpp` | Bluetooth A2DP source fed from the PCM buffer |
| `config/User_Setup_CYD_*.h` | TFT_eSPI display setups (ILI9341 and ST7789) |

Libraries (pinned in `tools/setup_libraries.sh`):
[ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) v1.8.11,
[arduino-libhelix](https://github.com/pschatzmann/arduino-libhelix) v0.9.4,
[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) 2.5.43,
[XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen) 1.4.

**Memory:** the board has no PSRAM. The Bluetooth stack takes about 80 KB and
the MP3 decoder about 33 KB, so buffers stay small and WiFi stays out of this
firmware. [Garionhk/CYD-MP3-Player](https://github.com/Garionhk/CYD-MP3-Player)
(Apache-2.0), a complete player for the same board, documents these limits
well and is a good reference.

## Building

1. Install [arduino-cli](https://arduino.github.io/arduino-cli/) (or the
   Arduino IDE 2) and the ESP32 core:

   ```sh
   arduino-cli core install esp32:esp32@3.3.10 \
     --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```

2. Install the libraries and set up the display. Use `st7789` instead of
   `ili9341` if your screen shows wrong colours or stays blank:

   ```sh
   tools/setup_libraries.sh ili9341
   ```

   This writes TFT_eSPI's `User_Setup.h`; run it again after updating TFT_eSPI.
   The sketch refuses to compile if the display setup is missing.

3. Edit `cyd_mp3_player/config.h` (speaker name, music folder, volume).

4. Build, or build and flash:

   ```sh
   tools/build.sh                 # compile only
   tools/build.sh /dev/ttyUSB0    # compile and flash (COMx on Windows)
   ```

   In the Arduino IDE: board **ESP32 Dev Module**, partition scheme
   **Huge APP (3MB No OTA/1MB SPIFFS)**.

## Trying it

1. Copy some MP3s into `/music` on a FAT32 card and insert it.
2. Put the speaker in pairing mode.
3. Flash and open the serial monitor at 115200 baud. The screen shows
   "Looking for speaker...", then "Playing" with the file name.
