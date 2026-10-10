// ESP32 CYD MP3 player: plays MP3 files from the SD card on a Bluetooth speaker.
// Build setup and wiring: see README.md.

#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>

#include "board_pins.h"
#include "config.h"
#include "audio_out.h"
#include "player.h"
#include "touch.h"

static SPIClass sd_spi(VSPI);
static TFT_eSPI tft;
static bool running = false;  // false when start-up stopped on an error

static void show_status(const char *line1, const String &line2)
{
    tft.fillRect(0, 60, tft.width(), 60, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(line1, 10, 60, 2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(line2, 10, 85, 2);
}

void setup()
{
    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);  // landscape, USB on the right
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("CYD MP3 player", 10, 10, 4);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString(BOARD_NAME "   tap screen: next track", 10, 40, 2);
    touch_begin();

    sd_spi.begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    if (!SD.begin(PIN_SD_CS, sd_spi, 20000000)) {
        show_status("No SD card", "Insert a FAT32 card and reset");
        return;
    }
    if (player_begin(MUSIC_DIR) == 0) {
        show_status("No MP3 files", "Put them in " MUSIC_DIR " on the card");
        return;
    }

    audio_out_begin(PLAYER_BT_NAME, SPEAKER_NAME, START_VOLUME);
    running = true;
    Serial.printf("Free heap after start: %u bytes\n", ESP.getFreeHeap());
}

void loop()
{
    static String shown = "?";
    static bool was_connected = false;
    static bool was_touched = false;
    if (!running) {
        delay(1000);
        return;
    }

    uint16_t x, y;
    bool touched = touch_read(&x, &y);
    if (touched && !was_touched) {
        Serial.printf("Tap at %u,%u: next track\n", x, y);
        player_next();
    }
    was_touched = touched;

    bool connected = audio_out_connected();
    String track = player_current_track();
    if (connected != was_connected || track != shown) {
        if (connected) {
            show_status("Playing", track);
        } else {
            show_status("Looking for speaker...", SPEAKER_NAME[0] ? SPEAKER_NAME : "any speaker in pairing mode");
        }
        was_connected = connected;
        shown = track;
    }
    delay(30);
}
