// Build helper: writes the game's default.xex to <output> so the recompiler can read it.
// <game_dir> holds either the extracted game files or the original Xbox Live package file.
// Usage: ewj_hd_extract <game_dir> <output>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <vector>

#include <rex/filesystem/devices/stfs_container_device.h>
#include <rex/filesystem/entry.h>
#include <rex/filesystem/file.h>
#include <rex/logging.h>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  if (argc != 3) {
    std::fprintf(stderr, "usage: %s <game_dir> <output>\n", argv[0]);
    return 2;
  }
  const fs::path game_dir = argv[1];
  const fs::path output = argv[2];
  fs::create_directories(output.parent_path());

  if (fs::exists(game_dir / "default.xex")) {
    fs::copy_file(game_dir / "default.xex", output, fs::copy_options::update_existing);
    return 0;
  }

  rex::InitLogging();
  rex::filesystem::StfsContainerDevice device("\\Device\\Package", game_dir);
  if (!device.Initialize()) {
    std::fprintf(stderr, "No default.xex and no Xbox Live package found in %s\n",
                 game_dir.string().c_str());
    return 1;
  }
  auto* entry = device.ResolvePath("default.xex");
  rex::filesystem::File* file = nullptr;
  if (!entry || entry->Open(0x80000000 /* GENERIC_READ */, &file) != 0 || !file) {
    std::fprintf(stderr, "The package in %s does not contain default.xex\n",
                 game_dir.string().c_str());
    return 1;
  }
  std::vector<uint8_t> data(entry->size());
  size_t read = 0;
  file->ReadSync(data, 0, &read);
  file->Destroy();
  if (read != data.size()) {
    std::fprintf(stderr, "Short read of default.xex (%zu of %zu bytes)\n", read, data.size());
    return 1;
  }
  std::ofstream(output, std::ios::binary)
      .write(reinterpret_cast<const char*>(data.data()), std::streamsize(data.size()));
  return 0;
}
