#include "touch.h"
#include "board_pins.h"

#if defined(TOUCH_CST820)
#include <Wire.h>

static constexpr uint8_t CST820_ADDR = 0x15;

void touch_begin()
{
    pinMode(PIN_TOUCH_RST, OUTPUT);
    digitalWrite(PIN_TOUCH_RST, LOW);
    delay(10);
    digitalWrite(PIN_TOUCH_RST, HIGH);
    delay(100);  // controller boot time
    Wire.begin(PIN_TOUCH_SDA, PIN_TOUCH_SCL);
}

bool touch_read(uint16_t *x, uint16_t *y)
{
    // Registers 0x00..0x06: gesture, finger count, then 12-bit X and Y.
    Wire.beginTransmission(CST820_ADDR);
    Wire.write(0x00);
    if (Wire.endTransmission(false) != 0 || Wire.requestFrom(CST820_ADDR, (uint8_t)7) != 7) {
        return false;
    }
    uint8_t r[7];
    for (uint8_t &b : r) {
        b = Wire.read();
    }
    if ((r[2] & 0x0F) == 0) {
        return false;
    }
    *x = ((r[3] & 0x0F) << 8) | r[4];
    *y = ((r[5] & 0x0F) << 8) | r[6];
    return true;
}

#elif defined(TOUCH_XPT2046)
// TODO: the XPT2046's pins are not on a free SPI host on the 2432S028R
// (HSPI drives the display, VSPI the SD card), so it needs a bit-banged driver.
void touch_begin() {}

bool touch_read(uint16_t *, uint16_t *)
{
    return false;
}
#endif
