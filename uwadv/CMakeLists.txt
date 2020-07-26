#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# CMakeList.txt: Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.8)

project("UnderworldAdventures")

set(CMAKE_CXX_STANDARD 17)

# Configure SDL2.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/source/thirdparty/SDL2-2.0.10)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Configure common paths.
include_directories("source")

# Include sub-projects.
add_subdirectory("source/base")
add_subdirectory("source/import")
add_subdirectory("source/underworld")
add_subdirectory("source/thirdparty/zziplib-0.13.69")
add_subdirectory("source/tools/strpak")
