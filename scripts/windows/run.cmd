@echo off
rem Runs the local build with the game in private\game; saves, cache and log go to private\recomp.
rem Extra arguments are passed to the game. Overrides: EWJ_GAME_DIR, EWJ_USER_DIR.
setlocal
pushd "%~dp0..\.."
set "BIN=out\build\win-amd64-release\ewj_hd.exe"
if defined EWJ_GAME_DIR (set "GAME=%EWJ_GAME_DIR%") else (set "GAME=%CD%\private\game")
if defined EWJ_USER_DIR (set "USER=%EWJ_USER_DIR%") else (set "USER=%CD%\private\recomp\user")
set "CACHE=%CD%\private\recomp\cache"
set "LOG=%CD%\private\recomp\recomp.log"
if not exist "%BIN%" (
  echo Missing build: "%BIN%". Run scripts\windows\build.cmd first.
  popd
  exit /b 1
)
dir /s /b /a-d "%GAME%" >nul 2>&1 || (
  echo Put your game in "%GAME%": the Xbox Live package file or the extracted files.
  popd
  exit /b 1
)
if not exist "%USER%" mkdir "%USER%"
if not exist "%CACHE%" mkdir "%CACHE%"
"%BIN%" --game_data_root="%GAME%" --user_data_root="%USER%" --cache_root="%CACHE%" --log_file="%LOG%" --gpu_plugin=xenos --mnk_mode --license_mask=1 --no-protect_zero --keybind_a=J --keybind_b=K --keybind_x=L --keybind_y=I --keybind_start=Return %*
set "result=%errorlevel%"
popd
exit /b %result%
