#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2022 Underworld Adventures Team
#
# CMakeList.txt: CMake project for Android platform.
#
cmake_minimum_required(VERSION 3.20)

# The Android app project consists of two parts:
# First, build the shared library that contains the app that uses SDL2
# Second, build the .apk/.aab using Gradle as build system, including the
# built shared library and the SDL2 libraries.

project(uwadv_android_apk NONE
	 VERSION 0.11.0)

add_custom_command(OUTPUT uwadv.apk
	 COMMAND "./gradlew.bat buildDebug"
	 COMMENT "Building Android .apk file")
