#!/usr/bin/env bash
# Builds the Vial firmware for the corne42 (Pro Micro RP2040) and optionally flashes it.
#
#   ./vial/build.sh           build -> firmware/crkbd_rev1_corne42_rp2040_ce.uf2
#   ./vial/build.sh flash     build, then copy to a controller in bootloader mode
#                             (hold BOOT while plugging in USB -> drive "RPI-RP2")
#
# Needs git and docker. vial-qmk is cloned into .vial-qmk/ on first run.
set -euo pipefail

REPO="$(cd "$(dirname "$0")/.." && pwd)"
QMK="$REPO/.vial-qmk"
UF2="crkbd_rev1_corne42_rp2040_ce.uf2"

if [ ! -d "$QMK" ]; then
    git clone --depth 1 --recurse-submodules --shallow-submodules \
        https://github.com/vial-kb/vial-qmk.git "$QMK"
fi

docker run --rm -u "$(id -u):$(id -g)" -e HOME=/tmp \
    -v "$QMK":/qmk_firmware \
    -v "$REPO/vial":/qmk_firmware/keyboards/crkbd/keymaps/corne42 \
    -w /qmk_firmware ghcr.io/qmk/qmk_cli \
    make crkbd/rev1:corne42 CONVERT_TO=rp2040_ce

mkdir -p "$REPO/firmware"
cp "$QMK/$UF2" "$REPO/firmware/$UF2"
echo "Firmware: firmware/$UF2"

if [ "${1:-}" = "flash" ]; then
    echo "Waiting for RPI-RP2 (hold BOOT while plugging in USB)..."
    until MNT="$(findmnt -rn -S LABEL=RPI-RP2 -o TARGET)" && [ -n "$MNT" ]; do
        sleep 1
    done
    cp "$REPO/firmware/$UF2" "$MNT/" && sync
    echo "Flashed via $MNT"
fi
