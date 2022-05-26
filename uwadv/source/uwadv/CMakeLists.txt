#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for uwadv.
#
cmake_minimum_required(VERSION 3.8)

project(uwadv)

set(uwadv_SRC
	"pch.cpp" "pch.hpp"
	"DebugServer.cpp" "DebugServer.hpp"
	"Game.cpp" "Game.hpp"
	"GameConfigLoader.cpp" "GameConfigLoader.hpp"
	"../IDebugServer.hpp"
	"main.cpp")

if (WIN32)
	set(uwadv_SRC ${uwadv_SRC}
		"../win32/CrashReporting.cpp" "../win32/CrashReporting.hpp"
		"../win32/resource.h"
		"../win32/Uus.ico"
		"../win32/uwadv_res.rc"
		"../win32/versioninfo.rc")
endif(WIN32)

add_executable(${PROJECT_NAME} ${uwadv_SRC})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} SDL2::SDL2main base conv import renderer screens script ui underworld)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "uwadv")
