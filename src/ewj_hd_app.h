// ewj_hd - application: F1 game menu, FPS counter and restart handling on top of rex::ReXApp.

#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>

#include <rex/graphics/command_processor.h>
#include <rex/graphics/graphics_system.h>
#include <rex/input/input_system.h>
#include <rex/rex_app.h>
#include <rex/runtime.h>
#include <rex/ui/keybinds.h>
#include <rex/ui/windowed_app_context.h>

#include "game_menu.h"

class EwjHdApp : public rex::ReXApp {
 public:
  using rex::ReXApp::ReXApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(
      rex::ui::WindowedAppContext& ctx) {
    return std::unique_ptr<EwjHdApp>(new EwjHdApp(ctx, "ewj_hd",
        PPCImageConfig));
  }

  void OnConfigurePaths(rex::PathConfig& paths) override { host_.config_path = paths.config_path; }

  void OnPostSetup() override {
    host_.user_data_root = user_data_root();
    ewj::ApplyPendingLevelUnlock(host_.user_data_root);
    host_.frame_stats = [this, window = std::make_shared<FrameWindow>()]() {
      return MeasureFrames(*window);
    };
    host_.set_fps_visible = [this](bool visible) {
      app_context().CallInUIThreadDeferred([this, visible] {
        fps_.reset();
        if (visible) fps_ = std::make_unique<ewj::FpsCorner>(imgui_drawer(), host_.frame_stats);
      });
    };
    host_.close_menu = [this] {
      app_context().CallInUIThreadDeferred([this] {
        menu_.reset();
        menu_open_ = false;
      });
    };
    host_.restart = [this] {
      ewj::RelaunchSelf();
      app_context().CallInUIThreadDeferred([this] {
        if (window()) window()->RequestClose();
      });
    };
    // F3 debug overlay: guest frames per second.
    SetGuestFrameStats(host_.frame_stats);
    if (ewj::FpsCounterEnabled()) host_.set_fps_visible(true);
    // While the menu is open, mouse clicks and keys drive the menu, not Jim.
    if (auto* input = static_cast<rex::input::InputSystem*>(runtime()->input_system())) {
      input->SetActiveCallback([this] { return !menu_open_.load(); });
    }
  }

  void OnCreateDialogs(rex::ui::ImGuiDrawer* drawer) override {
    rex::ui::RegisterBind("bind_game_menu", "F1", "Toggle game menu", [this, drawer] {
      if (menu_) {
        menu_.reset();
      } else {
        menu_ = std::make_unique<ewj::GameMenu>(drawer, host_);
      }
      menu_open_ = menu_ != nullptr;
    });
  }

  void OnShutdown() override {
    menu_.reset();
    fps_.reset();
  }

 private:
  // Guest frames per second from the GPU swap counter, over half-second windows.
  struct FrameWindow {
    std::mutex mutex;
    std::chrono::steady_clock::time_point start{};
    uint32_t start_swaps = 0;
    uint64_t samples = 0;
    rex::ui::FrameStats stats;
  };

  rex::ui::FrameStats MeasureFrames(FrameWindow& window) {
    auto* graphics = static_cast<rex::graphics::GraphicsSystem*>(runtime()->graphics_system());
    auto* cp = graphics ? graphics->command_processor() : nullptr;
    if (!cp) return rex::ui::FrameStats{};
    std::lock_guard lock(window.mutex);
    auto now = std::chrono::steady_clock::now();
    uint32_t swaps = cp->swap_count();
    double secs = std::chrono::duration<double>(now - window.start).count();
    if (window.samples == 0 || secs >= 0.5) {
      if (window.samples && swaps != window.start_swaps) {
        uint32_t delta = swaps - window.start_swaps;
        window.stats.fps = delta / secs;
        window.stats.frame_time_ms = secs * 1000.0 / delta;
      }
      window.start = now;
      window.start_swaps = swaps;
    }
    window.stats.frame_count = ++window.samples;
    return window.stats;
  }

  ewj::MenuHost host_;
  std::unique_ptr<ewj::GameMenu> menu_;
  std::unique_ptr<ewj::FpsCorner> fps_;
  std::atomic<bool> menu_open_{false};
};
