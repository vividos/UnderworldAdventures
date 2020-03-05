#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# CMakeList.txt: CMake project for base.
#
cmake_minimum_required(VERSION 3.8)

project(base)

# Configure zlib.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../thirdparty/zlib1211)
find_package(zlib REQUIRED)

# Add source to this project's library.
add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"ArchiveFile.cpp" "ArchiveFile.hpp"
	"Base.cpp" "Base.hpp"
	"ConfigFile.cpp" "ConfigFile.hpp"
	"Constants.hpp"
	"Exception.hpp"
	"File.cpp" "File.hpp"
	"FileSystem.cpp" "FileSystem.hpp"
	"Keymap.cpp" "Keymap.hpp"
	"KeyValuePairTextFileReader.cpp" "KeyValuePairTextFileReader.hpp"
	"Math.hpp"
	"Plane3d.hpp"
	"ResourceManager.cpp" "ResourceManager.hpp"
	"Savegame.cpp" "Savegame.hpp"
	"SDL_rwops_gzfile.c" "SDL_rwops_gzfile.h"
	"Settings.cpp" "Settings.hpp"
	"SettingsLoader.cpp"
	"String.cpp" "String.hpp"
	"TextFile.cpp" "TextFile.hpp"
	"Triangle3d.hpp"
	"Uw2decode.cpp" "Uw2decode.hpp"
	"Vector2d.hpp"
	"Vector3d.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../thirdparty/zziplib-0.13.69"
		"${ZLIB_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES} zziplib)
