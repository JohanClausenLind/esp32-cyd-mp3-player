#include "audio_out.h"

#include <BluetoothA2DPSource.h>
#include <freertos/stream_buffer.h>

// About 93 ms of audio. RAM is tight with no PSRAM, so keep this modest.
static constexpr size_t PCM_BUFFER_BYTES = 16 * 1024;

static BluetoothA2DPSource a2dp;
static StreamBufferHandle_t pcm_buffer;
static const char *wanted_speaker = "";

// Called during discovery for every device that looks like a speaker.
static bool on_speaker_found(const char *name, esp_bd_addr_t, int rssi)
{
    Serial.printf("Found speaker \"%s\" (RSSI %d)\n", name, rssi);
    return wanted_speaker[0] == '\0' || strcmp(name, wanted_speaker) == 0;
}

// Called by the Bluetooth stack whenever it wants more audio. Must not block.
static int32_t on_audio_needed(Frame *frames, int32_t count)
{
    size_t bytes = count * sizeof(Frame);
    size_t got = xStreamBufferReceive(pcm_buffer, frames, bytes, 0);
    if (got < bytes) {
        memset((uint8_t *)frames + got, 0, bytes - got);  // underrun: silence
    }
    return count;
}

static void on_connection_changed(esp_a2d_connection_state_t state, void *)
{
    Serial.printf("Speaker: %s\n", a2dp.to_str(state));
}

void audio_out_begin(const char *player_name, const char *speaker_name, uint8_t volume)
{
    wanted_speaker = speaker_name;
    pcm_buffer = xStreamBufferCreate(PCM_BUFFER_BYTES, sizeof(Frame));

    a2dp.set_local_name(player_name);
    a2dp.set_ssid_callback(on_speaker_found);
    a2dp.set_on_connection_state_changed(on_connection_changed);
    a2dp.set_auto_reconnect(true);
    a2dp.set_volume(volume);
    a2dp.set_data_callback_in_frames(on_audio_needed);
    a2dp.start();
}

bool audio_out_connected()
{
    return a2dp.is_connected();
}

void audio_out_write_stereo(const int16_t *pcm, size_t frames)
{
    xStreamBufferSend(pcm_buffer, pcm, frames * sizeof(Frame), portMAX_DELAY);
}

void audio_out_flush()
{
    xStreamBufferReset(pcm_buffer);
}
