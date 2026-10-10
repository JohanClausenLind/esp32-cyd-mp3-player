// Touch input for the board's touch controller (CST820 or XPT2046).
#pragma once

#include <Arduino.h>

void touch_begin();

// True while the screen is touched; x and y are raw controller coordinates.
bool touch_read(uint16_t *x, uint16_t *y);
