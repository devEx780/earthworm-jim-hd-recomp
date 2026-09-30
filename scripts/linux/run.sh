#!/bin/sh
# Runs the local build with the game in private/game; saves, cache and log go to private/recomp.
# Extra arguments are passed to the game.
REPO=$(cd "$(dirname "$0")/../.." && pwd)
mkdir -p "$REPO/private/recomp"
EWJ_BIN="$REPO/out/build/linux-amd64-release/ewj_hd" \
EWJ_GAME_DIR="$REPO/private/game" \
EWJ_USER_DIR="$REPO/private/recomp/user" \
EWJ_CACHE_DIR="$REPO/private/recomp/cache" \
EWJ_LOG="$REPO/private/recomp/recomp.log" \
  exec "$REPO/launchers/run.sh" "$@"
