// Pin maps for the supported boards. The board is picked by the TFT_eSPI
// setup that tools/setup_libraries.sh installed (it defines CYD_BOARD_*).
// Display pins are also in config/User_Setup_*.h.
#pragma once

#include <TFT_eSPI.h>

#if defined(CYD_BOARD_JC2432W328C)
// Guition JC2432W328C: ST7789 240x320, CST820 capacitive touch (I2C)
#define BOARD_NAME     "JC2432W328C"
#define PIN_LCD_BL     27
#define TOUCH_CST820
#define PIN_TOUCH_SDA  33
#define PIN_TOUCH_SCL  32
#define PIN_TOUCH_RST  25
#define PIN_TOUCH_INT  21

#elif defined(CYD_BOARD_2432S028R)
// ESP32-2432S028R "Cheap Yellow Display": ILI9341/ST7789, XPT2046 resistive touch
#define BOARD_NAME     "ESP32-2432S028R"
#define PIN_LCD_BL     21
#define TOUCH_XPT2046
#define PIN_TOUCH_CLK  25
#define PIN_TOUCH_MOSI 32
#define PIN_TOUCH_MISO 39
#define PIN_TOUCH_CS   33
#define PIN_TOUCH_IRQ  36

#else
#error "TFT_eSPI is not configured for this board. Run tools/setup_libraries.sh (see README.md)."
#endif

// Shared by both boards.
// Display on HSPI: MOSI 13, SCLK 14, CS 15, DC 2, reset tied to EN.
#define PIN_LCD_MOSI   13
#define PIN_LCD_SCLK   14
#define PIN_LCD_CS     15
#define PIN_LCD_DC      2

// Micro SD slot on VSPI
#define PIN_SD_MOSI    23
#define PIN_SD_MISO    19
#define PIN_SD_SCLK    18
#define PIN_SD_CS       5

#define PIN_LED_RED     4  // RGB LED, active low
#define PIN_LED_GREEN  16
#define PIN_LED_BLUE   17
#define PIN_BOOT_BTN    0
