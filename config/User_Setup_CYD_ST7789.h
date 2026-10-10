// TFT_eSPI setup for the ESP32-2432S028R "Cheap Yellow Display", ST7789 panel (often the dual-USB "CYD2USB" batch).
// tools/setup_libraries.sh copies this over TFT_eSPI/User_Setup.h.
#define CYD_USER_SETUP
#define DISABLE_ALL_LIBRARY_WARNINGS  // touch is read with XPT2046_Touchscreen, not TFT_eSPI

#define ST7789_DRIVER
#define TFT_RGB_ORDER TFT_RGB
#define TFT_INVERSION_OFF  // try TFT_INVERSION_ON if colours look inverted
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define USE_HSPI_PORT
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1
#define TFT_BL   21
#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY      40000000
#define SPI_READ_FREQUENCY 20000000
