#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for xmi2mid.
#
cmake_minimum_required(VERSION 3.8)

project(xmi2mid)

add_executable(${PROJECT_NAME} "xmi2mid.cpp")

target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../audio")

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} audio)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "uwadv/tools")
