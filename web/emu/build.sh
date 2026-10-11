#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Run from any directory. Requires generated tables from the firmware build.
set -eu
cd "$(dirname "$0")/../.."
mkdir -p build/emu/fonts
if ! command -v emcc >/dev/null 2>&1; then
    exec docker run --rm -v "$PWD:/src" -w /src emscripten/emsdk:4.0.20 sh web/emu/build.sh
fi
emcc -O2 -ffp-contract=off -std=gnu99 -w -Ibuild/gen -Ifirmware/src \
    '-DMELODEE_VERSION="v1.1.0 Web preview"' --no-entry -sSTANDALONE_WASM \
    -sSTACK_SIZE=1048576 -sINITIAL_MEMORY=67108864 -sFILESYSTEM=0 \
    -o build/emu/melodee.wasm web/emu/melodee_web.c
BUILD=$(date +%s)
sed -e 's/@VERSION@/Melodee v1.1.0/g' -e 's/@PREVIEW@/ (minimal preview)/g' -e "s/@BUILD@/$BUILD/g" web/emu/index.html > build/emu/index.html
cp web/emu/worklet.js build/emu/
cp web/emu/fonts/* build/emu/fonts/
if [ -f build/melodee-1.1.0.fwsc ]; then
    python3 web/emu/make_installer.py build/melodee-1.1.0.fwsc build/emu/installer
fi
