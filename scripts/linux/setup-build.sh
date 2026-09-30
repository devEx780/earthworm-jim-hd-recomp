#!/bin/bash
# Mirrors rexglue-sdk .github/workflows/_build-platform.yaml (linux-amd64, ubuntu:22.04).
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y --no-install-recommends ca-certificates curl wget gnupg git git-lfs \
  software-properties-common lsb-release rsync
cd /tmp
wget -qO llvm.sh https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 20
apt-get install -y clang-20 lld-20
update-alternatives --install /usr/bin/clang clang /usr/bin/clang-20 200
update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-20 200
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc > /etc/apt/trusted.gpg.d/lunarg.asc
wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
add-apt-repository -y ppa:ubuntu-toolchain-r/test
wget -qO- https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor -o /usr/share/keyrings/kitware.gpg
echo "deb [signed-by=/usr/share/keyrings/kitware.gpg] https://apt.kitware.com/ubuntu/ jammy main" > /etc/apt/sources.list.d/kitware.list
apt-get update
apt-get install -y cmake ninja-build build-essential g++-13 git curl unzip zip autoconf python3-venv \
  libgtk-3-dev libx11-xcb-dev libxss-dev vulkan-sdk \
  libwayland-dev libwayland-bin wayland-protocols libxkbcommon-dev libdecor-0-dev \
  libasound2-dev libpulse-dev libpipewire-0.3-dev mesa-vulkan-drivers
clang --version | head -1
cmake --version | head -1
ldd --version | head -1
