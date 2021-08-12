#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for ui.
#
cmake_minimum_required(VERSION 3.8)

project(ui)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"Cutscene.cpp" "Cutscene.hpp"
	"FadingHelper.hpp"
	"Font.cpp" "Font.hpp"
	"ImageManager.cpp" "ImageManager.hpp"
	"ImageQuad.cpp" "ImageQuad.hpp"
	"IndexedImage.cpp" "IndexedImage.hpp"
	"MouseCursor.cpp" "MouseCursor.hpp"
	"Palette256.cpp" "Palette256.hpp"
	"Screen.cpp" "Screen.hpp"
	"TextEditWindow.cpp" "TextEditWindow.hpp"
	"TextScroll.cpp" "TextScroll.hpp"
	"Window.cpp" "Window.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../renderer"
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} base import)
