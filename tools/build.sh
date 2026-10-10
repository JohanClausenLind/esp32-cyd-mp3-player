#!/usr/bin/env bash
# Compiles the sketch with arduino-cli. Pass a serial port to also upload:
#   tools/build.sh                 # compile only
#   tools/build.sh /dev/ttyUSB0    # compile and flash
set -euo pipefail

here="$(cd "$(dirname "$0")/.." && pwd)"
# BOARD can be overridden, e.g. espressif:esp32:esp32 for a core installed from git
fqbn="${BOARD:-esp32:esp32:esp32}:PartitionScheme=huge_app"

if [[ $# -ge 1 ]]; then
    arduino-cli compile --fqbn "$fqbn" --upload --port "$1" "$here/cyd_mp3_player"
else
    arduino-cli compile --fqbn "$fqbn" "$here/cyd_mp3_player"
fi
