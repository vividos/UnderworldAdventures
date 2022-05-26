#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for strpak.
#
cmake_minimum_required(VERSION 3.8)

project(strpak)

add_executable(${PROJECT_NAME} "strpak.cpp")

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} SDL2::SDL2main base import underworld)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "uwadv/tools")
