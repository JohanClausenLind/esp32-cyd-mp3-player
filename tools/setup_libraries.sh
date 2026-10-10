#!/usr/bin/env bash
# Installs the pinned libraries into the Arduino libraries folder and writes
# TFT_eSPI's User_Setup.h for your board. Run it again after updating TFT_eSPI.
#
#   tools/setup_libraries.sh                   # Guition JC2432W328C (default)
#   tools/setup_libraries.sh cyd-ili9341       # ESP32-2432S028R, ILI9341 panel
#   tools/setup_libraries.sh cyd-st7789        # ESP32-2432S028R, ST7789 panel
#
# Set ARDUINO_LIBRARIES to override where the libraries go.
set -euo pipefail

board="${1:-jc2432w328c}"
here="$(cd "$(dirname "$0")/.." && pwd)"

case "$board" in
    jc2432w328c) setup=User_Setup_JC2432W328C.h ;;
    cyd-ili9341) setup=User_Setup_CYD_ILI9341.h ;;
    cyd-st7789)  setup=User_Setup_CYD_ST7789.h ;;
    *) echo "unknown board '$board' (use jc2432w328c, cyd-ili9341 or cyd-st7789)" >&2; exit 1 ;;
esac

# Arduino IDE uses ~/Documents/Arduino on macOS and Windows, ~/Arduino on Linux.
if [[ -n "${ARDUINO_LIBRARIES:-}" ]]; then
    libs="$ARDUINO_LIBRARIES"
elif [[ -d "$HOME/Documents/Arduino" ]]; then
    libs="$HOME/Documents/Arduino/libraries"
else
    libs="$HOME/Arduino/libraries"
fi
mkdir -p "$libs"
echo "Arduino libraries folder: $libs"

clone() {  # repo tag folder
    if [[ -d "$libs/$3" ]]; then
        echo "$3 already installed, keeping it"
    else
        git clone --quiet --depth 1 --branch "$2" "https://github.com/$1.git" "$libs/$3"
        echo "installed $3 $2"
    fi
}

clone pschatzmann/ESP32-A2DP        v1.8.11  ESP32-A2DP
clone pschatzmann/arduino-libhelix  v0.9.4   arduino-libhelix
clone Bodmer/TFT_eSPI               V2.5.43  TFT_eSPI

# TFT_eSPI ships configured for an ESP8266 board; replace that with ours.
cp "$here/config/$setup" "$libs/TFT_eSPI/User_Setup.h"
echo "TFT_eSPI configured with $setup"
