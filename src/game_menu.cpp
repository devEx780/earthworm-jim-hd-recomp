// ewj_hd - in-game menu (F1). See game_menu.h.

#include "game_menu.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <imgui.h>

#include <rex/cvar.h>
#include <rex/logging.h>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
REXCVAR_DEFINE_BOOL(ewj_show_fps, false, "EWJ", "Show the FPS counter in the corner");
REXCVAR_DEFINE_BOOL(ewj_cheats, false, "EWJ", "Show the Cheats section in the F1 menu");

namespace ewj {
namespace {

constexpr size_t kUnlockOffset = 14;

std::filesystem::path PendingUnlockMarker(const std::filesystem::path& user_data_root) {
  return user_data_root / "ewj-unlock-levels.pending";
}

std::vector<uint8_t> ReadFile(const std::filesystem::path& path) {
  std::ifstream in(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

bool LevelsUnlocked(const std::vector<uint8_t>& save) {
  return save.size() > kUnlockOffset + 1 && save[kUnlockOffset] == 0xFF &&
         save[kUnlockOffset + 1] == 0xFF;
}

int CvarInt(const char* name) {
  try {
    return std::stoi(rex::cvar::GetFlagByName(name));
  } catch (...) {
    return 1;
  }
}

bool CvarBool(const char* name) { return rex::cvar::GetFlagByName(name) == "true"; }

// Updates only the keys this menu owns, keeping every other line of the config.
// Players 2-4 must be signed in from startup, so the choice only takes effect on restart:
// it is saved to the config but the running value is left alone.
int g_pending_players = 0;

void SaveSettings(const std::filesystem::path& config_path) {
  static const char* kKeys[] = {"resolution_scale", "fullscreen", "keyboard_own_player",
                                "ewj_show_fps", "local_players"};
  std::vector<std::string> lines;
  {
    std::ifstream in(config_path);
    for (std::string line; std::getline(in, line);) {
      bool ours = false;
      for (const char* key : kKeys) {
        std::string k(key);
        ours |= line.rfind(k, 0) == 0 && line.find_first_not_of(' ', k.size()) != std::string::npos &&
                line[line.find_first_not_of(' ', k.size())] == '=';
      }
      if (!ours) lines.push_back(line);
    }
  }
  for (const char* key : kKeys) {
    std::string value = std::string(key) == "local_players" ? std::to_string(g_pending_players)
                                                            : rex::cvar::GetFlagByName(key);
    lines.push_back(std::string(key) + " = " + value);
  }
  std::ofstream out(config_path, std::ios::trunc);
  for (const auto& line : lines) out << line << '\n';
}

}  // namespace

std::filesystem::path SavePath(const std::filesystem::path& user_data_root) {
  return user_data_root / "584109E2" / "profile" / "User" / "63E83FFF";
}

void RelaunchSelf() {
#ifdef _WIN32
  std::wstring command_line = GetCommandLineW();
  STARTUPINFOW startup{sizeof(startup)};
  PROCESS_INFORMATION process{};
  if (CreateProcessW(nullptr, command_line.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr,
                     &startup, &process)) {
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
  } else {
    REXLOG_ERROR("EWJ menu: relaunch failed ({})", GetLastError());
  }
#else
  // /proc/self/cmdline holds the original argv, NUL-separated.
  auto cmdline = ReadFile("/proc/self/cmdline");
  std::vector<char*> argv;
  for (size_t i = 0; i < cmdline.size(); i += std::strlen(reinterpret_cast<char*>(&cmdline[i])) + 1)
    argv.push_back(reinterpret_cast<char*>(&cmdline[i]));
  argv.push_back(nullptr);
  if (fork() == 0) {
    execv("/proc/self/exe", argv.data());
    _exit(127);
  }
#endif
}

bool FpsCounterEnabled() { return REXCVAR_GET(ewj_show_fps); }

void ApplyPendingLevelUnlock(const std::filesystem::path& user_data_root) {
  auto marker = PendingUnlockMarker(user_data_root);
  if (!std::filesystem::exists(marker)) return;
  std::filesystem::remove(marker);
  auto path = SavePath(user_data_root);
  auto save = ReadFile(path);
  if (save.size() <= kUnlockOffset + 1) {
    REXLOG_WARN("EWJ unlock: no save at {}; start a game first", path.string());
    return;
  }
  auto stamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char suffix[32];
  std::strftime(suffix, sizeof(suffix), ".backup-%Y%m%d-%H%M%S", std::localtime(&stamp));
  std::filesystem::copy_file(path, path.string() + suffix,
                             std::filesystem::copy_options::overwrite_existing);
  save[kUnlockOffset] = save[kUnlockOffset + 1] = 0xFF;
  std::ofstream(path, std::ios::binary | std::ios::trunc)
      .write(reinterpret_cast<const char*>(save.data()), std::streamsize(save.size()));
  REXLOG_INFO("EWJ unlock: all story levels unlocked, backup {}{}", path.string(), suffix);
}

GameMenu::GameMenu(rex::ui::ImGuiDrawer* drawer, MenuHost& host)
    : ImGuiDialog(drawer),
      host_(host),
      running_scale_(CvarInt("resolution_scale")),
      running_players_(CvarInt("local_players")) {
  if (g_pending_players == 0) g_pending_players = running_players_;
}

void GameMenu::OnDraw(ImGuiIO& io) {
  const float ui_scale = std::max(1.0f, io.DisplaySize.y / 720.0f);
  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.12f),
                          ImGuiCond_Always, ImVec2(0.5f, 0.0f));
  ImGui::SetNextWindowSize(ImVec2(560 * ui_scale, 0), ImGuiCond_Always);
  bool open = true;
  if (!ImGui::Begin("Earthworm Jim HD - Menu (F1)", &open,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
    ImGui::End();
    return;
  }
  ImGui::SetWindowFontScale(ui_scale);

  ImGui::SeparatorText("Graphics");
  ImGui::TextUnformatted("Internal resolution");
  int scale = CvarInt("resolution_scale");
  for (int s = 1; s <= 4; ++s) {
    char label[48];
    std::snprintf(label, sizeof(label), "%dx  (%dx%d)%s", s, 1280 * s, 720 * s,
                  s == 1 ? "  original" : "");
    if (ImGui::RadioButton(label, scale == s)) {
      rex::cvar::SetFlagByName("resolution_scale", std::to_string(s));
      SaveSettings(host_.config_path);
    }
  }
  if (CvarInt("resolution_scale") != running_scale_) {
    ImGui::TextColored(ImVec4(1, 0.8f, 0.2f, 1), "Applies after restart (current: %dx).",
                       running_scale_);
  }
  ImGui::TextDisabled("The game renders at 1280x720 and its art is fixed-size 2D, so higher\n"
                      "scales look almost the same but cost much more GPU.");

  bool fullscreen = CvarBool("fullscreen");
  if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
    rex::cvar::SetFlagByName("fullscreen", fullscreen ? "true" : "false");
    SaveSettings(host_.config_path);
  }
  bool show_fps = REXCVAR_GET(ewj_show_fps);
  if (ImGui::Checkbox("Show FPS", &show_fps)) {
    REXCVAR_SET(ewj_show_fps, show_fps);
    host_.set_fps_visible(show_fps);
    SaveSettings(host_.config_path);
  }
  auto stats = host_.frame_stats();
  if (stats.fps > 0) {
    ImGui::SameLine();
    ImGui::TextDisabled("(%.1f FPS; the game is capped at 60)", stats.fps);
  }

  ImGui::SeparatorText("Controls");
  ImGui::TextUnformatted("Local players (profiles created at startup)");
  for (int n = 1; n <= 4; ++n) {
    if (n > 1) ImGui::SameLine();
    if (ImGui::RadioButton(std::to_string(n).c_str(), g_pending_players == n)) {
      g_pending_players = n;
      SaveSettings(host_.config_path);
    }
  }
  if (g_pending_players != running_players_) {
    ImGui::TextColored(ImVec4(1, 0.8f, 0.2f, 1), "Applies after restart (current: %d).",
                       running_players_);
  }
  bool split = CvarBool("keyboard_own_player");
  if (ImGui::Checkbox("Keyboard = player 1, controllers = player 2+", &split)) {
    rex::cvar::SetFlagByName("keyboard_own_player", split ? "true" : "false");
    SaveSettings(host_.config_path);
  }
  ImGui::TextDisabled("Local multiplayer: pick 2+ players and restart; in the lobby each\n"
                      "player presses A on their own controller. Off: keyboard and first\n"
                      "controller both control player 1.");

  if (REXCVAR_GET(ewj_cheats)) {
    ImGui::SeparatorText("Cheats");
    auto save = ReadFile(SavePath(host_.user_data_root));
    if (LevelsUnlocked(save)) {
      ImGui::TextColored(ImVec4(0.4f, 1, 0.4f, 1), "All levels are unlocked.");
    } else if (save.size() <= kUnlockOffset + 1) {
      ImGui::TextDisabled("No save yet: start a game before unlocking.");
    } else if (ImGui::Button("Unlock all levels and restart")) {
      std::ofstream(PendingUnlockMarker(host_.user_data_root)).put('1');
      host_.restart();
    }
    ImGui::TextDisabled("Unlocks the 16 story levels in Level Select (Continue).\n"
                        "The original save is copied next to it (.backup-...).");
  }

  ImGui::Separator();
  if ((CvarInt("resolution_scale") != running_scale_ || g_pending_players != running_players_) &&
      ImGui::Button("Restart now")) {
    host_.restart();
  }
  ImGui::End();
  if (!open) host_.close_menu();
}

void FpsCorner::OnDraw(ImGuiIO& io) {
  auto stats = stats_();
  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 10, 10), ImGuiCond_Always, ImVec2(1, 0));
  ImGui::SetNextWindowBgAlpha(0.4f);
  if (ImGui::Begin("##ewj_fps", nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                       ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoSavedSettings |
                       ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("%.0f FPS", stats.fps);
  }
  ImGui::End();
}

}  // namespace ewj
