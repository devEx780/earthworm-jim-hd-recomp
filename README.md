# Earthworm Jim HD PC port

This is a native Windows and Linux port of **Earthworm Jim HD**, the Xbox 360 and Xbox Live Arcade game (Title ID `584109E2`, version 1.0.0.11). It is made by static recompilation with the [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk).

You need your own copy of the game. The repository and the release archives contain **no game files**.

## Download

Download `ewj-hd-windows.zip` for Windows or `ewj-hd-linux.tar.gz` for Linux and Steam Deck from the [Releases](https://github.com/devEx780/earthworm-jim-hd-recomp/releases) page. Extract it, put your game in the `game` folder and start `play.cmd` or `run.sh`. [PLAYING.md](docs/PLAYING.md) has the details.

Do not share these archives together with game files.

To build the game yourself instead, follow [BUILDING.md](docs/BUILDING.md).

## Features

- The game runs as a native executable with Direct3D 12 or Vulkan.
- It supports keyboard, controllers and local multiplayer for up to 4 players.
- The F1 menu has fullscreen, an FPS counter and internal resolution scaling. Higher resolutions look almost the same but put more load on the GPU.
- The game runs from the original Xbox Live package file or from the extracted game files.
- It has been tested on Windows, ROG Ally, Linux and Steam Deck.

## Status

The menus, all 16 story levels, saving, Continue and local multiplayer work. The full campaign and every boss have not been played through yet. Online features such as online multiplayer and leaderboards do not work, because they rely on Xbox Live and this port has no Xbox Live connection. On ultrawide (21:9) screens the game runs with black bars on the sides, because it only renders 16:9.

## Documentation

- [Building](docs/BUILDING.md)
- [Playing](docs/PLAYING.md)

## Repository layout

| Path | Content |
| --- | --- |
| `src/` | Application code and the F1 menu |
| `tools/` | Build helper that reads `default.xex` from your game files |
| `sdk/` | ReXGlue SDK (submodule) with the fixes this game needs |
| `ewj_hd_manifest.toml` | Recompilation manifest |
| `scripts/windows/`, `scripts/linux/` | Build, run and packaging scripts |
| `launchers/` | Launchers copied into packaged builds |

## License

The code in this repository is released under the [BSD 3-Clause License](LICENSE). The ReXGlue SDK and its third-party components keep their own licenses, found in `sdk/LICENSE` and `sdk/thirdparty/`.

Earthworm Jim HD and its assets belong to their respective owners. This project is not affiliated with or endorsed by them.

## Development note

This port was developed with the help of AI coding tools. The maintainer directed the work, reviewed the changes and tested the builds. Report problems through the [issue tracker](https://github.com/devEx780/earthworm-jim-hd-recomp/issues).

## Credits

- Port: devEx780
- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) by Tom Clay and contributors, derived from [Xenia](https://github.com/xenia-project/xenia)
