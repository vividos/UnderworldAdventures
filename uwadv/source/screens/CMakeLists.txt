#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for screens.
#
cmake_minimum_required(VERSION 3.8)

project(screens)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"AcknowledgementsScreen.cpp" "AcknowledgementsScreen.hpp"
	"ConversationScreen.cpp" "ConversationScreen.hpp"
	"CreateCharacterScreen.cpp" "CreateCharacterScreen.hpp"
	"CutsceneViewScreen.cpp" "CutsceneViewScreen.hpp"
	"IngameControls.cpp" "IngameControls.hpp"
#	"MapViewScreen.cpp" "MapViewScreen.hpp"
	"OriginalIngameScreen.cpp" "OriginalIngameScreen.hpp"
	"Panel.cpp" "Panel.hpp"
	"SaveGameScreen.cpp" "SaveGameScreen.hpp"
	"StartMenuScreen.cpp" "StartMenuScreen.hpp"
	"StartSplashScreen.cpp" "StartSplashScreen.hpp"
	"UwadvMenuScreen.cpp" "UwadvMenuScreen.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} base audio conv import renderer script ui underworld)
