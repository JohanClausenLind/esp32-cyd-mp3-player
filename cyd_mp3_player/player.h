// Plays the MP3 files in one SD card folder, in order, on repeat.
#pragma once

#include <Arduino.h>

// Scan dir for .mp3 files and start the playback task. Returns the track count.
size_t player_begin(const char *dir);

// Name of the file playing now, or "" when idle.
String player_current_track();

// Skip to the next track.
void player_next();
