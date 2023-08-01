#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2022 Underworld Adventures Team
#
# CMakeList.txt: CMake project for renderer.
#
cmake_minimum_required(VERSION 3.8)

project(renderer)

# Configure OpenGL/OpenGLESv2
if(ANDROID)
	find_path(GLES2_INCLUDE_DIR GLES2/gl2.h
	    HINTS $ENV{ANDROID_NDK_HOME})
	find_library(GLES2_LIBRARY libGLESv2.so
	    HINTS ${GLES2_INCLUDE_DIR}/../lib)
	include_directories(${GLES2_INCLUDE_DIR})
else()
	find_package(OpenGL REQUIRED)
endif()

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"Critter.cpp" "Critter.hpp"
	"CritterFramesManager.cpp" "CritterFramesManager.hpp"
	"LevelTilemapRenderer.cpp" "LevelTilemapRenderer.hpp"
	"MainGameLoop.cpp" "MainGameLoop.hpp"
	"Model3D.hpp"
	"Model3DBuiltin.cpp" "Model3DBuiltin.hpp"
	"Model3DManager.cpp" "Model3DManager.hpp"
	"Model3DVrml.cpp" "Model3DVrml.hpp"
	"PolygonTessellator.cpp" "PolygonTessellator.hpp"
	"Quadtree.cpp" "Quadtree.hpp"
	"OpenGL.cpp" "OpenGL.hpp"
	"Renderer.cpp" "Renderer.hpp"
	"RenderOptions.hpp"
	"RenderWindow.cpp" "RenderWindow.hpp"
	"Scaler.cpp" "Scaler.hpp"
	"Texture.cpp" "Texture.hpp"
	"TextureManager.cpp" "TextureManager.hpp"
	"UnderworldRenderer.cpp" "UnderworldRenderer.hpp"
	"VertexBufferObject.cpp" "VertexBufferObject.hpp"
	"Viewport.cpp" "Viewport.hpp")

if(ANDROID)
add_library(${PROJECT_NAME} STATIC
)
else()
add_library(${PROJECT_NAME} STATIC
	"fixed/GluPolygonTessellatorImpl.cpp" "fixed/GluPolygonTessellatorImpl.hpp"
)
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} base import hqx physics ui underworld)

if(ANDROID)
	target_link_libraries(${PROJECT_NAME} ${GLES2_LIBRARY})
else()
	target_link_libraries(${PROJECT_NAME} opengl32 glu32)
endif()
