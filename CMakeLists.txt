# ewj_hd - Earthworm Jim HD PC port.
#
# generated/rexglue.cmake and the recompiled sources come from `rexglue codegen`,
# which the build scripts run before configuring this project.

cmake_minimum_required(VERSION 3.25)
project(ewj_hd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(generated/rexglue.cmake)

# Sources
set(EWJHD_SOURCES
    src/main.cpp
    src/game_menu.cpp
)

if(WIN32)
    add_executable(ewj_hd WIN32 ${EWJHD_SOURCES})
else()
    add_executable(ewj_hd ${EWJHD_SOURCES})
endif()

rexglue_setup_target(ewj_hd GPU_PLUGINS xenos)
