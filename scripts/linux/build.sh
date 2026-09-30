#!/bin/bash
# Linux release build: builds the SDK from sdk/ (incremental), extracts default.xex from
# private/game (extracted files or the Xbox Live package file), recompiles and builds the game.
# Extra arguments go to the final game build.
set -euo pipefail
cd "$(dirname "$0")/../.."
[ -f sdk/CMakeLists.txt ] || { echo "Missing SDK submodule: git submodule update --init --recursive"; exit 1; }
[ -n "$(find private/game -type f 2>/dev/null | head -n 1)" ] ||
  { echo "Put your game in private/game: the extracted files or the Xbox Live package file."; exit 1; }

SDK=$PWD/sdk/out/install/linux-amd64
(cd sdk && { [ -f out/build/linux-amd64/build.ninja ] || cmake --preset linux-amd64; } &&
  cmake --build out/build/linux-amd64 --config Release --target install)
export PATH="$SDK/bin:$PATH"

cmake -S tools -B out/build/tools -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_PREFIX_PATH="$SDK"
cmake --build out/build/tools
out/build/tools/ewj_hd_extract private/game out/xex/default.xex
rexglue codegen ewj_hd_manifest.toml
cmake --preset linux-amd64-release -DCMAKE_PREFIX_PATH="$SDK"
cmake --build out/build/linux-amd64-release "$@"
