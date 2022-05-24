#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
#
# CMakeList.txt: CMake project for base.
#
cmake_minimum_required(VERSION 3.12)

project(base)

# Configure zlib
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../thirdparty/zlib1211)
find_package(zlib REQUIRED)
add_compile_definitions(ZLIB_WINAPI Z_LARGE64)

# ignore the compiler's warning about deprecated codecvt classes
add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"ArchiveFile.cpp" "ArchiveFile.hpp"
	"Base.cpp" "Base.hpp"
	"Color3ub.cpp" "Color3ub.hpp"
	"ConfigFile.cpp" "ConfigFile.hpp"
	"Constants.hpp"
	"Exception.hpp"
	"File.cpp" "File.hpp"
	"FileSystem.cpp" "FileSystem.hpp"
	"Keymap.cpp" "Keymap.hpp"
	"KeyValuePairTextFileReader.cpp" "KeyValuePairTextFileReader.hpp"
	"Math.hpp"
	"Path.cpp" "Path.hpp"
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
		"${ZLIB_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES} zziplib)
