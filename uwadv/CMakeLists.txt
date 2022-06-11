#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.15)

# Configure C/C++
enable_language(CXX C)
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_POLICY_DEFAULT_CMP0092 NEW)
	add_compile_options(/W4)
endif(MSVC)

project("UnderworldAdventures")

# Configure OpenGL
find_package(OpenGL REQUIRED)

# Configure SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Configure common paths.
include_directories("source")

# Include sub-projects.
add_subdirectory("source/base")
add_subdirectory("source/audio")
add_subdirectory("source/import")
add_subdirectory("source/underworld")
add_subdirectory("source/conv")
add_subdirectory("source/physics")
add_subdirectory("source/renderer")
add_subdirectory("source/screens")
add_subdirectory("source/script")
add_subdirectory("source/ui")
add_subdirectory("source/uwadv")
add_subdirectory("source/thirdparty/hqx")
add_subdirectory("source/thirdparty/lua")
add_subdirectory("source/thirdparty/SDL_pnglite")
add_subdirectory("source/tools/convdbg")
add_subdirectory("source/tools/convdec")
add_subdirectory("source/tools/strpak")
add_subdirectory("source/tools/uwdump")
add_subdirectory("source/tools/xmi2mid")
add_subdirectory("uadata")

install(
	FILES
		"source/win32/uwadv.cfg"
		${SDL2_RUNTIME_FILES}
	DESTINATION "uwadv")

install(
	FILES
		${SDL2_RUNTIME_FILES}
	DESTINATION "uwadv/tools")
