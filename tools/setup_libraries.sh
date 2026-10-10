#!/usr/bin/env bash
# Installs the pinned libraries into the Arduino libraries folder and writes
# TFT_eSPI's User_Setup.h for your board. Run it again after updating TFT_eSPI.
#
#   tools/setup_libraries.sh                   # Guition JC2432W328C (default)
#   tools/setup_libraries.sh cyd-ili9341       # ESP32-2432S028R, ILI9341 panel
#   tools/setup_libraries.sh cyd-st7789        # ESP32-2432S028R, ST7789 panel
#
# The libraries folder comes from the Arduino IDE 2 / arduino-cli settings.
# Set ARDUINO_LIBRARIES to override it.
set -euo pipefail

board="${1:-jc2432w328c}"
here="$(cd "$(dirname "$0")/.." && pwd)"

case "$board" in
    jc2432w328c) setup=User_Setup_JC2432W328C.h ;;
    cyd-ili9341) setup=User_Setup_CYD_ILI9341.h ;;
    cyd-st7789)  setup=User_Setup_CYD_ST7789.h ;;
    *) echo "unknown board '$board' (use jc2432w328c, cyd-ili9341 or cyd-st7789)" >&2; exit 1 ;;
esac

to_unix() {  # Windows paths from arduino-cli -> Git Bash paths
    if command -v cygpath >/dev/null 2>&1; then cygpath -u "$1"; else printf '%s\n' "$1"; fi
}

# Find the sketchbook folder the Arduino IDE / arduino-cli really use. It is
# often not ~/Documents/Arduino (OneDrive redirection, localized folder names).
sketchbook=""
ide_cfg="$HOME/.arduinoIDE/arduino-cli.yaml"
if [[ -f "$ide_cfg" ]]; then  # Arduino IDE 2 settings
    sketchbook="$(sed -n 's/^[[:space:]]*user:[[:space:]]*//p' "$ide_cfg" | head -n1 | tr -d '"'"'"'\r')"
fi
if [[ -z "$sketchbook" ]] && command -v arduino-cli >/dev/null 2>&1; then
    sketchbook="$(arduino-cli config get directories.user 2>/dev/null | tr -d '\r' || true)"
fi
if [[ -z "$sketchbook" ]]; then
    for d in "$HOME/OneDrive/Documents/Arduino" "$HOME/OneDrive/Dokument/Arduino" \
             "$HOME/Documents/Arduino" "$HOME/Arduino"; do
        if [[ -d "$d" ]]; then sketchbook="$d"; break; fi
    done
fi
sketchbook="$(to_unix "${sketchbook:-$HOME/Arduino}")"
libs="${ARDUINO_LIBRARIES:-$sketchbook/libraries}"
mkdir -p "$libs"
echo "Arduino libraries folder: $libs"

# Folder of an installed library with this name= in library.properties, if any.
installed() {
    { grep -lx "name=$1" "$libs"/*/library.properties 2>/dev/null || true; } | head -n1 | xargs -r dirname
}

clone() {  # repo tag library-name folder
    local found
    found="$(installed "$3")"
    if [[ -n "$found" ]]; then
        echo "$3 already installed in $found, keeping it"
    else
        git -c advice.detachedHead=false clone --quiet --depth 1 --branch "$2" "https://github.com/$1.git" "$libs/$4"
        echo "installed $3 $2"
    fi
}

clone pschatzmann/ESP32-A2DP        v1.8.11  ESP32-A2DP  ESP32-A2DP
clone pschatzmann/arduino-libhelix  v0.9.4   libhelix    arduino-libhelix
clone Bodmer/TFT_eSPI               V2.5.43  TFT_eSPI    TFT_eSPI

# TFT_eSPI ships configured for an ESP8266 board; replace that with ours.
tft="$(installed TFT_eSPI)"
cp "$here/config/$setup" "$tft/User_Setup.h"
echo "Wrote $tft/User_Setup.h"
echo "TFT_eSPI configured with $setup"
