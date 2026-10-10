#!/usr/bin/env bash
# Installs the pinned libraries into the Arduino libraries folder and points
# TFT_eSPI at the CYD display setup.
#
#   tools/setup_libraries.sh            # ILI9341 panel (default)
#   tools/setup_libraries.sh st7789     # ST7789 panel
set -euo pipefail

panel="${1:-ili9341}"
here="$(cd "$(dirname "$0")/.." && pwd)"
libs="${ARDUINO_LIBRARIES:-$HOME/Arduino/libraries}"
[[ "$(uname)" == "Darwin" && -z "${ARDUINO_LIBRARIES:-}" ]] && libs="$HOME/Documents/Arduino/libraries"
mkdir -p "$libs"

clone() {  # repo tag folder
    if [[ -d "$libs/$3" ]]; then
        echo "$3 already installed, leaving it alone"
    else
        git clone --quiet --depth 1 --branch "$2" "https://github.com/$1.git" "$libs/$3"
        echo "installed $3 $2"
    fi
}

clone pschatzmann/ESP32-A2DP        v1.8.11  ESP32-A2DP
clone pschatzmann/arduino-libhelix  v0.9.4   arduino-libhelix
clone Bodmer/TFT_eSPI               V2.5.43  TFT_eSPI
clone PaulStoffregen/XPT2046_Touchscreen v1.4 XPT2046_Touchscreen

case "$panel" in
    ili9341) setup=User_Setup_CYD_ILI9341.h ;;
    st7789)  setup=User_Setup_CYD_ST7789.h ;;
    *) echo "unknown panel '$panel' (use ili9341 or st7789)" >&2; exit 1 ;;
esac
cp "$here/config/$setup" "$libs/TFT_eSPI/User_Setup.h"
echo "TFT_eSPI configured with $setup"
