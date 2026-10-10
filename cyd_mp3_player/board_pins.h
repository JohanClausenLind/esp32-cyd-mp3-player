// Pin map for the ESP32-2432S028R "Cheap Yellow Display" (CYD).
// The display pins are also set in config/User_Setup*.h for TFT_eSPI.
#pragma once

// 2.8" 240x320 TFT (ILI9341 or ST7789, depending on the batch) on HSPI
#define PIN_LCD_MOSI   13
#define PIN_LCD_MISO   12
#define PIN_LCD_SCLK   14
#define PIN_LCD_CS     15
#define PIN_LCD_DC      2
#define PIN_LCD_BL     21
// LCD reset is tied to EN

// XPT2046 resistive touch controller
#define PIN_TOUCH_CLK  25
#define PIN_TOUCH_MOSI 32
#define PIN_TOUCH_MISO 39
#define PIN_TOUCH_CS   33
#define PIN_TOUCH_IRQ  36

// Micro SD slot on VSPI
#define PIN_SD_MOSI    23
#define PIN_SD_MISO    19
#define PIN_SD_SCLK    18
#define PIN_SD_CS       5

// Other on-board parts
#define PIN_LED_RED     4  // RGB LED, active low
#define PIN_LED_GREEN  16
#define PIN_LED_BLUE   17
#define PIN_LDR        34  // light sensor
#define PIN_SPEAKER    26  // on-board amp; unused, audio goes over Bluetooth
#define PIN_BOOT_BTN    0
