#include "player.h"

#include <SD.h>
#include <MP3DecoderHelix.h>
#include <vector>
#include "audio_out.h"

using namespace libhelix;

static std::vector<String> tracks;
static String current_track;
static SemaphoreHandle_t track_lock;
static volatile bool skip_requested;

static MP3DecoderHelix decoder;

// Simple resampler state: decoded audio at 48 or 22.05 kHz must become 44.1 kHz.
// Nearest-sample for now; good enough to hear, to be replaced by a proper filter.
static uint32_t resample_phase;  // 16.16 fixed point position in the input

static void on_pcm(MP3FrameInfo &info, short *pcm, size_t samples, void *)
{
    const int channels = info.nChans;
    if (channels < 1 || info.samprate <= 0) {
        return;
    }
    const size_t in_frames = samples / channels;
    const uint32_t step = ((uint64_t)info.samprate << 16) / BT_SAMPLE_RATE;

    int16_t out[256 * 2];
    size_t n = 0;
    while ((resample_phase >> 16) < in_frames) {
        const short *f = pcm + (resample_phase >> 16) * channels;
        out[2 * n] = f[0];
        out[2 * n + 1] = channels > 1 ? f[1] : f[0];
        if (++n == 256) {
            audio_out_write_stereo(out, n);
            n = 0;
        }
        resample_phase += step;
    }
    resample_phase -= in_frames << 16;
    if (n) {
        audio_out_write_stereo(out, n);
    }
}

static void set_current(const String &name)
{
    xSemaphoreTake(track_lock, portMAX_DELAY);
    current_track = name;
    xSemaphoreGive(track_lock);
}

static void play_file(const String &path)
{
    File f = SD.open(path);
    if (!f) {
        Serial.printf("Cannot open %s\n", path.c_str());
        return;
    }
    Serial.printf("Playing %s\n", path.c_str());
    set_current(path.substring(path.lastIndexOf('/') + 1));

    resample_phase = 0;
    decoder.begin();
    uint8_t buf[1024];
    int n;
    skip_requested = false;
    while (!skip_requested && (n = f.read(buf, sizeof(buf))) > 0) {
        decoder.write(buf, n);
    }
    decoder.end();
    f.close();
    if (skip_requested) {
        audio_out_flush();
    }
}

static void player_task(void *)
{
    for (;;) {
        for (const String &path : tracks) {
            while (!audio_out_connected()) {
                set_current("");
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            play_file(path);
        }
    }
}

size_t player_begin(const char *dir)
{
    track_lock = xSemaphoreCreateMutex();

    File root = SD.open(dir);
    if (!root || !root.isDirectory()) {
        Serial.printf("No folder %s on the SD card\n", dir);
        return 0;
    }
    for (File f = root.openNextFile(); f; f = root.openNextFile()) {
        String name = f.name();
        String lower = name;
        lower.toLowerCase();
        if (!f.isDirectory() && lower.endsWith(".mp3")) {
            tracks.push_back(String(dir) + "/" + name);
        }
    }
    std::sort(tracks.begin(), tracks.end());
    Serial.printf("%u track(s) in %s\n", (unsigned)tracks.size(), dir);

    if (!tracks.empty()) {
        decoder.setDataCallback(on_pcm);
        // Decoding is CPU heavy: keep it on core 1, away from the Bluetooth stack.
        xTaskCreatePinnedToCore(player_task, "player", 8192, nullptr, 5, nullptr, 1);
    }
    return tracks.size();
}

String player_current_track()
{
    if (!track_lock) {
        return "";
    }
    xSemaphoreTake(track_lock, portMAX_DELAY);
    String name = current_track;
    xSemaphoreGive(track_lock);
    return name;
}

void player_next()
{
    skip_requested = true;
}
