#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002-2021 Underworld Adventures Team
#
# CMakeList.txt: CMake project for physics.
#
cmake_minimum_required(VERSION 3.8)

project(physics)

add_library(${PROJECT_NAME} STATIC
	"pch.cpp" "pch.hpp"
	"CollisionDetection.cpp" "CollisionDetection.hpp"
	"GeometryProvider.cpp" "GeometryProvider.hpp"
	"Pathfinder.cpp" "Pathfinder.hpp"
	"PhysicsBody.hpp"
	"PhysicsModel.cpp" "PhysicsModel.hpp"
	"PlayerPhysicsObject.cpp" "PlayerPhysicsObject.hpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC "${PROJECT_SOURCE_DIR}"
	PRIVATE
		"${PROJECT_SOURCE_DIR}/../underworld")

target_link_libraries(${PROJECT_NAME} base underworld)
