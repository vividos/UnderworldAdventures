#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
#
# CMakeList.txt: CMake project for game.
#
cmake_minimum_required(VERSION 3.8)

project(game)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"BasicGame.cpp" "BasicGame.hpp"
	"DebugServer.cpp" "DebugServer.hpp"
	"GameConfigLoader.cpp" "GameConfigLoader.hpp"
	"LevelEditor.cpp" "LevelEditor.hpp"
)

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE "${PROJECT_SOURCE_DIR}/../import"
	PRIVATE "${PROJECT_SOURCE_DIR}/../script"
	PRIVATE "${PROJECT_SOURCE_DIR}/../physics"
	PRIVATE "${PROJECT_SOURCE_DIR}/../renderer"
	PRIVATE "${PROJECT_SOURCE_DIR}/../underworld"
	PRIVATE "${PROJECT_SOURCE_DIR}/../ui"
)

target_link_libraries(${PROJECT_NAME} base import underworld renderer ui)
