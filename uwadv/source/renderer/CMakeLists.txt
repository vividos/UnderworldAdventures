#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for renderer.
#
cmake_minimum_required(VERSION 3.8)

project(renderer)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"Critter.cpp" "Critter.hpp"
	"LevelTilemapRenderer.cpp" "LevelTilemapRenderer.hpp"
	"MainGameLoop.cpp" "MainGameLoop.hpp"
	"Model3D.cpp" "Model3D.hpp"
	"Model3DBuiltin.cpp" "Model3DBuiltin.hpp"
	"Model3DVrml.cpp" "Model3DVrml.hpp"
	"PolygonTessellator.cpp" "PolygonTessellator.hpp"
	"Quadtree.cpp" "Quadtree.hpp"
	"Renderer.cpp" "Renderer.hpp"
	"RenderOptions.hpp"
	"RenderWindow.cpp" "RenderWindow.hpp"
	"Scaler.cpp" "Scaler.hpp"
	"Texture.cpp" "Texture.hpp"
	"TextureManager.cpp" "TextureManager.hpp"
	"UnderworldRenderer.cpp" "UnderworldRenderer.hpp"
	"Viewport.cpp" "Viewport.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} base import hqx physics ui underworld opengl32 glu32)
