#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
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
	"IngameControls.hpp"
	"MapViewScreen.cpp" "MapViewScreen.hpp"
	"OriginalIngameScreen.cpp" "OriginalIngameScreen.hpp"
	"Panel.cpp" "Panel.hpp"
	"SaveGameScreen.cpp" "SaveGameScreen.hpp"
	"StartMenuScreen.cpp" "StartMenuScreen.hpp"
	"StartSplashScreen.cpp" "StartSplashScreen.hpp"
	"UwadvMenuScreen.cpp" "UwadvMenuScreen.hpp"
	"controls/Ingame3DView.cpp" "controls/Ingame3DView.hpp"
	"controls/IngameCommandButtons.cpp" "controls/IngameCommandButtons.hpp"
	"controls/IngameCompass.cpp" "controls/IngameCompass.hpp"
	"controls/IngameDragon.cpp" "controls/IngameDragon.hpp"
	"controls/IngameFlask.cpp" "controls/IngameFlask.hpp"
	"controls/IngameGargoyleEyes.cpp" "controls/IngameGargoyleEyes.hpp"
	"controls/IngameMoveArrows.cpp" "controls/IngameMoveArrows.hpp"
	"controls/IngamePowerGem.cpp" "controls/IngamePowerGem.hpp"
	"controls/IngameRuneshelf.cpp" "controls/IngameRuneshelf.hpp"
	"controls/IngameSpellArea.cpp" "controls/IngameSpellArea.hpp"
	"controls/OriginalIngameControl.cpp" "controls/OriginalIngameControl.hpp"
)

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} base audio conv import renderer script ui underworld)
