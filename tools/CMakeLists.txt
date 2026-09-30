# Build helper, configured before the game: reads default.xex from your game files for the recompiler.
cmake_minimum_required(VERSION 3.25)
project(ewj_hd_tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(rexglue REQUIRED CONFIG)

add_executable(ewj_hd_extract extract_xex.cpp)
target_link_libraries(ewj_hd_extract PRIVATE rex::runtime)
