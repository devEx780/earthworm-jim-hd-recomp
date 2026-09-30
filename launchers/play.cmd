@echo off
rem Launcher for the portable folder. Next to this file: ewj_hd.exe, its DLLs and game\ with the
rem Xbox Live package file or the extracted game files. Saves go to user\.
rem Overrides: EWJ_GAME_DIR, EWJ_USER_DIR.
setlocal
pushd "%~dp0"
if defined EWJ_GAME_DIR (set "GAME=%EWJ_GAME_DIR%") else (set "GAME=%~dp0game")
if defined EWJ_USER_DIR (set "USER=%EWJ_USER_DIR%") else (set "USER=%~dp0user")
dir /s /b /a-d "%GAME%" >nul 2>&1 || (
  echo Put your game in "%GAME%": the Xbox Live package file or the extracted files.
  pause
  popd
  exit /b 1
)
if not exist "%USER%" mkdir "%USER%"
if not exist "%~dp0cache" mkdir "%~dp0cache"
start "" ewj_hd.exe --game_data_root="%GAME%" --user_data_root="%USER%" --cache_root="%~dp0cache" --log_file="%~dp0recomp.log" --gpu_plugin=xenos --mnk_mode --license_mask=1 --no-protect_zero --keybind_a=J --keybind_b=K --keybind_x=L --keybind_y=I --keybind_start=Return %*
popd
