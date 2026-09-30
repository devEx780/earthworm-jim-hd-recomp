#!/bin/bash
# Packs the Linux build (e.g. for the Steam Deck): binary, runtime .so files and launcher, no game
# data. Usage: package.sh [build-repo-root] [output-dir]
set -euo pipefail
SRC=${1:-$(cd "$(dirname "$0")/../.." && pwd)}
OUT=${2:-$(cd "$(dirname "$0")/../.." && pwd)/private/dist}
B=$SRC/out/build/linux-amd64-release
T=$(mktemp -d)/ewj-hd-linux
mkdir -p "$T/game" "$OUT"
cp "$B/ewj_hd" "$B"/*.so "$T/"
cp "$SRC/launchers/run.sh" "$T/"
chmod +x "$T/ewj_hd" "$T/run.sh"
strip --strip-debug "$T/ewj_hd" "$T"/*.so
tar -C "$(dirname "$T")" -czf "$OUT/ewj-hd-linux.tar.gz" ewj-hd-linux
ls -la "$OUT/ewj-hd-linux.tar.gz"
