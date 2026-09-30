@echo off
rem Builds the SDK (sdk\ submodule, incremental) and the game, Windows AMD64 Release.
rem Needs Visual Studio 2022 or its Build Tools with the C++ workload, plus LLVM (clang) and CMake
rem on PATH. Your game goes in private\game: the extracted files or the Xbox Live package file.
setlocal
set "result="
pushd "%~dp0..\.."

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo Visual Studio 2022 or Build Tools not found.
  set "result=1"
  goto failed
)
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
if not defined VSDIR (
  echo Visual Studio C++ tools not found. Install the "Desktop development with C++" workload.
  set "result=1"
  goto failed
)
call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul || goto failed
if exist "%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" (
  set "PATH=%PATH%;%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
)

if not exist "sdk\CMakeLists.txt" (
  echo Missing SDK submodule. Run: git submodule update --init --recursive
  set "result=1"
  goto failed
)
dir /s /b /a-d "private\game" >nul 2>&1 || (
  echo Put your game in private\game: the extracted files or the Xbox Live package file.
  set "result=1"
  goto failed
)

set "SDK=%CD%\sdk\out\install\win-amd64"
pushd sdk
if not exist "out\build\win-amd64\build.ninja" (
  cmake --preset win-amd64 -DREXGLUE_USE_VULKAN=ON || (popd & goto failed)
)
cmake --build out\build\win-amd64 --config Release --target install --parallel || (popd & goto failed)
popd
set "PATH=%SDK%\bin;%PATH%"

cmake -S tools -B out\build\tools -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_PREFIX_PATH="%SDK%" || goto failed
cmake --build out\build\tools || goto failed
out\build\tools\ewj_hd_extract.exe private\game out\xex\default.xex || goto failed
rexglue.exe codegen ewj_hd_manifest.toml || goto failed
cmake --preset win-amd64-release -DCMAKE_PREFIX_PATH="%SDK%" || goto failed
cmake --build out\build\win-amd64-release --parallel || goto failed
popd
exit /b 0
:failed
if not defined result set "result=%errorlevel%"
popd
exit /b %result%
