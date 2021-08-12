#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for script.
#
cmake_minimum_required(VERSION 3.8)

project(script)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"CreateCharacterLuaScripting.cpp" "CreateCharacterLuaScripting.hpp"
	"IScripting.hpp"
	"LuaCodeDebugger.cpp" "LuaCodeDebugger.hpp"
	"LuaScripting.cpp" "LuaScripting.hpp"
	"LuaState.cpp" "LuaState.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} base underworld lua)
