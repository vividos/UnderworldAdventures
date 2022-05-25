#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
#
# CMakeList.txt: CMake project for SDL_pnglite.
#
cmake_minimum_required(VERSION 3.12)

project(SDL_pnglite)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../SDL2-2.0.18)
find_package(SDL2 REQUIRED)

find_package(ZLIB REQUIRED)

add_library(${PROJECT_NAME} STATIC
	"pnglite.c" "pnglite.h"
	"SDL_pnglite.c" "SDL_pnglite.h")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE "${ZLIB_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
