#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for convdec.
#
cmake_minimum_required(VERSION 3.8)

project(convdec)

add_executable(${PROJECT_NAME}
	"convdec.cpp"
	"Decompiler.cpp" "Decompiler.hpp")

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../conv"
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} SDL2::SDL2main base conv import underworld)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "uwadv/tools")
