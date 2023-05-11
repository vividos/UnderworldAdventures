#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
#
# cpp/CMakeList.txt: CMake project for Android shared library.
#

# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Android Studio 2021.2.1 comes with 3.18.1
cmake_minimum_required(VERSION 3.18.1)

# since vcpkg can't be used in Android Studio (yet?), include the path to the
# installed and built packages for arm64-android, which is a community
# supported triple.
set(VCPKG_ROOT $ENV{VCPKG_ROOT})
string(REPLACE "\\" "/" VCPKG_ROOT ${VCPKG_ROOT})
list(APPEND CMAKE_MODULE_PATH ${VCPKG_ROOT}/installed/arm64-android/share/)

set(SDL2_DIR ${VCPKG_ROOT}/installed/arm64-android/share/SDL2/)

project("uwadv_android")

find_package(SDL2 REQUIRED)

add_library(${PROJECT_NAME} SHARED
    AndroidGame.cpp AndroidGame.hpp
    uwadv_android.cpp
)

set(UWADV_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../../../../../source")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${UWADV_SOURCE_DIR}/base"
		"${UWADV_SOURCE_DIR}/renderer"
		"${UWADV_SOURCE_DIR}/script"
		"${UWADV_SOURCE_DIR}/screens"
		"${UWADV_SOURCE_DIR}/ui"
		"${UWADV_SOURCE_DIR}/underworld"
		"${UWADV_SOURCE_DIR}/game")

find_library(
    log-lib
    log)

target_link_libraries(${PROJECT_NAME}
    # Links the target library to the log library
    # included in the NDK.
    ${log-lib}
    SDL2::SDL2 SDL2::SDL2main
    base conv import script underworld
)
