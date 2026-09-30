// ewj_hd - in-game menu (F1): resolution, display, controls and level-unlock cheat.

#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include <rex/ui/imgui_dialog.h>
#include <rex/ui/overlay/debug_overlay.h>

namespace ewj {

struct MenuHost {
  std::filesystem::path config_path;       // ewj_hd.toml next to the exe
  std::filesystem::path user_data_root;    // profile/save root of this run
  std::function<rex::ui::FrameStats()> frame_stats;
  std::function<void(bool)> set_fps_visible;
  std::function<void()> close_menu;
  std::function<void()> restart;           // relaunch with the same command line
};

// Profile save of the title: bytes 14-15 = 0xFFFF unlocks the 16 story levels.
std::filesystem::path SavePath(const std::filesystem::path& user_data_root);
// Applied at startup, before the game reads its profile (the game rewrites the save from its
// own memory copy, so patching the file while it runs would be lost).
void ApplyPendingLevelUnlock(const std::filesystem::path& user_data_root);
bool FpsCounterEnabled();
// Starts a new copy of the game with the same command line (settings that need a restart).
void RelaunchSelf();

class GameMenu final : public rex::ui::ImGuiDialog {
 public:
  GameMenu(rex::ui::ImGuiDrawer* drawer, MenuHost& host);

 protected:
  void OnDraw(ImGuiIO& io) override;

 private:
  MenuHost& host_;
  int running_scale_;
  int running_players_;
};

class FpsCorner final : public rex::ui::ImGuiDialog {
 public:
  FpsCorner(rex::ui::ImGuiDrawer* drawer, std::function<rex::ui::FrameStats()> stats)
      : ImGuiDialog(drawer), stats_(std::move(stats)) {}

 protected:
  void OnDraw(ImGuiIO& io) override;

 private:
  std::function<rex::ui::FrameStats()> stats_;
};

}  // namespace ewj
