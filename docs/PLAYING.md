# Playing

## Starting the game

Download `ewj-hd-windows.zip` for Windows or `ewj-hd-linux.tar.gz` for Linux and Steam Deck from the [Releases](https://github.com/devEx780/earthworm-jim-hd-recomp/releases) page. You can also make these archives yourself with `package.ps1` or `package.sh`, as described in [BUILDING.md](BUILDING.md).

1. Extract the archive.
2. Put your game in the `game` folder. It can be the Xbox Live package file or the extracted files.
3. Start `play.cmd` on Windows or `run.sh` on Linux.

The game keeps saves in `user/` and writes its log to `recomp.log`, next to the executable.

If you built the game yourself, you can also run `scripts\windows\run.cmd` or `scripts/linux/run.sh` from the repository. That script uses the game in `private/game` and keeps saves in `private/recomp`.

## Controls

A controller works out of the box. The keyboard controls are:

| Key | Xbox button |
| --- | --- |
| W A S D | Left stick |
| J / K / L / I | A / B / X / Y |
| Enter | Start |
| F1 | Game menu |
| F3 | FPS overlay |
| F4 | All SDK settings |

## Game menu (F1)

The menu sets the internal resolution from 1x (1280x720) to 4x (5120x2880). The game art is fixed-size 2D, so higher resolutions look almost the same as 1x but put much more load on the GPU. Keep 1x on handhelds.

The menu also turns on fullscreen and the FPS counter, and it sets the number of local players.

Cheats are hidden by default. To show them, start the game with `--ewj_cheats=true` or set `ewj_cheats = true` in `ewj_hd.toml`. The cheat unlocks all story levels and backs up the save first.

The menu saves its settings in `ewj_hd.toml` next to the executable. Some options need a restart, and the menu then shows a "Restart now" button. The game itself caps the frame rate at 60 FPS.

## Local multiplayer

Set the number of local players in the F1 menu and restart. Then open Multiplayer and choose Local. In the lobby, each player presses A on their own controller or on the keyboard.

Controllers become players in the order you connect them. The first controller is player 1, the second is player 2, and so on. A controller that you reconnect takes the lowest free player.

The keyboard always controls player 1, together with the first controller. If you turn on "keyboard is player 1" in the menu, the keyboard is player 1 on its own and controllers start at player 2. In that case only three controllers can join.

## Graphics backend

Windows uses Direct3D 12 by default. To use Vulkan, the renderer used on Linux, add `--gpu_backend=vulkan`.

## Steam Deck (SteamOS)

1. In Desktop Mode, extract `ewj-hd-linux.tar.gz` to the Deck's internal storage. A FAT or exFAT card drops the executable permission. If that happens, run `chmod +x ewj_hd run.sh`.
2. Copy your game into `ewj-hd-linux/game/`.
3. Open Konsole in that folder and run `./run.sh` to check that the game starts.
4. In Steam, choose Add a Non-Steam Game and select `run.sh`. Do not enable Proton, because this is a native Linux build.

To open the game menu, map F1 to a back button in Steam Input.

## Known limitations

- Online features do not work: online multiplayer, leaderboards, world records and score posting. They all rely on Xbox Live, and this port has no Xbox Live connection. At the end of a level the game says that it could not post your score. Continue without posting. Your best scores and records stay in the local save and appear in the level select screen.
- Ultrawide (21:9) screens work, but the picture stays 16:9 with black bars on the sides. The game itself only renders 16:9: its camera, levels and HUD are built for that shape. A true ultrawide view would need changes to the game's own code, which this port translates but does not rewrite. That would take a decompilation of the game.
