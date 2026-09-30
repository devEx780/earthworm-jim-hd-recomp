#!/bin/sh
# Linux / Steam Deck launcher. Layout next to this script: ewj_hd, game/ (the Xbox Live package
# file or the extracted game files), user/ and cache/ (created on first run).
# Overrides: EWJ_BIN, EWJ_GAME_DIR, EWJ_USER_DIR, EWJ_CACHE_DIR, EWJ_LOG.
DIR=$(cd "$(dirname "$0")" && pwd)
BIN=${EWJ_BIN:-$DIR/ewj_hd}
GAME=${EWJ_GAME_DIR:-$DIR/game}
USER_DIR=${EWJ_USER_DIR:-$DIR/user}
CACHE=${EWJ_CACHE_DIR:-$DIR/cache}
LOG=${EWJ_LOG:-$DIR/recomp.log}
[ -x "$BIN" ] || { echo "Missing recomp binary: $BIN"; exit 1; }
[ -n "$(find "$GAME" -type f 2>/dev/null | head -n 1)" ] || {
  echo "Copy your game into $GAME: the Xbox Live package file or the extracted files."
  exit 1
}
mkdir -p "$USER_DIR" "$CACHE" || exit 1
cd "$DIR" || exit 1
exec "$BIN" --game_data_root="$GAME" --user_data_root="$USER_DIR" --cache_root="$CACHE" \
  --log_file="$LOG" --gpu_plugin=xenos --mnk_mode --license_mask=1 --no-protect_zero \
  --keybind_a=J --keybind_b=K --keybind_x=L --keybind_y=I --keybind_start=Return "$@"
