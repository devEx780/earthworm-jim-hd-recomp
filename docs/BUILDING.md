# Building

The build turns your copy of Earthworm Jim HD into a native executable. You need the Xbox Live Arcade version, Title ID `584109E2`, version 1.0.0.11. This repository includes nothing from the game.

## 1. Get the source

```sh
git clone --recursive https://github.com/devEx780/earthworm-jim-hd-recomp.git
cd earthworm-jim-hd-recomp
```

If you cloned without `--recursive`, run `git submodule update --init --recursive`.

## 2. Add your game

Put **one** of these in `private/game/`:

- The original Xbox Live package file. It is the single file with a long hexadecimal name under `584109E2/000D0000/` in your console dump.
- The files extracted from that package, such as `default.xex`, `data.dat`, `data01.dat` and the `data` folder.

The build reads `default.xex` from either one.

## 3. Build

The first build compiles the SDK and recompiles the game. It takes about 15 to 30 minutes, depending on the machine. Later builds only redo what changed.

### Windows

You need:

- Visual Studio 2022 or Build Tools for Visual Studio 2022, with the **Desktop development with C++** workload.
- [LLVM](https://github.com/llvm/llvm-project/releases) (clang) 18 or newer and [CMake](https://cmake.org/download/) 3.25 or newer, both on `PATH`.
- Git.

```bat
scripts\windows\build.cmd
scripts\windows\run.cmd
```

The build creates `out\build\win-amd64-release\ewj_hd.exe`. `run.cmd` keeps saves in `private\recomp\` and passes any extra arguments to the game, for example `scripts\windows\run.cmd --gpu_backend=vulkan`.

### Linux

The build is tested on Ubuntu 22.04 and newer. The setup script installs clang 20, CMake, the Vulkan SDK and the development libraries.

```sh
sudo bash scripts/linux/setup-build.sh   # once
bash scripts/linux/build.sh
```

The build creates `out/build/linux-amd64-release/ewj_hd`. On other distributions, install the equivalent of the packages listed in `scripts/linux/setup-build.sh`.

## 4. Make a portable folder (optional)

A portable folder lets you copy the game to another PC, a handheld or a Steam Deck.

```bat
powershell -ExecutionPolicy Bypass -File scripts\windows\package.ps1
```

```sh
bash scripts/linux/package.sh
```

The script creates `private/dist/ewj-hd-windows.zip` or `private/dist/ewj-hd-linux.tar.gz`. The archive holds the executable, its libraries and a launcher, without any game data. [PLAYING.md](PLAYING.md) explains how to use it.

## Troubleshooting

- **"Put your game in private/game"** means the folder is empty. Go back to step 2.
- **"No default.xex and no Xbox Live package found"** means the file in `private/game` is not a valid Xbox Live package.
- **"Visual Studio C++ tools not found"** means the "Desktop development with C++" workload is missing. Install it with the Visual Studio Installer.
- **"clang" or "cmake" not recognized** means they are not on `PATH`. Add the `bin` folders of LLVM and CMake to `PATH` and open a new terminal.
