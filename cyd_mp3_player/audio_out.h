// PCM hand-off between the MP3 decoder and the Bluetooth A2DP source.
#pragma once

#include <Arduino.h>

// A2DP always streams 44.1 kHz, 16-bit stereo.
constexpr uint32_t BT_SAMPLE_RATE = 44100;

// Start Bluetooth and look for the speaker.
void audio_out_begin(const char *player_name, const char *speaker_name, uint8_t volume);

bool audio_out_connected();

// Queue interleaved stereo int16 samples. Blocks while the buffer is full.
void audio_out_write_stereo(const int16_t *pcm, size_t frames);

// Drop anything still queued (track change).
void audio_out_flush();
