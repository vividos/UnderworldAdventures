#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# CMakeList.txt: CMake project for zziplib.
#
cmake_minimum_required(VERSION 3.8)

project(zziplib)

# Configure zlib.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../zlib1211)
find_package(zlib REQUIRED)
add_compile_definitions(ZLIB_WINAPI Z_LARGE64)

file(GLOB_RECURSE zziplib_SRC
	"zzip/*.c" "zzip/*.h"
	"SDL/*.c" "SDL/*.h")

add_library(${PROJECT_NAME} STATIC ${zziplib_SRC})

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/SDL"
		"${ZLIB_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME} "${ZLIB_LIBRARIES}")
