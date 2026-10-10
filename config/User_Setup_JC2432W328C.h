// TFT_eSPI setup for the Guition JC2432W328C (2.8" ESP32 board with ST7789
// panel and CST820 capacitive touch). Values from the board vendor's examples:
// https://github.com/hohastudios/JC2432W328C
// tools/setup_libraries.sh copies this over TFT_eSPI/User_Setup.h.
#define CYD_USER_SETUP
#define CYD_BOARD_JC2432W328C
#define DISABLE_ALL_LIBRARY_WARNINGS  // touch is I2C (CST820), not TFT_eSPI's XPT2046

#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
#define TFT_RGB_ORDER TFT_RGB
#define TFT_INVERSION_OFF

#define USE_HSPI_PORT      // VSPI belongs to the SD card
#define TFT_MISO -1        // panel is write-only on this board
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1
#define TFT_BL   27
#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY 27000000  // vendor's tested speed; may go higher
