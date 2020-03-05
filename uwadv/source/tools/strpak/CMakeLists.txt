#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# CMakeList.txt: CMake project for strpak.
#
cmake_minimum_required(VERSION 3.8)

project(strpak)

# Add source to this project's executable.
add_executable(strpak "strpak.cpp")

target_link_libraries(strpak ${SDL2_LIBRARIES} base import underw)
