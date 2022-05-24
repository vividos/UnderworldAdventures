#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
#
# CMakeList.txt: CMake project for underworld.
#
cmake_minimum_required(VERSION 3.8)

project(underworld)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"ConvGlobals.cpp" "ConvGlobals.hpp"
	"GameLogic.cpp" "GameLogic.hpp"
	"GameStrings.cpp" "GameStrings.hpp"
	"Inventory.cpp" "Inventory.hpp"
	"Level.cpp" "Level.hpp"
	"LevelList.cpp" "LevelList.hpp"
	"MapNotes.cpp" "MapNotes.hpp"
	"Object.cpp" "Object.hpp"
	"ObjectList.cpp" "ObjectList.hpp"
	"Player.cpp" "Player.hpp"
	"Properties.hpp"
	"QuestFlags.cpp" "QuestFlags.hpp"
	"Runes.cpp" "Runes.hpp"
	"TileMap.cpp" "TileMap.hpp"
	"Underworld.cpp" "Underworld.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE "${PROJECT_SOURCE_DIR}/../script")

target_link_libraries(${PROJECT_NAME} base)
