#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for convdbg.
#
cmake_minimum_required(VERSION 3.8)

project(convdbg)

add_executable(${PROJECT_NAME} "convdbg.cpp" "convdbg.hpp")

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../conv"
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} base conv import underworld)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "uwadv/tools")
