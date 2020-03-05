#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2020 Underworld Adventures Team
#
# CMakeList.txt: CMake project for import.
#
cmake_minimum_required(VERSION 3.8)

project(import)

# Add source to this project's library.
add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"Bits.hpp"
	"ConvLoader.cpp" "ConvLoader.hpp"
	"CrittersLoader.cpp" "CrittersLoader.hpp"
	"CutsceneLoader.cpp" "CutsceneLoader.hpp"
	"FontLoader.cpp" "FontLoader.hpp"
	"GameStringsImporter.cpp" "GameStringsImporter.hpp"
	"ImageLoader.cpp" "ImageLoader.hpp"
	"Import.hpp"
	"ItemCombineLoader.cpp"
	"LevelImporter.cpp" "LevelImporter.hpp"
	"MapLoader.cpp"
	"ModelDecoder.cpp"
	"ObjectListLoader.cpp" "ObjectListLoader.hpp"
	"PlayerImporter.cpp" "PlayerImporter.hpp"
	"PropertiesLoader.cpp"
	"TextureLoader.cpp" "TextureLoader.hpp"
	"TgaImport.cpp"
	"UnderworldLoader.cpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../conv"
		"${PROJECT_SOURCE_DIR}/../renderer"
		"${PROJECT_SOURCE_DIR}/../ui"
		"${PROJECT_SOURCE_DIR}/../underw")

target_link_libraries(${PROJECT_NAME} base)
